#include <iostream>
#include <vector>

class Stack {
    std::vector<int64_t> v_ = {};

public:
    void push(int64_t x) {
        v_.push_back(x);
    }

    int64_t pop() {
        if (v_.empty()) {
            throw std::invalid_argument("error");
        }
        auto x = v_.back();
        v_.pop_back();
        return x;
    }

    int64_t back() {
        if (v_.empty()) {
            throw std::invalid_argument("error");
        }
        return v_.back();
    }

    size_t size() {
        return v_.size();
    }

    void clear() {
        v_.clear();
    }
};

int main() {
    bool exit;
    do {

    } while (!exit);
}
