#include <algorithm>
#include <iostream>
#include <vector>
#include <utility>

enum Color { RED, BLACK, DOUBLE_BLACK };

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

    Node() : value(), left(nullptr), right(nullptr), parent(nullptr), color(Color::RED) {
    }
    explicit Node(const T& x)
        : value(x), left(nullptr), right(nullptr), parent(nullptr), color(Color::RED) {
    }
    Node(T value, Node<T>* pa)
        : value(value), left(nullptr), right(nullptr), parent(pa), color(Color::RED) {
    }
};

template <typename ValueType>
class RBTree {
public:
    struct Iterator {
        Iterator() : current_(nullptr), root_(nullptr) {
        }

        explicit Iterator(const Node<ValueType>* node) {
            current_ = node;
            if (!node) {
                root_ = nullptr;
                return;
            }
            root_ = node;
            while (root_->parent) {
                root_ = node->parent;
            }
        }

        Iterator(const Node<ValueType>* node, const Node<ValueType>* root)
            : current_(node), root_(root) {
        }

        const ValueType& operator*() const {
            return current_->value;
        }
        const ValueType* operator->() const {
            return &(current_->value);
        }

        Iterator& operator++() {
            if (!current_) {
                current_ = mostLeft(root_);
                return *this;
            }
            if (!current_->right) {
                while (current_->parent && current_->parent->right == current_) {
                    current_ = current_->parent;
                }
                current_ = current_->parent;
            } else {
                current_ = mostLeft(current_->right);
            }
            return *this;
        }
        Iterator operator++(int) {
            Iterator copy_it(this->current_);
            ++(*this);
            return copy_it;
        }

        Iterator& operator--() {
            if (!current_) {
                current_ = mostRight(root_);
                return *this;
            }
            if (!current_->left) {
                while (current_->parent && current_->parent->left == current_) {
                    current_ = current_->parent;
                }
                current_ = current_->parent;
            } else {
                current_ = mostRight(current_->left);
            }
            return *this;
        }
        Iterator operator--(int) {
            Iterator copy_it(this->current_, root_);
            --(*this);
            return copy_it;
        }

        bool operator==(const Iterator& other) const {
            return current_ == other.current_ && root_ == other.root_;
        }
        bool operator!=(const Iterator& other) const {
            return current_ != other.current_ || root_ != other.root_;
        }

        static const Node<ValueType>* mostLeft(const Node<ValueType>* sub_tree) {
            if (!sub_tree) {
                return nullptr;
            }
            const Node<ValueType>* current = sub_tree;
            while (current->left) {
                current = current->left;
            }
            return current;
        }

        static const Node<ValueType>* mostRight(const Node<ValueType>* sub_tree) {
            if (!sub_tree) {
                return nullptr;
            }
            const Node<ValueType>* current = sub_tree;
            while (current->right) {
                current = current->right;
            }
            return current;
        }

    private:
        const Node<ValueType>* current_;
        const Node<ValueType>* root_;
    };

    RBTree() : root(nullptr), size_(0) {
    }
    RBTree(std::initializer_list<ValueType> list) : root(nullptr), size_(0) {
        for (ValueType val : list) {
            insert(val);
        }
    }
    RBTree(const RBTree& other) {
        if (!other.root) {
            root = nullptr;
            size_ = 0;
            return;
        }
        root = new Node<ValueType>(other.root->value);
        root->color = Color::BLACK;
        size_ = other.size_;
        copyChildren(root, other.root);
    }

    RBTree<ValueType>& operator=(const RBTree& other) {
        if (!other.root) {
            root = nullptr;
            size_ = 0;
            return *this;
        }
        root = new Node<ValueType>(other.root->value);
        root->color = Color::BLACK;
        size_ = other.size_;
        copyChildren(root, other.root);
        return *this;
    }

    ~RBTree() {
        deleteNode(root);
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
    }

    size_t size() const {
        return size_;
    }
    bool empty() const {
        return size_ == 0;
    }

    Iterator lowerBound(const ValueType& value) const {
        if (!root) {
            return Iterator(nullptr, root);
        }
        Node<ValueType>* current = root;
        while (current->right && current->value < value) {
            current = current->right;
        }
        if (current->value < value) {
            return Iterator(nullptr, root);
        }
        while (current->left && value < current->left->value) {
            current = current->left;
        }
        if (current->left && value == current->left->value) {
            current = current->left;
        }
        return Iterator(current, root);
    }

    Iterator true_lowerBound(const ValueType& value) const {
        auto it = begin();
        for (; it != end(); ++it) {
            if (value < *it || value == *it) {
                return it;
            }
        }
        return it;
    }

    Iterator find(const ValueType& value) const {
        Node<ValueType>* current = root;
        while (current && !(current->value == value)) {
            if (current->value < value) {
                current = current->right;
            } else {
                current = current->left;
            }
        }
        return Iterator(current, root);
    }

    Iterator begin() const {
        return Iterator(Iterator::mostLeft(root), root);
    }
    Iterator end() const {
        return Iterator(nullptr, root);
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
                balance(new_node);
                ++size_;
            }
        } else {
            if (current->left) {
                insertNode(current->left, value);
            } else {
                Node<ValueType>* new_node = new Node<ValueType>(value, current);
                current->left = new_node;
                balance(new_node);
                ++size_;
            }
        }
    }

    void rightRotation(Node<ValueType>* pivot) {
        Node<ValueType>* node = pivot->left;
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
            new_current->left = nullptr;
        }
    }
};
