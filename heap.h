#include <vector>

template <class ValueType>
class Heap {
    std::vector<ValueType> mas_;

    void heapify(size_t ind) {
        bool flag = true;

        size_t current_ind = ind;

        while (flag) {
            size_t biggest_child = current_ind;
            size_t left_child = 2 * current_ind + 1;
            size_t right_child = 2 * current_ind + 2;

            if (left_child < mas_.size() && mas_[biggest_child] < mas_[left_child]) {
                biggest_child = left_child;
            }

            if (right_child < mas_.size() && mas_[biggest_child] < mas_[right_child]) {
                biggest_child = right_child;
            }

            flag = false;

            if (biggest_child != current_ind) {
                std::swap(mas_[biggest_child], mas_[current_ind]);
                current_ind = biggest_child;
                flag = true;
            }
        }
    }

public:
    Heap() {
        mas_ = std::vector<ValueType>();
    }

    template <class Iterator>
    Heap(Iterator begin, Iterator end) {
        mas_ = std::vector<ValueType>();
        for (Iterator it = begin; it != end; ++it) {
            mas_.push_back(*it);
        }
        for (size_t i = mas_.size() / 2 + 1; i > 0; --i) {
            heapify(i - 1);
        }
    }

    Heap(std::initializer_list<ValueType> list) {
        mas_ = std::vector<ValueType>(list);
        for (size_t i = mas_.size() / 2 + 1; i > 0; --i) {
            heapify(i - 1);
        }
    }

    Heap(const Heap& heap) {
        mas_ = heap.mas_;
    }

    Heap& operator=(const Heap& heap) {
        mas_ = heap.mas_;
        return *this;
    }

    Heap(Heap&& heap) {
        mas_ = std::move(heap.mas_);
    }

    Heap& operator=(Heap&& heap) {
        mas_ = std::move(heap.mas_);
        return *this;
    }

    ~Heap() = default;

    size_t size() const {
        return mas_.size();
    }

    bool empty() const {
        return mas_.empty();
    }

    void insert(const ValueType& val) {
        size_t ind = mas_.size();
        mas_.push_back(val);
        size_t parent_ind = (ind - 1) / 2;

        while (ind > 0 && mas_[parent_ind] < mas_[ind]) {
            std::swap(mas_[ind], mas_[parent_ind]);

            ind = parent_ind;
            parent_ind = (ind - 1) / 2;
        }
    }

    ValueType extract() {
        auto temp = mas_.front();
        mas_.front() = mas_.back();
        mas_.pop_back();
        heapify(0);
        return temp;
    }

    void print() {
        for (auto x : mas_) {
            std::cout << x << ' ';
        }
        std::cout << "; " << size() << '\n';
    }
};
