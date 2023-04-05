#include <iostream>

#include <iterator>
#include <initializer_list>

class Vector {
public:
    struct Iterator {
        using IteratorCategory = std::random_access_iterator_tag;
        using DifferenceType = std::ptrdiff_t;

        explicit Iterator(int* ptr) : m_ptr_(ptr){};

        int& operator*() const {
            return *m_ptr_;
        }
        int* operator->() {
            return m_ptr_;
        }

        Iterator& operator++() {
            m_ptr_++;
            return *this;
        }
        Iterator operator++(int) {
            auto temp = *this;
            m_ptr_++;
            return temp;
        }

        Iterator& operator--() {
            m_ptr_--;
            return *this;
        }
        Iterator operator--(int) {
            auto temp = *this;
            m_ptr_--;
            return temp;
        }

        Iterator operator+(const DifferenceType& movement) {
            return Iterator(this->m_ptr_ + movement);
        }
        Iterator operator-(const DifferenceType& movement) {
            return Iterator(this->m_ptr_ - movement);
        }

        Iterator& operator+=(const DifferenceType& movement) {
            m_ptr_ += movement;
            return *this;
        }
        Iterator& operator-=(const DifferenceType& movement) {
            m_ptr_ -= movement;
            return *this;
        }

        friend bool operator==(const Iterator& a, const Iterator& b) {
            return a.m_ptr_ == b.m_ptr_;
        }
        friend bool operator!=(const Iterator& a, const Iterator& b) {
            return a.m_ptr_ != b.m_ptr_;
        }

    private:
        int* m_ptr_;
    };

private:
    int* m_ptr_ = nullptr;
    size_t size_, capacity_;

public:
    Vector() {
        size_ = 0;
        capacity_ = 10;
        m_ptr_ = new int[capacity_];
    }

    explicit Vector(size_t n_size) {
        size_ = n_size;
        capacity_ = 2 * size_;
        m_ptr_ = new int[capacity_];
        for (size_t i = 0; i < size_; i++) {
            m_ptr_[i] = 0;
        }
    }

    Vector(const int* vals, size_t size) {
        size_ = size;
        capacity_ = 2 * size_;
        m_ptr_ = new int[capacity_];
        for (size_t i = 0; i < size_; i++) {
            m_ptr_[i] = vals[i];
        }
    }

    Vector(const Vector& vec) {
        size_ = vec.size_;
        capacity_ = vec.capacity_;
        m_ptr_ = new int[capacity_];
        for (size_t i = 0; i < size_; i++) {
            m_ptr_[i] = vec[i];
        }
    }

    Vector(std::initializer_list<int> vals) {
        size_ = vals.size();
        capacity_ = 2 * size_;
        m_ptr_ = new int[capacity_];
        size_t i = 0;
        for (auto x : vals) {
            m_ptr_[i++] = x;
        }
    }

    ~Vector() {
        delete[] m_ptr_;
    }

    size_t getSize() const {
        return size_;
    }

    size_t getCapacity() const {
        return capacity_;
    }

    bool isEmpty() const {
        return size_ == 0;
    }

    void resize(size_t n_size) {
        if (n_size >= capacity_) {
            int* temp = new int[2 * n_size];
            std::copy(m_ptr_, m_ptr_ + size_, temp);
            capacity_ = 2 * n_size;
            delete[] m_ptr_;
            m_ptr_ = temp;
        }
        if (n_size > size_) {
            for (size_t i = size_; i < n_size; i++) {
                m_ptr_[i] = 0;
            }
        }
        size_ = n_size;
    }

    void pushBack(int value) {
        resize(size_ + 1);
        m_ptr_[size_ - 1] = value;
    }

    void popBack() {
        if (isEmpty()) {
            throw std::runtime_error("Empty Array!");
        }
        size_--;
    }

    void clear() {
        size_ = 0;
    }

    void insert(size_t pos, int value) {
        if (pos > size_) {
            throw std::runtime_error("Wrong Position!");
        }
        resize(size_ + 1);
        for (size_t i = size_ - 1; i > pos; i--) {
            m_ptr_[i] = m_ptr_[i - 1];
        }
        m_ptr_[pos] = value;
    }

    void erase(size_t pos) {
        if (isEmpty()) {
            throw std::runtime_error("Empty Array!");
        }
        if (pos >= size_) {
            throw std::runtime_error("Wrong Position!");
        }
        for (size_t i = pos; i < size_ - 1; i++) {
            m_ptr_[i] = m_ptr_[i + 1];
        }
        size_--;
    }

    int at(size_t pos) {
        if (pos >= size_) {
            throw std::runtime_error("Wrong Position!");
        }
        return m_ptr_[pos];
    }

    int front() {
        if (isEmpty()) {
            throw std::runtime_error("Empty Array!");
        }
        return m_ptr_[0];
    }

    int back() {
        if (isEmpty()) {
            throw std::runtime_error("Empty Array!");
        }
        return m_ptr_[size_ - 1];
    }

    Iterator begin() {
        return Iterator(m_ptr_);
    }

    Iterator end() {
        return Iterator(m_ptr_ + size_);
    }

    int& operator[](size_t pos) const {
        if (pos >= size_) {
            throw std::runtime_error("Wrong Position!");
        }
        return m_ptr_[pos];
    }

    Vector& operator=(const Vector& other) {
        size_ = other.size_;
        capacity_ = other.capacity_;
        m_ptr_ = new int[capacity_];
        for (size_t i = 0; i < size_; i++) {
            m_ptr_[i] = other[i];
        }
    }
};

void mergeSort(Vector::Iterator begin, Vector::Iterator end) {
    size_t size = 0;
    for (auto it = begin; it != end; it++) {
        size++;
    }
    if (size <= 1) {
        return;
    }
    mergeSort(begin, begin + size / 2);
    mergeSort(begin + size / 2, end);
    Vector::Iterator curr = begin;
    Vector::Iterator m2 = begin + size / 2;
    size_t m1_inserted_count = 0;
    while (m1_inserted_count != size / 2 && m2 != end) {
        if (*curr > *m2) {
            int temp = *m2;
            for (auto it = m2; it != curr; it--) {
                *it = *(it - 1);
            }
            *curr = temp;
            m2++;
        } else {
            m1_inserted_count++;
        }
        curr++;
    }
}

void insertionSort(Vector::Iterator begin, Vector::Iterator end) {
    if (begin == end) {
        return;
    }
    Vector::Iterator unsorted = begin + 1;
    while (unsorted != end) {
        Vector::Iterator curr = begin;
        while (*unsorted > *curr && curr != unsorted) {
            curr++;
        }
        int temp = *unsorted;
        for (Vector::Iterator move = unsorted; move != curr; move--) {
            *move = *(move - 1);
        }
        *curr = temp;
        unsorted++;
    }
}

using std::cout;

void print(Vector& v) {
    for (auto x : v) {
        cout << x << ' ';
    }
    cout << "\nSize: " << v.getSize() << "; Capacity: " << v.getCapacity() << '\n';
}

int main() {
    Vector v = {1, 4, 8, 3, 9};
    print(v);
    v.pushBack(10);
    print(v);
    v.pushBack(10);v.pushBack(10);v.pushBack(10);
    v.pushBack(10);
    print(v);
    cout << v.front() << ' ' << v.back() << '\n';
    v.insert(0, 3);
    print(v);
    v.insert(1, 2);
    print(v);
    v.insert(12, 3);
    print(v);
    try {
        v.insert(14, 3);
    } catch (std::runtime_error e) {
        cout << e.what() << '\n';
    }
    v.erase(3);
    print(v);
    cout << v[3] << std::endl;
    v.popBack();
    print(v);
    v.resize(5);
    print(v);
    v.resize(10);
    print(v);
    mergeSort(v.begin(), v.begin() + 5);
    print(v);

    Vector w;
    cout << std::boolalpha << (w.begin() == w.end()) << std::endl;
    try {
        w.erase(1);
    } catch (std::runtime_error e) {
        cout << e.what() << '\n';
    }
    try {
        w[0];
    } catch (std::runtime_error e) {
        cout << e.what() << '\n';
    }
    print(w);

    Vector z(v);
    print(z);

    w = z;
    print(w);

    w.insert(6, 10);
    print(w);

    w.insert(11, 10);
    print(w);

    try {
        w.insert(13, 0);
        print(w);
    } catch (std::runtime_error e) {
        cout << e.what() << '\n';
    }

    w.insert(0, 0);
    print(w);

    w.pushBack(4);
    print(w);

    w.popBack();
    print(w);

    w.erase(4);
    print(w);

    cout << w[11] << w[0] << '\n';

    try {
        cout << w[13] << '\n';
    } catch (std::runtime_error e) {
        cout << e.what() << '\n';
    }

    try {
        cout << w.at(13) << '\n';
    } catch (std::runtime_error e) {
        cout << e.what() << '\n';
    }

    try {
        cout << w.at(0) << '\n';
    } catch (std::runtime_error e) {
        cout << e.what() << '\n';
    }

    Vector b(w);

    insertionSort(w.begin(), w.end());
    print(w);

    mergeSort(b.begin(), b.end());
    print(b);

    insertionSort(b.begin(), b.begin());
    print(b);

    insertionSort(b.begin(), b.begin() + 1);
    print(b);

    mergeSort(b.begin(), b.begin());
    print(b);
}
