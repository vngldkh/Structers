#include <iostream>

class Node {
    friend class Tree;
    int data_;
    Node* left_;
    Node* right_;

    size_t getHeight() {
        size_t l_h = 0, r_h = 0;
        if (left_ != nullptr) {
            l_h = left_->getHeight();
        }
        if (right_ != nullptr) {
            r_h = right_->getHeight();
        }
        return 1 + std::max(l_h, r_h);
    }

public:
    Node(int val) : data_(val), left_(nullptr), right_(nullptr){};
};

class Tree {
    Node* root_;

    void deleteBranch(Node* branch) {
        if (branch == nullptr) {
            return;
        }
        if (branch->right_ != nullptr) {
            deleteBranch(branch->right_);
        }
        if (branch->left_ != nullptr) {
            deleteBranch(branch->left_);
        }
        delete branch;
    }

public:
    Tree() : root_(nullptr){};
    ~Tree() {
        if (root_ == nullptr) {
            return;
        }
        deleteBranch(root_);
    }

    void insert(int val) {
        if (root_ == nullptr) {
            root_ = new Node(val);
            return;
        }
        auto temp = root_;
        bool to_continue = false;
        do {
            if (temp->data_ == val) {
                return;
            }
            if (val > temp->data_) {
                if (temp->right_ == nullptr) {
                    temp->right_ = new Node(val);
                    to_continue = false;
                } else {
                    temp = temp->right_;
                    to_continue = true;
                }
            } else if (val < temp->data_) {
                if (temp->left_ == nullptr) {
                    temp->left_ = new Node(val);
                    to_continue = false;
                } else {
                    temp = temp->left_;
                    to_continue = true;
                }
            }
        } while (to_continue);
    }

    size_t getHeight() {
        if (root_ == nullptr) {
            return 0;
        }
        return root_->getHeight();
    }
};

int main() {
    int x;
    std::cin >> x;
    Tree tree;
    while (x != 0) {
        tree.insert(x);
        std::cin >> x;
    }
    std::cout << tree.getHeight();
}
