#pragma once

#include "randomizer.hpp"
#include <vector>
#include <iostream>

struct SelectionRandomizer {
    randomizer rnd;
    static inline randomizer SELECTION_RANDOMIZER_LEARN_RANDOM;

    // (id, power)
    std::vector<std::pair<int, int>> kit;

    int power_sum() const {
        int sum = 0;
        for (auto [id, power]: kit) {
            sum += power;
        }
        return sum;
    }

public:
    SelectionRandomizer(int n) : kit(n) {
        ASSERT(n > 0, "invalid n");
        for (int i = 0; i < n; i++) {
            kit[i] = {i, 10};
        }
    }

    SelectionRandomizer(std::vector<std::pair<int, int>> init_kit) : kit(std::move(init_kit)) {
        ASSERT(kit.size() > 0, "invalid kit");
#ifdef MY_DEBUG_MODE
        std::vector<int> cnt_id(kit.size());
        for (auto [id, power]: kit) {
            ASSERT(0 <= id && id < kit.size(), "invalid id");
            ASSERT(power >= 0, "invalid power");
            cnt_id[id]++;
        }
        for (int i = 0; i < kit.size(); i++) {
            ASSERT(cnt_id[i] == 1, "have equal id");
        }
#endif
    }

    SelectionRandomizer(std::vector<int> powers) : kit(powers.size()) {
        ASSERT(kit.size() > 0, "invalid kit");
        for (int i = 0; i < powers.size(); i++) {
            kit[i] = {i, powers[i]};
            ASSERT(powers[i] >= 0, "invalid power");
        }
    }

    void reset_rnd() {
        rnd = randomizer();
    }

    int select() {
        double p = rnd.get_d() * power_sum();
        int sum = 0;
        for (auto [id, power]: kit) {
            sum += power;
            if (p <= sum) {
                return id;
            }
        }
        ASSERT(false, "select failed");
        return -1;
    }

    friend std::ostream &operator<<(std::ostream &output, SelectionRandomizer &selection) {
        output << "{ ";
        for (int i = 0; i < selection.kit.size(); i++) {
            output << selection.kit[i].second;
            if (i + 1 < selection.kit.size()) {
                output << ", ";
            }
        }
        output << "}";
        return output;
    }
};