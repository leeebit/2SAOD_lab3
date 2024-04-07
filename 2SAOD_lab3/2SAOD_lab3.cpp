#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>

struct Rectangle {
    int x1, y1, x2, y2;

    bool operator==(const Rectangle& other) const {
        return x1 == other.x1 && y1 == other.y1 && x2 == other.x2 && y2 == other.y2;
    }
};

struct Node {
    bool leaf;
    std::vector<Rectangle> rects;
    std::vector<Node*> children;
};

double area(Rectangle r) {
    return (r.x2 - r.x1) * (r.y2 - r.y1);
}

Rectangle unionRectangles(Rectangle r1, Rectangle r2) {
    return { std::min(r1.x1, r2.x1), std::min(r1.y1, r2.y1), std::max(r1.x2, r2.x2), std::max(r1.y2, r2.y2) };
}

bool compareRectangles(Rectangle r1, Rectangle r2) {
    return area(r1) < area(r2);
}

class RTree {
private:
    Node* root;

public:
    RTree() {
        root = new Node();
        root->leaf = true;
    }

    void insert(Rectangle rect) {
        insert(root, rect);
    }

    void remove(Rectangle rect) {
        remove(root, rect);
    }

    Rectangle nearestNeighbor(Rectangle rect) {
        return nearestNeighbor(root, rect);
    }

private:
    void insert(Node* node, Rectangle rect) {
        if (node->leaf) {
            node->rects.push_back(rect);
            if (node->rects.size() > 5) {
                split(node);
            }
        }
        else {
            Node* child = chooseSubtree(node, rect);
            insert(child, rect);
        }
    }

    Node* chooseSubtree(Node* node, Rectangle rect) {
        double minIncrease = std::numeric_limits<double>::max();
        Node* bestChild = nullptr;
        for (Node* child : node->children) {
            double areaBefore = area(child->rects[0]);
            double areaAfter = area(unionRectangles(child->rects[0], rect));
            double increase = areaAfter - areaBefore;
            if (increase < minIncrease) {
                minIncrease = increase;
                bestChild = child;
            }
        }
        return bestChild;
    }

    void split(Node* node) {
        std::sort(node->rects.begin(), node->rects.end(), compareRectangles);
        Node* newNode = new Node();
        int splitIndex = node->rects.size() / 2;
        newNode->rects.assign(node->rects.begin() + splitIndex, node->rects.end());
        node->rects.erase(node->rects.begin() + splitIndex, node->rects.end());
        if (!node->leaf) {
            newNode->children.assign(node->children.begin() + splitIndex, node->children.end());
            node->children.erase(node->children.begin() + splitIndex, node->children.end());
            newNode->leaf = false;
        }
    }

    bool remove(Node* node, Rectangle rect) {
        if (node->leaf) {
            auto it = std::find(node->rects.begin(), node->rects.end(), rect);
            if (it != node->rects.end()) {
                node->rects.erase(it);
                return true;
            }
            return false;
        }
        else {
            for (Node* child : node->children) {
                if (overlap(child->rects[0], rect)) {
                    if (remove(child, rect)) {
                        if (child->rects.empty()) {
                            auto it = std::find(node->children.begin(), node->children.end(), child);
                            node->children.erase(it);
                            delete child;
                        }
                        return true;
                    }
                }
            }
            return false;
        }
    }

    Rectangle nearestNeighbor(Node* node, Rectangle rect) {
        if (node->leaf) {
            double minDist = std::numeric_limits<double>::max();
            Rectangle nearest;
            for (Rectangle r : node->rects) {
                double dist = distance(r, rect);
                if (dist < minDist) {
                    minDist = dist;
                    nearest = r;
                }
            }
            return nearest;
        }
        else {
            double minDist = std::numeric_limits<double>::max();
            Rectangle nearest;
            for (Node* child : node->children) {
                Rectangle childNearest = nearestNeighbor(child, rect);
                double dist = distance(childNearest, rect);
                if (dist < minDist) {
                    minDist = dist;
                    nearest = childNearest;
                }
            }
            return nearest;
        }
    }


    bool overlap(Rectangle r1, Rectangle r2) {
        return !(r1.x1 > r2.x2 || r1.x2 < r2.x1 || r1.y1 > r2.y2 || r1.y2 < r2.y1);
    }

    double distance(Rectangle r1, Rectangle r2) {
        int dx = std::max(std::max(r1.x1 - r2.x2, 0), std::max(r2.x1 - r1.x2, 0));
        int dy = std::max(std::max(r1.y1 - r2.y2, 0), std::max(r2.y1 - r1.y2, 0));
        return std::sqrt(dx * dx + dy * dy);
    }
};

int main()
{
    system("chcp 1251");

    RTree tree;

    // Добавляем несколько прямоугольников
    tree.insert({ 0, 0, 1, 1 });
    tree.insert({ 2, 2, 3, 3 });
    tree.insert({ 4, 4, 5, 5 });
    std::cout << "Добавили три прямоугольника в R-дерево.\n";

    // Ищем ближайший прямоугольник к заданному
    Rectangle nearest = tree.nearestNeighbor({ 1, 1, 2, 2 });
    std::cout << "Ближайший прямоугольник к (1, 1, 2, 2): (" << nearest.x1 << ", " << nearest.y1 << ", " << nearest.x2 << ", " << nearest.y2 << ")\n";

    // Удаляем прямоугольник
    tree.remove({ 0, 0, 1, 1 });
    std::cout << "Удалили прямоугольник (0, 0, 1, 1).\n";

    // Проверяем, что прямоугольник был удален
    nearest = tree.nearestNeighbor({ 0, 0, 1, 1 });
    std::cout << "Теперь ближайший прямоугольник к (0, 0, 1, 1): (" << nearest.x1 << ", " << nearest.y1 << ", " << nearest.x2 << ", " << nearest.y2 << ")\n";

    return 0;
}
