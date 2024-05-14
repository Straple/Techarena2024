#pragma once

#include <climits>

class MyBitSet {
    uint64_t word[2] = {0, 0};

    void flip(uint8_t x) {
        word[x / 64] ^= (static_cast<uint64_t>(1) << (x % 64));
    }

public:
    void insert(uint8_t x) {
        ASSERT(0 <= x && x < 128, "invalid x");
        ASSERT(!contains(x), "x already insert");

        flip(x);
    }

    void erase(uint8_t x) {
        ASSERT(0 <= x && x < 128, "invalid x");
        ASSERT(contains(x), "x already erase");

        flip(x);
    }

    void clear() {
        word[0] = word[1] = 0;
    }

    [[nodiscard]] bool contains(uint8_t x) const {
        ASSERT(0 <= x && x < 128, "invalid x");
        return (word[x / 64] >> (x % 64)) & 1;
    }

    [[nodiscard]] bool empty() const {
        return word[0] != 0 || word[1] != 0;
    }

    [[nodiscard]] int size() const {
        return __builtin_popcountll(word[0]) + __builtin_popcountll(word[1]);
    }

    class Iterator {
        uint8_t bit;
        const MyBitSet &object;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using pointer = int;
        using reference = int;
        using iterator_category = std::forward_iterator_tag;

        Iterator(uint8_t BIT, const MyBitSet &OBJECT)
            : bit(BIT), object(OBJECT) {
        }

        Iterator &operator++() {
            if (bit == 128 || (++bit) == 128) {
                return *this;
            }

            uint64_t val = object.word[bit / 64] >> (bit % 64);
            if (val == 0) {
                if (bit < 64) {
                    bit = 64;
                    val = object.word[1];
                    if (val == 0) {
                        bit += 64 - bit % 64;
                    } else {
                        bit += __builtin_ctzll(val);
                    }
                } else {
                    bit = 128;
                }
            } else {
                bit += __builtin_ctzll(val);
            }
            return *this;
        }

        const int operator*() const {
            return bit;
        }

        friend bool
        operator==(const Iterator &lhs, const Iterator &rhs) {
            return lhs.bit == rhs.bit && &lhs.object == &rhs.object;
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
        return Iterator(128, *this);
    }

    friend bool operator==(const MyBitSet &lhs, const MyBitSet &rhs) {
        return lhs.word[0] == rhs.word[0] && lhs.word[1] == rhs.word[1];
    }

    friend bool operator!=(const MyBitSet &lhs, const MyBitSet &rhs) {
        return !(lhs == rhs);
    }

    friend MyBitSet operator|(MyBitSet lhs, const MyBitSet &rhs) {
        lhs.word[0] |= rhs.word[0];
        lhs.word[1] |= rhs.word[1];
        return lhs;
    }
    friend MyBitSet operator&(MyBitSet lhs, const MyBitSet &rhs) {
        lhs.word[0] &= rhs.word[0];
        lhs.word[1] &= rhs.word[1];
        return lhs;
    }
    friend MyBitSet operator^(MyBitSet lhs, const MyBitSet &rhs) {
        lhs.word[0] ^= rhs.word[0];
        lhs.word[1] ^= rhs.word[1];
        return lhs;
    }
};

/*class MyBitSet {
    static constexpr std::size_t reserved_size = 128;
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
};*/
