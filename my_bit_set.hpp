#pragma once

#include <climits>

template<std::size_t reserved_size>
class MyBitSet {
    using word_type = uint64_t;
    static constexpr std::size_t bits_cnt = sizeof(word_type) * 8;
    static constexpr std::size_t bits_size = (reserved_size + bits_cnt - 1) / bits_cnt;
    word_type bits[bits_size]{};

    static_assert(reserved_size % bits_cnt == 0, "invalid reserved_size");

    void flip(int x) {
        bits[x / bits_cnt] ^= (uint64_t(1) << (x % bits_cnt));
    }

public:
    void insert(int x) {
        ASSERT(0 <= x && x < reserved_size, "invalid x");
        ASSERT(!contains(x), "x already insert");

        flip(x);
    }

    void erase(int x) {
        ASSERT(0 <= x && x < reserved_size, "invalid x");
        ASSERT(contains(x), "x already erase");

        flip(x);
    }

    void clear() {
        for (int i = 0; i < bits_size; i++) {
            bits[i] = 0;
        }
    }

    [[nodiscard]] bool contains(int x) const {
        ASSERT(0 <= x && x < reserved_size, "invalid x");
        return (bits[x / bits_cnt] >> (x % bits_cnt)) & 1;
    }

    [[nodiscard]] bool empty() const {
        for (int i = 0; i < bits_size; i++) {
            if (bits[i] != 0) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] int size() const {
        int sz = 0;
        for (int i = 0; i < bits_size; i++) {
            sz += __builtin_popcountll(bits[i]);
        }
        return sz;
    }

    class Iterator {
        int x = 0;
        const MyBitSet &object;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using pointer = int;
        using reference = int;
        using iterator_category = std::forward_iterator_tag;

        Iterator(int X, const MyBitSet &OBJECT)
            : x(X), object(OBJECT) {
        }

        Iterator &operator++() {
            if (x == reserved_size) {
                return *this;
            }

            x++;

            while (x / bits_cnt < bits_size) {
                uint64_t val = object.bits[x / bits_cnt] >> (x % bits_cnt);
                if (val == 0) {
                    x += bits_cnt - x % bits_cnt;
                } else {
                    x += __builtin_ctzll(val);
                    break;
                }
            }

            return *this;
        }

        const int operator*() const {
            return x;
        }

        friend bool
        operator==(const Iterator &lhs, const Iterator &rhs) {
            return lhs.x == rhs.x && &lhs.object == &rhs.object;
        }

        friend bool
        operator!=(const Iterator &lhs, const Iterator &rhs) {
            return !(lhs == rhs);
        }
    };

    [[nodiscard]] Iterator begin() const {
        return ++Iterator(-1, *this);
    }

    [[nodiscard]] Iterator end() const {
        return Iterator(reserved_size, *this);
    }


    friend bool operator==(const MyBitSet &lhs, const MyBitSet &rhs) {
        for (int i = 0; i < bits_size; i++) {
            if (lhs.bits[i] != rhs.bits[i]) {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const MyBitSet &lhs, const MyBitSet &rhs) {
        return !(lhs == rhs);
    }

    friend MyBitSet operator|(MyBitSet lhs, const MyBitSet &rhs) {
        for (int i = 0; i < bits_size; i++) {
            lhs.bits[i] |= rhs.bits[i];
        }
        return lhs;
    }
    friend MyBitSet operator&(MyBitSet lhs, const MyBitSet &rhs) {
        for (int i = 0; i < bits_size; i++) {
            lhs.bits[i] &= rhs.bits[i];
        }
        return lhs;
    }
    friend MyBitSet operator^(MyBitSet lhs, const MyBitSet &rhs) {
        for (int i = 0; i < bits_size; i++) {
            lhs.bits[i] ^= rhs.bits[i];
        }
        return lhs;
    }
};