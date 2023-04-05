#include <iostream>
#include <tuple>
#include <utility>

enum Color { RED, BLACK };

template <typename ValueType>
constexpr bool operator==(const ValueType& first, const ValueType& second) {
    return !(first < second) && !(second < first);
}

template <typename K, typename V>
struct Node {
    K key;
    V value;
    Node<K, V>* left;
    Node<K, V>* right;
    Node<K, V>* parent;
    Color color;

    Node() : key(), value(), left(nullptr), right(nullptr), parent(nullptr), color(Color::RED) {
    }
    explicit Node(const K& key, const V& value)
        : key(key),
          value(value),
          left(nullptr),
          right(nullptr),
          parent(nullptr),
          color(Color::RED) {
    }
    Node(K key, V value, Node<K, V>* pa)
        : key(key), value(value), left(nullptr), right(nullptr), parent(pa), color(Color::RED) {
    }
};

template <typename K, typename V>
class Map {
public:
    struct Iterator {
        Iterator() : current_(nullptr), root_(nullptr) {
        }
        explicit Iterator(Node<K, V>* node) : current_(node), root_(nullptr) {
        }
        Iterator(Node<K, V>* node, Node<K, V>* root) : current_(node), root_(root) {
        }

        const std::pair<K, V>& operator*() const {
            return *new std::pair<K, V>(current_->key, current_->value);
        }
        const std::pair<K, V>* operator->() const {
            if (!current_) {
                return nullptr;
            }
            return new std::pair<K, V>(current_->key, current_->value);
        }

        Iterator& operator++() {
            if (!current_) {
                if (!root_) {
                    return *this;
                }
                current_ = mostLeft(root_);
                return *this;
            }
            if (!current_->right) {
                while (current_->parent && current_->parent->right == current_) {
                    current_ = current_->parent;
                }
                if (!current_->parent) {
                    root_ = current_;
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
                if (!root_) {
                    return *this;
                }
                current_ = mostRight(root_);
                return *this;
            }
            if (!current_->left) {
                while (current_->parent && current_->parent->left == current_) {
                    current_ = current_->parent;
                }
                if (!current_->parent) {
                    root_ = current_;
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

        static Node<K, V>* mostLeft(Node<K, V>* sub_tree) {
            if (!sub_tree) {
                return nullptr;
            }
            Node<K, V>* current = sub_tree;
            while (current->left) {
                current = current->left;
            }
            return current;
        }

        static Node<K, V>* mostRight(Node<K, V>* sub_tree) {
            if (!sub_tree) {
                return nullptr;
            }
            Node<K, V>* current = sub_tree;
            while (current->right) {
                current = current->right;
            }
            return current;
        }

        Node<K, V>* current_;

    private:
        Node<K, V>* root_;
    };

    Map() : root(nullptr), size_(0) {
    }
    Map(std::initializer_list<std::pair<K, V>> list) {
        for (auto [key, val] : list) {
            insert(key, val);
        }
    }
    Map(const Map& other) {
        if (!other.root) {
            root = nullptr;
            size_ = 0;
            return;
        }
        root = new Node<K, V>(other.root->key, other.root->value);
        root->color = Color::BLACK;
        size_ = other.size_;
        copyChildren(root, other.root);
    }

    Map<K, V>& operator=(const Map& other) {
        if (!other.root) {
            root = nullptr;
            size_ = 0;
            return *this;
        }
        root = new Node<K, V>(other.root->key, other.root->value);
        root->color = Color::BLACK;
        size_ = other.size_;
        copyChildren(root, other.root);
        return *this;
    }

    ~Map() {
        deleteNode(root);
    }

    void insert(const K& key, const V& value) {
        if (!root) {
            root = new Node<K, V>(key, value);
            root->color = Color::BLACK;
            ++size_;
            return;
        }
        Node<K, V>* current = root;
        insertNode(current, key, value);
    }

    void erase(const K& key) {
        Node<K, V>* to_del = findNode(key);
        if (!to_del) {
            return;
        }

        // Это лист.
        if (!to_del->right && !to_del->left) {
            delLeaf(to_del);
            // Вершина с двумя потомками.
        } else if (to_del->right && to_del->left) {
            Node<K, V>* ios = Iterator::mostRight(to_del->left);
            std::swap(ios->key, to_del->key);
            std::swap(ios->value, to_del->value);
            delLeaf(ios);
            // Чёрная вершина с одним потомком.
        } else {
            // Только правый ребёнок.
            if (to_del->right) {
                std::swap(to_del->key, to_del->right->key);
                std::swap(to_del->value, to_del->right->value);
                delLeaf(to_del->right);
                // Только левый ребёнок.
            } else {
                std::swap(to_del->key, to_del->left->key);
                std::swap(to_del->value, to_del->left->value);
                delLeaf(to_del->left);
            }
        }
        --size_;
    }

    size_t size() const {
        return size_;
    }
    bool empty() const {
        return size_ == 0;
    }

    Iterator lowerBound(const K& key) const {
        Node<K, V>* current = root;
        Node<K, V>* res = nullptr;
        res = lowerBound(key, current, res);
        return Iterator(res, root);
    }

    Node<K, V>* lowerBound(const K& key, Node<K, V>* current, Node<K, V>* res) const {
        if (current == nullptr) {
            return res;
        }
        if (current->key < key) {
            res = lowerBound(key, current->right, res);
        } else {
            res = lowerBound(key, current->left, current);
        }
        return res;
    }

    Iterator find(const K& key) const {
        return Iterator(findNode(key), root);
    }

    Iterator begin() const {
        return Iterator(Iterator::mostLeft(root), root);
    }
    Iterator end() const {
        return Iterator(nullptr, root);
    }

    Node<K, V>* root;

private:
    size_t size_;

    void deleteNode(Node<K, V>* node) {
        if (!node) {
            return;
        }
        deleteNode(node->left);
        deleteNode(node->right);
        delete node;
    }

    void insertNode(Node<K, V>* current, K key, V value) {
        if (!current || key == current->key) {
            current->value = value;
            return;
        }
        if (current->key < key) {
            if (current->right) {
                insertNode(current->right, key, value);
            } else {
                Node<K, V>* new_node = new Node<K, V>(key, value, current);
                current->right = new_node;
                balance(new_node);
                ++size_;
            }
        } else {
            if (current->left) {
                insertNode(current->left, key, value);
            } else {
                Node<K, V>* new_node = new Node<K, V>(key, value, current);
                current->left = new_node;
                balance(new_node);
                ++size_;
            }
        }
    }

    void rightRotation(Node<K, V>* pivot) {
        Node<K, V>* node = pivot->left;
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

    void leftRotation(Node<K, V>* pivot) {
        Node<K, V>* node = pivot->right;
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

    void balance(Node<K, V>* son) {
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

        Node<K, V>* pa = son->parent;
        Node<K, V>* grandpa = pa->parent;
        Node<K, V>* uncle = pa == grandpa->right ? grandpa->left : grandpa->right;

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

    void copyChildren(Node<K, V>* new_current, Node<K, V>* current) {
        if (!new_current) {
            return;
        }

        if (current->left) {
            new_current->left =
                new Node<K, V>(current->left->key, current->left->value, new_current);
            new_current->left->color = current->left->color;
            copyChildren(new_current->left, current->left);
        } else {
            new_current->left = nullptr;
        }

        if (current->right) {
            new_current->right =
                new Node<K, V>(current->right->key, current->right->value, new_current);
            new_current->right->color = current->right->color;
            copyChildren(new_current->right, current->right);
        } else {
            new_current->left = nullptr;
        }
    }

    void delLeaf(Node<K, V>* to_del) {
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
        delete to_del;
    }

    void fixDB(Node<K, V>* node) {
        if (node == root) {
            node->color = Color::BLACK;
            return;
        }

        Node<K, V>* pa = node->parent;
        Node<K, V>* sibling = pa->right == node ? pa->left : pa->right;
        Node<K, V>* l_child = sibling->left;
        Node<K, V>* r_child = sibling->right;
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
                } else if (l_child && l_child->color == Color::RED && r_child &&
                           r_child->color == Color::BLACK) {
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
                } else if (r_child && r_child->color == Color::RED && l_child &&
                           l_child->color == Color::BLACK) {
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

    Node<K, V>* findNode(const K& key) const {
        Node<K, V>* current = root;
        while (current && !(current->key == key)) {
            if (current->key < key) {
                current = current->right;
            } else {
                current = current->left;
            }
        }
        return current;
    }
};