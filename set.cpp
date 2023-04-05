#include <algorithm>
#include <iostream>
#include <vector>
#include <utility>

enum Color { RED, BLACK };

template <typename ValueType>
constexpr bool operator==(const ValueType& first, const ValueType& second) {
    return !(first < second) && !(second < first);
}

template <typename T>
struct Node {
    T value;
    Node<T>* left;
    Node<T>* right;
    Node<T>* parent;
    Color color;
    size_t sub_size;

    Node() : value(), left(nullptr), right(nullptr), parent(nullptr), color(Color::RED), sub_size(1) {
    }
    explicit Node(const T& x)
        : value(x), left(nullptr), right(nullptr), parent(nullptr), color(Color::RED), sub_size(1) {
    }
    Node(T value, Node<T>* pa)
        : value(value), left(nullptr), right(nullptr), parent(pa), color(Color::RED), sub_size(1) {
    }
};

template <typename ValueType>
class RBTree {
public:
    static Node<ValueType>* mostLeft(Node<ValueType>* sub_tree) {
        if (!sub_tree) {
            return nullptr;
        }
        Node<ValueType>* current = sub_tree;
        while (current->left) {
            current = current->left;
        }
        return current;
    }

    static Node<ValueType>* mostRight(Node<ValueType>* sub_tree) {
        if (!sub_tree) {
            return nullptr;
        }
        Node<ValueType>* current = sub_tree;
        while (current->right) {
            current = current->right;
        }
        return current;
    }

    RBTree() : root(nullptr), size_(0) {
    }

    ~RBTree() {
        deleteNode(root);
    }

    ValueType eStatistic(size_t e) {
        Node<ValueType>* current = root;
        size_t cur_stat = getSize(current->left) + 1;
        while (cur_stat != e) {
            if (cur_stat > e) {
                current = current->left;
                cur_stat -= getSize(current->right) + 1;
            } else {
                current = current->right;
                cur_stat += getSize(current->left) + 1;
            }
        }
        return current->value;
    }

    void insert(const ValueType& value) {
        if (!root) {
            root = new Node<ValueType>(value);
            root->color = Color::BLACK;
            ++size_;
            return;
        }
        Node<ValueType>* current = root;
        insertNode(current, value);
    }

    void erase(const ValueType& value) {
        Node<ValueType>* to_del = findNode(value);
        if (!to_del) {
            return;
        }

        // Это лист.
        if (!to_del->right && !to_del->left) {
            delLeaf(to_del);
            // Вершина с двумя потомками.
        } else if (to_del->right && to_del->left) {
            Node<ValueType>* ios = mostRight(to_del->left);
            std::swap(ios->value, to_del->value);
            if (ios->left || ios->right) {
                delOnlyChild(ios);
            } else {
                delLeaf(ios);
            }
            // Чёрная вершина с одним потомком.
        } else {
            delOnlyChild(to_del);
        }
        --size_;
    }

    void delOnlyChild(Node<ValueType>* to_del) {
        // Только правый ребёнок.
        if (to_del->right) {
            std::swap(to_del->value, to_del->right->value);
            delLeaf(to_del->right);
            // Только левый ребёнок.
        } else {
            std::swap(to_del->value, to_del->left->value);
            delLeaf(to_del->left);
        }
    }

    void delLeaf(Node<ValueType>* to_del) {
        if (to_del->color == Color::RED) {
            auto pa = to_del->parent;
            if (pa->right == to_del) {
                pa->right = nullptr;
            } else {
                pa->left = nullptr;
            }
        } else {
            fixDB(to_del);
            if (to_del != root) {
                if (to_del->parent->left == to_del) {
                    to_del->parent->left = nullptr;
                } else {
                    to_del->parent->right = nullptr;
                }
            } else {
                root = nullptr;
            }
        }
        updateSize(to_del->parent);
        delete to_del;
    }

    void fixDB(Node<ValueType>* node) {
        if (node == root) {
            node->color = Color::BLACK;
            return;
        }

        Node<ValueType>* pa = node->parent;
        Node<ValueType>* sibling = pa->right == node ? pa->left : pa->right;
        Node<ValueType>* l_child = sibling->left;
        Node<ValueType>* r_child = sibling->right;
        bool is_left = pa->left == node;

        // Брат чёрный.
        if (sibling->color == Color::BLACK) {
            // Если удаляется левая вершина.
            if (is_left) {
                // Правый ребенок красный (левый - любой).
                if (r_child && r_child->color == Color::RED) {
                    sibling->color = pa->color;
                    pa->color = Color::BLACK;
                    r_child->color = Color::BLACK;
                    leftRotation(pa);
                    // Левый ребенок красный (правый - черный)
                } else if (l_child && l_child->color == Color::RED && isBlack(r_child)) {
                    l_child->color = Color::BLACK;
                    sibling->color = Color::RED;
                    rightRotation(sibling);
                    fixDB(node);
                    // Оба ребенка брата - черные.
                } else {
                    sibling->color = Color::RED;
                    if (pa->color == Color::RED) {
                        pa->color = Color::BLACK;
                    } else {
                        fixDB(pa);
                    }
                }
                // Если удаляется правая вершина.
            } else {
                // Левый ребенок красный (правый - любой).
                if (l_child && l_child->color == Color::RED) {
                    sibling->color = pa->color;
                    pa->color = Color::BLACK;
                    l_child->color = Color::BLACK;
                    rightRotation(pa);
                    // Правый ребенок красный (левый - черный)
                } else if (r_child && r_child->color == Color::RED && isBlack(l_child)) {
                    r_child->color = Color::BLACK;
                    sibling->color = Color::RED;
                    leftRotation(sibling);
                    fixDB(node);
                    // Оба ребенка брата - черные.
                } else {
                    sibling->color = Color::RED;
                    if (pa->color == Color::RED) {
                        pa->color = Color::BLACK;
                    } else {
                        fixDB(pa);
                    }
                }
            }
            // Брат красный.
        } else {
            pa->color = Color::RED;
            sibling->color = Color::BLACK;
            if (is_left) {
                leftRotation(pa);
            } else {
                rightRotation(pa);
            }
            fixDB(node);
        }
    }

    bool isBlack(Node<ValueType>* node) {
        return !node || node->color == Color::BLACK;
    }

    size_t size() const {
        return size_;
    }
    bool empty() const {
        return size_ == 0;
    }

    Node<ValueType>* findNode(const ValueType& value) const {
        Node<ValueType>* current = root;
        while (current && !(current->value == value)) {
            if (current->value < value) {
                current = current->right;
            } else {
                current = current->left;
            }
        }
        return current;
    }

    Node<ValueType>* root;

private:
    size_t size_;

    void deleteNode(Node<ValueType>* node) {
        if (!node) {
            return;
        }
        deleteNode(node->left);
        deleteNode(node->right);
        delete node;
    }

    void insertNode(Node<ValueType>* current, ValueType value) {
        if (!current || value == current->value) {
            return;
        }
        if (current->value < value) {
            if (current->right) {
                insertNode(current->right, value);
            } else {
                Node<ValueType>* new_node = new Node<ValueType>(value, current);
                current->right = new_node;
                updateSize(current->right);
                balance(new_node);
                ++size_;
            }
        } else {
            if (current->left) {
                insertNode(current->left, value);
            } else {
                Node<ValueType>* new_node = new Node<ValueType>(value, current);
                current->left = new_node;
                updateSize(current->left);
                balance(new_node);
                ++size_;
            }
        }
    }

    void rightRotation(Node<ValueType>* pivot) {
        Node<ValueType>* node = pivot->left;
        pivot->sub_size = pivot->sub_size - node->sub_size + getSize(node->right);
        node->sub_size = node->sub_size - getSize(node->right) + pivot->sub_size;
        if (pivot->parent) {
            if (pivot->parent->right == pivot) {
                pivot->parent->right = node;
            } else {
                pivot->parent->left = node;
            }
            node->parent = pivot->parent;
        } else {
            root = node;
            node->parent = nullptr;
        }
        pivot->left = node->right;
        if (node->right) {
            node->right->parent = pivot;
        }
        pivot->parent = node;
        node->right = pivot;
    }

    void leftRotation(Node<ValueType>* pivot) {
        Node<ValueType>* node = pivot->right;
        pivot->sub_size = pivot->sub_size - node->sub_size + getSize(node->left);
        node->sub_size = node->sub_size - getSize(node->left) + pivot->sub_size;
        if (pivot->parent) {
            if (pivot->parent->right == pivot) {
                pivot->parent->right = node;
            } else {
                pivot->parent->left = node;
            }
            node->parent = pivot->parent;
        } else {
            root = node;
            node->parent = nullptr;
        }
        pivot->right = node->left;
        if (node->left) {
            node->left->parent = pivot;
        }
        pivot->parent = node;
        node->left = pivot;
    }

    void balance(Node<ValueType>* son) {
        // Случай 3: корень красный.
        if (son == root) {
            son->color = Color::BLACK;
            return;
        }
        if (son->parent == root) {
            son->parent->color = Color::BLACK;
            return;
        }
        if (son->color == Color::BLACK || son->parent->color == Color::BLACK) {
            return;
        }

        Node<ValueType>* pa = son->parent;
        Node<ValueType>* grandpa = pa->parent;
        Node<ValueType>* uncle = pa == grandpa->right ? grandpa->left : grandpa->right;

        // Случай 1 (и 2): дядя красный.
        if (uncle && uncle->color == Color::RED) {
            pa->color = Color::BLACK;
            uncle->color = Color::BLACK;
            grandpa->color = Color::RED;
            balance(grandpa);
        } else {
            // Случай 4 (и 5): дядя чёрный или его нет.
            if (grandpa->left == pa) {
                if (pa->right == son) {
                    leftRotation(pa);
                    std::swap(pa, son);
                }
                rightRotation(grandpa);
            } else {
                if (pa->left == son) {
                    rightRotation(pa);
                    std::swap(pa, son);
                }
                leftRotation(grandpa);
            }
            grandpa->color = Color::RED;
            pa->color = Color::BLACK;
        }
    }

    void copyChildren(Node<ValueType>* new_current, Node<ValueType>* current) {
        if (!new_current) {
            return;
        }

        if (current->left) {
            new_current->left = new Node<ValueType>(current->left->value, new_current);
            new_current->left->color = current->left->color;
            copyChildren(new_current->left, current->left);
        } else {
            new_current->left = nullptr;
        }

        if (current->right) {
            new_current->right = new Node<ValueType>(current->right->value, new_current);
            new_current->right->color = current->right->color;
            copyChildren(new_current->right, current->right);
        } else {
            new_current->right = nullptr;
        }
    }

    void updateSize(Node<ValueType>* node) {
        if (!node) {
            return;
        }
        node->sub_size = getSize(node->right) + getSize(node->left) + 1;
        updateSize(node->parent);
    }

    size_t getSize(Node<ValueType>* node) const {
        return node ? node->sub_size : 0;
    }
};
