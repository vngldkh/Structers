#include <algorithm>

struct Node {
    int height;
    Node* left;
    Node* right;
    int value;
    int balance_factor;

    Node(int val) : height(1), left(nullptr), right(nullptr), value(val), balance_factor(0) {
    }
};

class AVLTree {
    Node* root_;
    int size_;

    void deleteNode(Node* node) {
        if (node == nullptr) {
            return;
        }
        deleteNode(node->left);
        deleteNode(node->right);
        delete node;
    }

    int getDepth(Node* node) {
        if (!node) {
            return 0;
        }
        return node->height;
    }

    bool isBalanced(Node* node) {
        return !node || abs(node->balance_factor) <= 1;
    }

    void update(Node* node) {
        if (!node) {
            return;
        }
        int r_height = getDepth(node->right);
        int l_height = getDepth(node->left);
        node->balance_factor = r_height - l_height;
        node->height = std::max(r_height, l_height) + 1;
    }

    void rightRotation(Node* grandpa, Node* pa, bool is_right) {
        Node* node = pa->left;
        if (grandpa) {
            if (is_right) {
                grandpa->right = node;
            } else {
                grandpa->left = node;
            }
        }
        pa->left = node->right;
        node->right = pa;
        update(pa);
        update(node);
    }

    void leftRotation(Node* grandpa, Node* pa, bool is_right) {
        Node* node = pa->right;
        if (grandpa) {
            if (is_right) {
                grandpa->right = node;
            } else {
                grandpa->left = node;
            }
        }
        pa->right = node->left;
        node->left = pa;
        update(pa);
        update(node);
    }

    void balance(Node* node) {
        if (!isBalanced(node->left)) {
            if (node->left->balance_factor > 0) {
                if (node->left->right->balance_factor < 0) {
                    rightRotation(node->left, node->left->right, true);
                }
                leftRotation(node, node->left, false);
            } else {
                if (node->left->left->balance_factor > 0) {
                    leftRotation(node->left, node->left->left, false);
                }
                rightRotation(node, node->left, false);
            }
        }
        if (!isBalanced(node->right)) {
            if (node->right->balance_factor > 0) {
                if (node->right->right->balance_factor < 0) {
                    rightRotation(node->right, node->right->right, true);
                }
                leftRotation(node, node->right, true);
            } else {
                if (node->right->left->balance_factor > 0) {
                    leftRotation(node->right, node->right->left, false);
                }
                rightRotation(node, node->right, true);
            }
        }
        update(node);
    }

    void balanceRoot() {
        Node* root = root_;
        if (!isBalanced(root)) {
            if (root->balance_factor > 0) {
                if (root->right->balance_factor < 0) {
                    rightRotation(root, root->right, false);
                }
                root_ = root->right;
                leftRotation(nullptr, root, false);
            } else {
                if (root->left->balance_factor > 0) {
                    leftRotation(root, root->left, false);
                }
                root_ = root->left;
                rightRotation(nullptr, root, false);
            }
        }
        update(root_);
    }

    void add(Node* current, int new_val) {
        if (current->value == new_val) {
            return;
        }

        if (new_val > current->value) {
            if (current->right == nullptr) {
                current->right = new Node(new_val);
                ++size_;
            } else {
                add(current->right, new_val);
            }
        } else {
            if (current->left == nullptr) {
                current->left = new Node(new_val);
                ++size_;
            } else {
                add(current->left, new_val);
            }
        }

        int l_depth, r_depth;
        l_depth = getDepth(current->left);
        r_depth = getDepth(current->right);
        current->balance_factor = r_depth - l_depth;
        balance(current);
    }

    Node* maxLess(Node* start) {
        Node* current = start->left;
        while (current->right && current->right->right) {
            current = current->right;
        }
        return current;
    }

    void newHeight(Node* border, Node* current) {
        if (current == border) {
            current->height = std::max(getDepth(current->left), getDepth(current->right)) + 1;
            return;
        }
        if (current != border && border->value > current->value) {
            newHeight(border, current->right);
        } else {
            newHeight(border, current->left);
        }
        current->height = std::max(getDepth(current->left), getDepth(current->right)) + 1;
    }

    void remove(Node* pa, Node* to_del, bool is_right) {
        --size_;
        // Если это лист.
        if (!to_del->left && !to_del->right) {
            if (pa) {
                if (is_right) {
                    pa->right = nullptr;
                } else {
                    pa->left = nullptr;
                }
                newHeight(pa, root_);
            } else {
                root_ = nullptr;
            }
            delete to_del;
            return;
        }

        // Только правый потомок.
        if (!to_del->left) {
            if (pa) {
                if (is_right) {
                    pa->right = to_del->right;
                } else {
                    pa->left = to_del->right;
                }
                newHeight(pa, root_);
            } else {
                root_ = to_del->right;
            }
            delete to_del;
            return;
        }

        // Только левый потомок.
        if (!to_del->right) {
            if (pa) {
                if (is_right) {
                    pa->right = to_del->left;
                } else {
                    pa->left = to_del->left;
                }
                newHeight(pa, root_);
            } else {
                root_ = to_del->left;
            }
            delete to_del;
            return;
        }

        // Оба потомка.
        Node* node = maxLess(to_del);
        Node* replace;
        if (!node->right) {
            replace = node;
        } else {
            replace = node->right;
            node->right = replace->left;
            replace->left = to_del->left;
        }

        if (pa) {
            if (is_right) {
                pa->right = replace;
            } else {
                pa->left = replace;
            }
        } else {
            root_ = replace;
        }
        replace->right = to_del->right;
        newHeight(node, root_);
        delete to_del;
    }

    void findToDel(Node* node, int val) {
        if (!node) {
            return;
        }
        if (val > node->value) {
            if (!node->right) {
                return;
            }
            if (node->right->value != val) {
                findToDel(node->right, val);
            } else {
                remove(node, node->right, true);
            }
        } else if (val < node->value) {
            if (!node->left) {
                return;
            }
            if (node->left->value != val) {
                findToDel(node->left, val);
            } else {
                remove(node, node->left, false);
            }
        } else {
            // Сюда попадаем, если значение находится в корне.
            remove(nullptr, node, false);
            return;
        }
        balance(node);
    }

    void subMas(Node* sub_tree, int* mas, int& curr) const {
        if (!sub_tree) {
            return;
        }
        if (sub_tree->left) {
            subMas(sub_tree->left, mas, curr);
        }
        mas[curr++] = sub_tree->value;
        if (sub_tree->right) {
            subMas(sub_tree->right, mas, curr);
        }
    }

public:
    AVLTree() : root_(nullptr), size_(0) {
    }

    int getHeight() {
        return getDepth(root_);
    }

    void insert(int value) {
        if (root_ == nullptr) {
            ++size_;
            root_ = new Node(value);
        } else {
            add(root_, value);
            balanceRoot();
        }
    }

    void erase(int value) {
        findToDel(root_, value);
        balanceRoot();
    }

    int* find(int value) {
        Node* current = root_;
        while (current && current->value != value) {
            if (current->value > value) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        if (!current) {
            return nullptr;
        }
        return &current->value;
    }

    int* traversal() {
        int* mas = new int[size_];
        int counter = 0;
        subMas(root_, mas, counter);
        return mas;
    }

    int* lowerBound(int value) const {
        Node* current = root_;
        Node* res = nullptr;
        res = lowerBound(value, current, res);
        return res ? &res->value : nullptr;
    }

    Node* lowerBound(int value, Node* current, Node* res) const {
        if (current == nullptr) {
            return res;
        }
        if (current->value < value) {
            res = lowerBound(value, current->right, res);
        } else {
            res = lowerBound(value, current->left, current);
        }
        return res;
    }

    bool empty() {
        return size_ == 0;
    }

    Node* getRoot() {
        return root_;
    }

    int getSize() const {
        return size_;
    }

    ~AVLTree() {
        deleteNode(root_);
    }
};