#include <iostream>
#include <deque>

class Deque {
    std::deque<int64_t> head_ = {};
    std::deque<int64_t> tail_ = {};

    void balance() {
        auto size = head_.size() + tail_.size();
        if (tail_.size() < size / 2) {
            tail_.push_front(head_.back());
            head_.pop_back();
        } else if (tail_.size() > size / 2) {
            head_.push_back(tail_.front());
            tail_.pop_front();
        }
    }

public:
    size_t size() const {
        return head_.size() + tail_.size();
    }

    void pushBack(int64_t value) {
        tail_.push_back(value);
        balance();
    }

    void pushMiddle(int64_t value) {
        if (size() % 2 == 0) {
            head_.push_back(value);
        } else {
            tail_.push_front(value);
        }
    }

    int64_t popFront() {
        auto temp = head_.front();
        head_.pop_front();
        balance();
        return temp;
    }

    int front() const {
        return head_.front();
    }

    int back() const {
        if (!tail_.empty()) {
            return tail_.back();
        } else {
            return head_.back();
        }
    }
};

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
