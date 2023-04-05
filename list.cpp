#include <iostream>

class Node {
public:
    int data{};
    Node* next;
    Node* previous;

    Node() : data(0), next(nullptr), previous(nullptr){};
};

class List {
    size_t size_;

public:
    List() : size_(0), head(nullptr), tail(nullptr){};

    List(int* values, size_t size) : size_(size), head(nullptr), tail(nullptr) {
        if (size == 0) {
            return;
        }
        if (abs(values[0]) > 2000000000) {
            throw std::runtime_error("Wrong Value!");
        }
        head = new Node();
        head->data = values[0];
        Node* temp = head;
        for (size_t i = 1; i < size; ++i, temp = temp->next) {
            if (abs(values[i]) > 2000000000) {
                Node* tmp;
                for (size_t j = 0; j < i; ++j) {
                    tmp = head->next;
                    delete head;
                    head = tmp;
                }
                throw std::runtime_error("Wrong Value!");
            }
            auto new_node = new Node();
            new_node->previous = temp;
            new_node->next = nullptr;
            new_node->data = values[i];
            temp->next = new_node;
        }
        tail = temp;
        tail->next = head;
        head->previous = tail;
    }

    ~List() {
        if (head == nullptr) {
            return;
        }
        head->previous = nullptr;
        tail->next = nullptr;
        Node* temp;
        while (head) {
            temp = head->next;
            delete head;
            head = temp;
        }
    }

    void pushBack(int value) {
        if (abs(value) > 2000000000) {
            throw std::runtime_error("Wrong Value!");
        }
        ++size_;
        if (size_ == 1) {
            head = new Node();
            head->data = value;
            head->next = head;
            head->previous = head;
            tail = head;
            return;
        }
        auto new_node = new Node();
        new_node->data = value;
        new_node->next = head;
        new_node->previous = tail;
        tail->next = new_node;
        head->previous = new_node;
        tail = new_node;
    }

    void pushFront(int value) {
        if (abs(value) > 2000000000) {
            throw std::runtime_error("Wrong Value!");
        }
        ++size_;
        if (size_ == 1) {
            head = new Node();
            head->data = value;
            head->next = head;
            head->previous = head;
            tail = head;
            return;
        }
        auto new_node = new Node();
        new_node->data = value;
        new_node->next = head;
        new_node->previous = tail;
        tail->next = new_node;
        head->previous = new_node;
        head = new_node;
    }

    int pop() {
        if (size_ == 0) {
            throw std::runtime_error("Can not pop such element!");
        }
        auto temp_data = head->data;
        --size_;
        if (size_ == 0) {
            delete head;
            head = nullptr;
            tail = nullptr;
            return temp_data;
        }
        tail->next = head->next;
        head->next->previous = tail;
        auto temp_node = head->next;
        delete head;
        head = temp_node;
        return temp_data;
    }

    int pop(size_t position) {
        if (position + 2 > size_ || size_ == 0) {
            throw std::runtime_error("Wrong Position!");
        }
        Node* node = head;
        --size_;
        for (size_t i = 0; i < position; ++i) {
            node = node->next;
        }
        auto old_node = node->next;
        auto temp_data = old_node->data;
        node->next = old_node->next;
        old_node->next->previous = node;
        delete old_node;
        if (position + 1 == size_) {
            tail = node;
        }
        return temp_data;
    }

    void push(int value, size_t position) {
        if (abs(value) > 2000000000) {
            throw std::runtime_error("Wrong Value!");
        }
        if (position + 1 > size_ || size_ == 0) {
            throw std::runtime_error("Wrong Position!");
        }
        Node* curr = head;
        for (size_t i = 0; i < position; ++i) {
            curr = curr->next;
        }
        auto new_node = new Node();
        new_node->data = value;
        new_node->previous = curr;
        new_node->next = curr->next;
        curr->next->previous = new_node;
        curr->next = new_node;
        if (position + 1 == size_) {
            tail = new_node;
        }
        ++size_;
    }

    Node* head{};
    Node* tail{};
};

void print(const List& l) {
    if (l.head == nullptr) {
        return;
    }
    for (Node* temp = l.head; temp != l.tail; temp = temp->next) {
        std::cout << temp->data << '\t';
    }
    std::cout << l.tail->data << '\n';
}

int main() {
    List l;
    print(l);
    l.pushFront(0);
    print(l);
    l.pushFront(9);
    print(l);
    std::cout << l.pop() << '\n';
    print(l);
    /*std::cout << l.pop(3) << '\n';
    print(l);
    l.push(7, 3);
    print(l);
    /*int* m = new int[0];
    List l(m, 0);
    print(l);
    std::cout << l.head << '\n';
    std::cout << l.head->data << '\n';
    std::cout << l.tail->data << '\n';
    std::cout << l.head->next->data << '\n';
    std::cout << l.head->previous->data << '\n';*/
    return 0;
}
