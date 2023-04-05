#include <iostream>
#include <cmath>
#include <deque>
#include <functional>

using std::cin;
using std::cout;
using std::deque;

class SqrtDecomp {
    deque<deque<int>> d_;
    deque<int64_t> comp_;
    size_t block_len_, block_count_, size_;
    int mod_;
    std::function<void(int64_t&, int64_t)> operation_;
    std::function<void(int64_t&, int64_t)> operation_rev_;
    bool flag_;

public:
    SqrtDecomp(size_t size, char op, int mod = 0) {
        block_len_ = static_cast<size_t>(sqrt(size));
        block_count_ = size / block_len_ + (size / block_len_ * block_len_ == size ? 0 : 1);
        d_ = deque<deque<int>>(block_count_, deque<int>(block_len_));
        comp_ = deque<int64_t>(block_count_, 0);
        if (op == '^') {
            flag_ = false;
            operation_ = [](int64_t& x, int y) { x ^= y; };
            operation_rev_ = [](int64_t& x, int y) { x ^= y; };
        } else {
            flag_ = true;
            operation_ = [mod](int64_t& x, int y) { x = (x + y % mod) % mod; };
            operation_rev_ = [mod](int64_t& x, int y) { x = (x - y % mod) % mod; };
        }
        this->mod_ = mod;
        this->size_ = size;
        for (size_t i = 0; i < size; ++i) {
            cin >> d_[i / block_len_][i % block_len_];
            operation_(comp_[i / block_len_], d_[i / block_len_][i % block_len_]);
        }
    }

    int at(size_t pos) const {
        return d_[(pos - 1) / block_len_][(pos - 1) % block_len_];
    }

    void insert(size_t pos, int value) {
        size_t block_no = (pos - 1) / block_len_;
        size_t block_pos = (pos - 1) % block_len_;
        ++size_;
        if (block_no == block_count_) {
            ++block_count_;
            d_.emplace_back(block_len_, 0);
            d_[block_no][0] = value;
            comp_.emplace_back(0);
            operation_(comp_[block_no], value);
            return;
        }

        d_[block_no].insert(d_[block_no].begin() + block_pos, value);
        operation_(comp_[block_no], value);

        for (size_t curr_block = block_no; curr_block < block_count_ - 1; ++curr_block) {
            operation_rev_(comp_[curr_block], d_[curr_block].back());
            operation_(comp_[curr_block + 1], d_[curr_block].back());
            d_[curr_block + 1].push_front(d_[curr_block].back());
            d_[curr_block].pop_back();
        }

        if ((size_ - 1) % block_len_ == 0) {
            d_.emplace_back(block_len_, 0);
            d_[block_count_].push_front(d_[block_count_ - 1].back());
            comp_.emplace_back(0);

            operation_(comp_[block_count_], d_[block_count_].front());
            operation_rev_(comp_[block_count_ - 1], d_[block_count_].front());

            ++block_count_;
            d_[block_count_ - 2].pop_back();
        }

        d_[block_count_ - 1].pop_back();
    }

    void erase(size_t pos) {
        size_t block_no = (pos - 1) / block_len_;
        size_t block_pos = (pos - 1) % block_len_;
        --size_;

        auto temp = d_[block_no][block_pos];
        d_[block_no].erase(d_[block_no].begin() + block_pos);

        operation_rev_(comp_[block_no], temp);

        for (size_t curr_block = block_no; curr_block < block_count_ - 1; ++curr_block) {
            d_[curr_block].push_back(d_[curr_block + 1].front());
            operation_(comp_[curr_block], d_[curr_block].back());
            operation_rev_(comp_[curr_block + 1], d_[curr_block].back());
            d_[curr_block + 1].pop_front();
        }

        if (size_ % block_len_ == 0) {
            d_.pop_back();
            comp_.pop_back();
            --block_count_;
        } else {
            d_.back().push_back(0);
        }
    }

    void change(size_t pos, int value) {
        size_t block_no = (pos - 1) / block_len_;
        size_t block_pos = (pos - 1) % block_len_;

        operation_rev_(comp_[block_no], d_[block_no][block_pos]);
        operation_(comp_[block_no], value);

        d_[block_no][block_pos] = value;
    }

    int64_t compute(size_t l, size_t r) const {
        if (size_ == 0) {
            return 0;
        }
        size_t l_block = (l - 1) / block_len_;
        size_t r_block = (r - 1) / block_len_;
        int64_t res = 0;

        for (size_t i = l_block; i <= r_block; ++i) {
            operation_(res, comp_[i]);
        }

        size_t l_pos = (l - 1) % block_len_;
        for (size_t i = 0; i < l_pos; ++i) {
            operation_rev_(res, d_[l_block][i]);
        }

        size_t r_pos = (r - 1) % block_len_;
        for (size_t i = r_pos + 1; i < block_len_; ++i) {
            operation_rev_(res, d_[r_block][i]);
        }

        if (flag_) {
            res %= mod_;
            if (res < 0) {
                res += mod_;
            }
        }
        return res;
    }

    size_t count() const {
        return size_;
    }
};

int main() {
    std::ios_base::sync_with_stdio(0);
    std::cin.tie(nullptr);
    char operation;
    cin >> operation;
    int mod;
    if (operation == '+') {
        cin >> mod;
    }

    size_t size;
    cin >> size;

    auto sq = SqrtDecomp(size, operation, mod);

    size_t operation_count;
    cin >> operation_count;
    std::string command;
    for (size_t i = 0; i < operation_count; ++i) {
        cin >> command;

        if (command == "insert") {
            size_t pos;
            cin >> pos;
            int value;
            cin >> value;
            sq.insert(pos, value);
        } else if (command == "erase") {
            size_t pos;
            cin >> pos;
            sq.erase(pos);
        } else if (command == "change") {
            size_t pos;
            cin >> pos;
            int value;
            cin >> value;
            sq.change(pos, value);
        } else if (command == "at") {
            size_t pos;
            cin >> pos;
            cout << sq.at(pos) << '\n';
        } else if (command == "compute") {
            size_t l, r;
            cin >> l >> r;
            auto res = sq.compute(l, r);
            cout << res << '\n';
        } else {
            cout << sq.count() << '\n';
        }
    }
}
