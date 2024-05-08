#pragma once

#include "assert.hpp"
#include "my_bit_set.hpp"
#include "randomizer.hpp"
#include "selection_randomizer.hpp"
#include "tools.hpp"

SelectionRandomizer SELECTION_ACTION = std::vector<int>{10, 1, 1, 1, 1, 16, 9, 6, 22, 12, 11, 30};
int STEPS = 300;

struct EgorTaskSolver {
    ///============================
    /// task data
    ///============================

    int N;
    int M;
    int K;
    int J;
    int L;

    int B;// колво блоков

    int prev_action = 0;
    int score_prev_action = 0;

    vector<MyInterval> free_intervals;

    ///============================
    /// internal data
    ///============================

    randomizer rnd;

    int total_score = 0;

    //int current_user = -1;

    struct SetInterval {
        int len = 0;
        MyBitSet<128> users;
        uint32_t beam_msk = 0;

        friend bool operator==(const SetInterval &lhs, const SetInterval &rhs) {
            return lhs.len == rhs.len &&
                   lhs.users == rhs.users &&
                   lhs.beam_msk == rhs.beam_msk;
        }
    };

    vector<vector<SetInterval>> intervals;

    struct MyUserInfo {

        // user info
        int id = -1;
        int rbNeed = -1;
        int beam = -1;

        // score
        int sum_len = 0;
    };

    vector<MyUserInfo> users_info;

    vector<int> users_with_equal_beam[32];

    vector<int> user_id_to_u;

    vector<int> users_order;

    struct Action {
        enum Type {
            ADD_USER_IN_INTERVAL,
            REMOVE_USER_IN_INTERVAL,
            CHANGE_INTERVAL_LEN,
            REMOVE_INTERVAL,
            ADD_INTERVAL,
        } type;

        int b = -1;
        int i = -1;
        int u = -1;
        int c = 0;
    };

    vector<Action> actions;

    void rollback() {
        ASSERT(!actions.empty(), "actions is empty");
        auto [type, b, i, u, c] = actions.back();
        actions.pop_back();

        if (type == Action::Type::ADD_USER_IN_INTERVAL) {
            IMPL_remove_user_in_interval(u, b, i);// O(1)
        } else if (type == Action::Type::REMOVE_USER_IN_INTERVAL) {
            IMPL_add_user_in_interval(u, b, i);// O(1)
        } else if (type == Action::Type::CHANGE_INTERVAL_LEN) {
            IMPL_change_interval_len(b, i, -c);// O(L)
        } else if (type == Action::Type::REMOVE_INTERVAL) {
            intervals[b].insert(intervals[b].begin() + i, SetInterval());// O(J)
        } else if (type == Action::Type::ADD_INTERVAL) {
            intervals[b].erase(intervals[b].begin() + i);// O(J)
        } else {
            ASSERT(false, "invalid type");
        }
    }

    void rollback(int size) {
        while (actions.size() > size) {
            rollback();
        }
    }

    EgorTaskSolver(int NN, int MM, int KK, int JJ, int LL,
                   const vector<Interval> &reservedRBs,
                   const vector<UserInfo> &userInfos,
                   vector<Interval> start_intervals, int random_seed) : N(NN), M(MM), K(KK), J(JJ), L(LL) {

        rnd.generator = mt19937_64(random_seed);

        ASSERT(2 <= L && L <= 16, "invalid L");
        ASSERT(0 < J && J <= 16, "invalid J");


        users_info.resize(N);
        user_id_to_u.resize(N);

        {
            /*for (int beam = 0; beam < 32; beam++) {
                users_with_equal_beam_size[beam] = 0;
            }*/
            for (int u = 0; u < N; u++) {
                ASSERT(u == userInfos[u].id, "are you stupid or something?");
                ASSERT(0 <= userInfos[u].beam && userInfos[u].beam < 32, "invalid beam");
                users_info[u].id = userInfos[u].id;
                users_info[u].rbNeed = userInfos[u].rbNeed;
                users_info[u].beam = userInfos[u].beam;

                //users_with_equal_beam_size[userInfos[u].beam]++;
                user_id_to_u[users_info[u].id] = u;
                users_with_equal_beam[userInfos[u].beam].push_back(userInfos[u].id);
            }
        }

        // build free_intervals
        {
            vector<bool> is_free(M + 1, true);
            is_free[M] = false;

            for (const auto &[start, end, users]: reservedRBs) {
                for (int i = start; i < end; i++) {
                    is_free[i] = false;
                }
            }

            int start = -1;
            for (int i = 0; i <= M; i++) {
                if (!is_free[i]) {
                    if (start != i - 1) {
                        free_intervals.push_back({start + 1, i});
                    }
                    start = i;
                }
            }
            B = free_intervals.size();
        }

        {
            users_order.resize(N);
            iota(users_order.begin(), users_order.end(), 0);
            sort(users_order.begin(), users_order.end(), [&](int lhs, int rhs) {
                return users_info[lhs].rbNeed > users_info[rhs].rbNeed;
            });
        }

        // build from start_intervals
        {
            sort(start_intervals.begin(), start_intervals.end(), [&](const auto &lhs, const auto &rhs) {
                return lhs.start < rhs.start;
            });

            intervals.resize(B);

            for (auto [start, end, users]: start_intervals) {
                bool already_push = false;
                for (int block = 0; block < B; block++) {
                    if (free_intervals[block].start <= start && end <= free_intervals[block].end) {
                        ASSERT(!already_push, "double push");
                        already_push = true;
                        intervals[block].push_back({end - start, {}, 0});
                        for (int u: users) {
                            add_user_in_interval(u, block, intervals[block].size() - 1);
                        }
                    }
                }
                ASSERT(already_push, "no push");
            }
            for (int i = start_intervals.size(); i < J; i++) {
                intervals[rnd.get(0, B - 1)].push_back(SetInterval());
            }
        }
    }

    ///===========================
    ///===========ACTIONS=========
    ///===========================

    void IMPL_change_interval_len(int b, int i, int c) {
        auto &interval = intervals[b][i];
        for (int u: interval.users) {
            total_score -= get_user_score(u);
            users_info[u].sum_len += c;
            total_score += get_user_score(u);
        }
        interval.len += c;
        ASSERT(0 <= interval.len, "invalid interval");
    }

    void change_interval_len(int b, int i, int c) {
        CNT_CALL_CHANGE_INTERVAL_LEN++;
        actions.push_back({Action::Type::CHANGE_INTERVAL_LEN, b, i, -1, c});
        IMPL_change_interval_len(b, i, c);
    }

    void IMPL_add_user_in_interval(int u, int b, int i) {
        auto &interval = intervals[b][i];
        auto &user = users_info[u];

        ASSERT(interval.users.size() + 1 <= L, "failed add");
        ASSERT(!interval.users.contains(u), "user already contains");
        ASSERT(((interval.beam_msk >> user.beam) & 1) == 0, "equal beams");

        total_score -= get_user_score(u);

        user.sum_len += interval.len;
        interval.users.insert(u);
        interval.beam_msk ^= (uint32_t(1) << user.beam);

        total_score += get_user_score(u);
    }

    void add_user_in_interval(int u, int b, int i) {
        CNT_CALL_ADD_USER_IN_INTERVAL++;
        actions.push_back({Action::Type::ADD_USER_IN_INTERVAL, b, i, u, 0});
        IMPL_add_user_in_interval(u, b, i);
    }

    void IMPL_remove_user_in_interval(int u, int b, int i) {
        auto &interval = intervals[b][i];
        auto &user = users_info[u];

        ASSERT(interval.users.contains(u), "user no contains");
        ASSERT(((interval.beam_msk >> users_info[u].beam) & 1) == 1, "user no contains");

        total_score -= get_user_score(u);

        user.sum_len -= interval.len;
        interval.users.erase(u);
        interval.beam_msk ^= (uint32_t(1) << user.beam);

        total_score += get_user_score(u);
    }

    void remove_user_in_interval(int u, int b, int i) {
        CNT_CALL_REMOVE_USER_IN_INTERVAL++;
        actions.push_back({Action::Type::REMOVE_USER_IN_INTERVAL, b, i, u, 0});
        IMPL_remove_user_in_interval(u, b, i);
    }

    void remove_interval(int b, int i) {
        if (intervals[b][i].len != 0) {
            change_interval_len(b, i, -intervals[b][i].len);
        }
        for (int u: intervals[b][i].users) {
            remove_user_in_interval(u, b, i);
        }

        actions.push_back({Action::Type::REMOVE_INTERVAL, b, i, -1, 0});
        intervals[b].erase(intervals[b].begin() + i);
    }

    void insert_interval(int b, int i) {
        actions.push_back({Action::Type::ADD_INTERVAL, b, i, -1, 0});
        intervals[b].insert(intervals[b].begin() + i, SetInterval());
    }

    ///==========================
    ///===========GETTERS========
    ///==========================

    [[nodiscard]] vector<Interval> get_total_answer() const {
        vector<Interval> answer;
        for (int block = 0; block < B; block++) {
            int start = free_intervals[block].start;
            for (auto [len, users, beam_msk]: intervals[block]) {
                if (len != 0 && !users.empty()) {
                    answer.push_back(Interval{start, start + len, {}});
                    start += len;
                    for (int u: users) {
                        answer.back().users.push_back(users_info[u].id);
                    }
                }
            }
            ASSERT(start <= free_intervals[block].end, "invalid segment");
        }
        return answer;
    }

    /*[[nodiscard]] tuple<int, int, int> get_user_position(int u) const {
        ASSERT(0 <= u && u < N, "invalid u");
        for (int block = 0; block < B; block++) {
            int left = -1;
            int right = -1;
            for (int i = 0; i < intervals[block].size(); i++) {
                if (intervals[block][i].users.contains(u)) {
                    right = i;
                    if (left == -1) {
                        left = i;
                    }
                }
            }
            if (left != -1) {
                return {block, left, right};
            }
        }
        return {-1, -1, -1};
    }*/

    [[nodiscard]] int get_user_score(int u) const {
        ASSERT(0 <= u && u < N, "invalid u");
        int len = users_info[u].sum_len;
        int rbNeed = users_info[u].rbNeed;
        return min(len, rbNeed);
    }

    [[nodiscard]] int get_block_len(int b) const {
        ASSERT(0 <= b && b < B, "invalid b");
        int len = 0;
        for (auto &interval: intervals[b]) {
            len += interval.len;
        }
        return len;
    }

    [[nodiscard]] int get_intervals_size() const {
        int size = 0;
        for (int b = 0; b < B; b++) {
            size += intervals[b].size();
        }
        return size;
    }

    ///==========================
    ///===========RANDOM=========
    ///==========================

    double temperature = 1;

    bool is_good(int old_score) {
        return total_score > old_score || rnd.get_d() < exp((total_score - old_score) / temperature);
    }

    ///==========================
    ///===========INTERVAL=======
    ///==========================

#define CHOOSE_INTERVAL(condition)                          \
    int b, i;                                               \
    {                                                       \
        vector<pair<int, int>> ips;                         \
        for (int b = 0; b < B; b++) {                       \
            for (int i = 0; i < intervals[b].size(); i++) { \
                if (condition) {                            \
                    ips.emplace_back(b, i);                 \
                }                                           \
            }                                               \
        }                                                   \
        if (ips.empty()) {                                  \
            return;                                         \
        }                                                   \
        int k = rnd.get(0, ips.size() - 1);                 \
        b = ips[k].first;                                   \
        i = ips[k].second;                                  \
    }

    void interval_flow_over() {
        CNT_CALL_INTERVAL_FLOW_OVER++;

        CHOOSE_INTERVAL(i + 1 < intervals[b].size());

        int change = rnd.get(-intervals[b][i].len, intervals[b][i + 1].len);

        int old_score = total_score;

        if (change > 0) {
            change_interval_len(b, i + 1, -change);
            change_interval_len(b, i, change);
        } else {
            change_interval_len(b, i, change);
            change_interval_len(b, i + 1, -change);
        }

        if (is_good(old_score)) {
            CNT_ACCEPTED_INTERVAL_FLOW_OVER++;
        } else {
            rollback();
            rollback();
            ASSERT(old_score == total_score, "failed back score");
        }
    }

    void interval_increase_len() {
        CNT_CALL_INTERVAL_INCREASE_LEN++;

        CHOOSE_INTERVAL(get_block_len(b) < free_intervals[b].len());

        int change = rnd.get(1, min(10, (free_intervals[b].len() - get_block_len(b))));

        int old_score = total_score;

        change_interval_len(b, i, change);

        if (is_good(old_score)) {
            CNT_ACCEPTED_INTERVAL_INCREASE_LEN++;
        } else {
            rollback();
            ASSERT(old_score == total_score, "failed back score");
        }
    }

    void interval_decrease_len() {
        CNT_CALL_INTERVAL_DECREASE_LEN++;

        CHOOSE_INTERVAL(intervals[b][i].len > 0);

        int change = rnd.get(-intervals[b][i].len, -1);

        int old_score = total_score;

        change_interval_len(b, i, change);

        if (is_good(old_score)) {
            CNT_ACCEPTED_INTERVAL_DECREASE_LEN++;
        } else {
            rollback();
            ASSERT(old_score == total_score, "failed back score");
        }
    }

    void interval_kek() {
        /*int old_score = total_score;
        //int old_actions_size = actions.size();

        auto old_intervals = intervals;
        auto old_users_info = users_info;

        if (get_intervals_size() == J) {
            vector<tuple<int, int, int>> ps;
            for (int b = 0; b < B; b++) {
                for (int i = 0; i < intervals[b].size(); i++) {
                    int old_actions_size = actions.size();
                    remove_interval(b, i);
                    ps.push_back({total_score, b, i});
                    rollback(old_actions_size);
                    ASSERT(old_score == total_score, "invalid back score");
                }
            }

            ASSERT(!ps.empty(), "empty ps");
            //sort(ps.begin(), ps.end(), greater<>());
            auto [to_score, b, i] = ps[rnd.get(0, (int) ps.size() - 1)];
            remove_interval(b, i);
            ASSERT(to_score == total_score, "invalid to score");
        }

        {
            vector<pair<int, int>> ps;
            for (int b = 0; b < B; b++) {
                for (int i = 0; i <= intervals[b].size(); i++) {
                    if (i == 0 || i == intervals[b].size() || (intervals[b][i - 1].users & intervals[b][i].users).empty()) {
                        ps.push_back({b, i});
                    }
                }
            }

            ASSERT(!ps.empty(), "empty ps");
            auto [b, i] = ps[rnd.get(0, ps.size() - 1)];
            insert_interval(b, i);
            int may_add_len = free_intervals[b].len() - get_block_len(b);
            if (may_add_len != 0) {
                change_interval_len(b, i, may_add_len);
            }
        }

        for (int b = 0; b < B; b++) {
            for (int i = 0; i < intervals[b].size(); i++) {
                intervals[b][i].users.clear();
                intervals[b][i].beam_msk = 0;
            }
        }
        for (int u = 0; u < N; u++) {
            users_info[u].sum_len = 0;
        }
        total_score = 0;

        {
            vector<int> p(N);
            iota(p.begin(), p.end(), 0);
            sort(p.begin(), p.end(), [&](int lhs, int rhs) {
                return users_info[lhs].rbNeed > users_info[rhs].rbNeed;
            });


            for (int u: p) {
                user_do_new_interval(u);
            }
        }*/

        // 980408
        /*for (int b = 0; b < B; b++) {
            for (int i = 0; i < intervals[b].size(); i++) {
                intervals[b][i].users.clear();
                intervals[b][i].beam_msk = 0;
            }
        }
        for (int u = 0; u < N; u++) {
            users_info[u].sum_len = 0;
        }
        total_score = 0;

        {
            CHOOSE_INTERVAL(merge_verify(b, i));
            interval_do_merge(b, i);
        }

        {
            CHOOSE_INTERVAL(true);
            int right_len = rnd.get(0, intervals[b][i].len);
            interval_do_split(b, i, right_len);
        }
        {
            vector<int> p(N);
            iota(p.begin(), p.end(), 0);
            sort(p.begin(), p.end(), [&](int lhs, int rhs) {
                return users_info[lhs].rbNeed > users_info[rhs].rbNeed;
            });


            for (int u: p) {
                user_do_new_interval(u);
            }
        }*/

        {
            /*set<pair<int, int>, greater<>> ps;
            {
                CHOOSE_INTERVAL(merge_verify(b, i));
                for (int u: intervals[b][i].users) {
                    ps.insert({users_info[u].rbNeed, u});
                }
                for (int u: intervals[b][i + 1].users) {
                    ps.insert({users_info[u].rbNeed, u});
                }
                interval_do_merge(b, i);
            }

            {
                CHOOSE_INTERVAL(true);
                int right_len = rnd.get(0, intervals[b][i].len);
                for (int u: intervals[b][i].users) {
                    ps.insert({users_info[u].rbNeed, u});
                }
                interval_do_split(b, i, right_len);
            }*/

            /*for (int b = 0; b < B; b++) {
                for (int i = 0; i < intervals[b].size(); i++) {
                    for (int u: intervals[b][i].users) {
                        remove_user_in_interval(u, b, i);
                    }
                }
            }

            vector<int> p(N);
            iota(p.begin(), p.end(), 0);
            sort(p.begin(), p.end(), [&](int lhs, int rhs) {
                return users_info[lhs].rbNeed > users_info[rhs].rbNeed;
            });

            for (int u: p) {
                user_do_new_interval(u);
            }*/
        }

        /*if (is_good(old_score)) {

        } else {
            //rollback(old_actions_size);
            intervals = old_intervals;
            users_info = old_users_info;
            total_score = old_score;
            //ASSERT(old_score == total_score, "failed back score");
        }*/
        // выбрать самый мало дающий интервал и удалить его
        /*int best_b = -1, best_i = -1, best_score = -1e9;
        for (int b = 0; b < B; b++) {
            for (int i = 0; i < intervals[b].size(); i++) {

                int old_actions_size = actions.size();

                remove_interval(b, i);

                if (best_score < total_score) {
                    best_score = total_score;
                    best_b = b;
                    best_i = i;
                }

                rollback(old_actions_size);
            }
        }

        if (best_b == -1) {
            return;
        }

        remove_interval(best_b, best_i);*/

        /*for (int b = 1; b < B; b++) {
            for (int b2 = 0; b2 < B; b2++) {
                for (int i = 0; i < intervals[b].size(); i++) {
                    int old_score = total_score;
                    int old_actions_size = actions.size();

                    remove_interval(b, i);

                    if (rnd.get_d() < 0.5) {
                        insert_interval(b2, 0);
                        change_interval_len(b2, 0, free_intervals[b2].len() - get_block_len(b2));
                        if (intervals[b2].size() > 1) {
                            for (int u: intervals[b2][1].users) {
                                if (users_info[u].sum_len < users_info[u].rbNeed) {
                                    add_user_in_interval(u, b2, 0);
                                }
                            }
                        }
                    } else {
                        insert_interval(b2, (int) intervals[b2].size());
                        change_interval_len(b2, intervals[b2].size() - 1, free_intervals[b2].len() - get_block_len(b2));
                        if (intervals[b2].size() > 1) {
                            for (int u: intervals[b2][intervals[b2].size() - 2].users) {
                                if (users_info[u].sum_len < users_info[u].rbNeed) {
                                    add_user_in_interval(u, b2, intervals[b2].size() - 1);
                                }
                            }
                        }
                    }

                    if (is_good(old_score)) {
                        CNT_ACCEPTED_INTERVAL_MERGE_EQUAL++;
                    } else {
                        rollback(old_actions_size);
                        ASSERT(old_score == total_score, "failed back score");
                    }
                }
            }
        }*/
        // очень мало таких ситуаций
        /*
        ASSERT(get_intervals_size() <= J, "failed intervals size");
        if (get_intervals_size() == J) {
            return;
        }

        int old_score = total_score;
        int old_actions_size = actions.size();

        vector<vector<int>> kek;
        for (int b = 0; b < B; b++) {
            for (int l = 0; l < intervals[b].size(); l++) {
                int len = 0;
                for (int r = l; r < intervals[b].size(); r++) {

                    len += intervals[b][r].len;

                    // можно ли вырезать этот отрезок интервалов
                    if (
                            (l == 0 || (intervals[b][l - 1].users & intervals[b][l].users).empty()) &&
                            (r == (int) intervals[b].size() - 1 || (intervals[b][r + 1].users & intervals[b][r].users).empty())) {

                        for (int b2 = 0; b2 < B; b2++) {
                            if (b != b2 && get_block_len(b2) + len <= free_intervals[b2].len()) {

                                kek.push_back({b, l, r, b2, 0});

                                for (int i = 1; i < intervals[b2].size(); i++) {
                                    if ((intervals[b2][i - 1].users & intervals[b2][i].users).empty()) {
                                        kek.push_back({b, l, r, b2, i});
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (kek.empty()) {
            return;
        }

        vector<int> state = kek[rnd.get(0, kek.size() - 1)];
        int b = state[0];
        int l = state[1];
        int r = state[2];
        int b2 = state[3];
        int i = state[4];
        cout << b << ' ' << l << ' ' << r << ' ' << b2 << ' ' << i << endl;

        if (b == 1) {
            cout << "here" << endl;
        }
        intervals[b2].insert(intervals[b2].begin() + i, intervals[b].begin() + l, intervals[b].begin() + r + 1);
        intervals[b].erase(intervals[b].begin() + l, intervals[b].begin() + r + 1);

        for (int k = l; k <= r; k++) {
            //intervals[b2].insert(intervals[b2].begin() + i, intervals[b][
            //intervals[b2].insert(intervals[b2].begin() + i + k + 1, intervals[b][k]);
        }
        for (int k = l; k <= r; k++) {
            //intervals[b].erase(intervals[b].begin() + l);
            //remove_interval(b, l);
        }

        for (int u = 0; u < N; u++) {
            auto [b, l, r] = get_user_position(u);
            for (int b2 = 0; b2 < B; b2++) {
                for (int i = 0; i < intervals[b2].size(); i++) {
                    if (intervals[b2][i].users.contains(u)) {
                        ASSERT(b2 == b, "invalid b");
                        ASSERT(l <= i && i <= r, "invalid i");
                    } else {
                        if (!(b != b2 || i < l || r < i)) {
                            cout << "failed: " << b2 << ' ' << i << endl;
                        }
                        ASSERT(b != b2 || i < l || r < i, "kek");
                    }
                }
            }
        }*/
    }

    /*void interval_move_left() {
        CHOOSE_INTERVAL(b > 0 && intervals[b].size() > 0 && get_block_len(b - 1) + intervals[b][0].len <= free_intervals[b - 1].len());

        insert_interval(b - 1, intervals[b - 1].size());
        change_interval_len(b - 1, intervals[b - 1].size() - 1, intervals[b][0].len);
        for (int u: intervals[b][0].users) {
            add_user_in_interval(u, b - 1, intervals[b - 1].size() - 1);
        }
        remove_interval(b, 0);
    }

    void interval_move_right() {
        CHOOSE_INTERVAL(b + 1 < B && intervals[b].size() > 0 && get_block_len(b + 1) + intervals[b].back().len <= free_intervals[b + 1].len());

        insert_interval(b + 1, 0);
        change_interval_len(b + 1, 0, intervals[b].back().len);
        for (int u: intervals[b].back().users) {
            add_user_in_interval(u, b + 1, 0);
        }
        remove_interval(b, intervals[b].size() - 1);
    }*/

    /*void interval_get_full_free_space(int interval) {
        CNT_CALL_INTERVAL_GET_FULL_FREE_SPACE++;
        int block = intervals[interval].block;
        if (block == -1) {
            return;
        }

        int len = 0;
        for (int i = 0; i < J; i++) {
            if (intervals[i].block == block) {
                len += intervals[i].len;
            }
        }

        int change = free_intervals[block].len() - len;
        ASSERT(change >= 0, "what?");
        if (change != 0) {
            int old_score = total_score;

            change_interval_len(interval, change);

            //if (is_good(old_score)) {
            //    CNT_ACCEPTED_INTERVAL_GET_FULL_FREE_SPACE++;
            //} else {
            //    change_interval_len(interval, -change);
            //    ASSERT(old_score == total_score, "failed back score");
            //}
        }
    }*/

    bool merge_verify(int b, int i) {
        return i + 1 < intervals[b].size();
    }

    void interval_do_merge(int b, int i) {
        CNT_CALL_INTERVAL_DO_MERGE_EQUAL++;

        ASSERT(merge_verify(b, i), "invalid merge");

        for (int u: intervals[b][i + 1].users) {
            if (intervals[b][i].users.size() + 1 <= L &&
                !intervals[b][i].users.contains(u) &&
                ((intervals[b][i].beam_msk >> users_info[u].beam) & 1) == 0) {
                add_user_in_interval(u, b, i);
            }
        }
        int right_len = intervals[b][i + 1].len;
        remove_interval(b, i + 1);
        change_interval_len(b, i, right_len);
    }

    void interval_do_split(int b, int i, int right_len) {
        CNT_CALL_INTERVAL_DO_SPLIT++;

        insert_interval(b, i + 1);
        change_interval_len(b, i, -right_len);
        change_interval_len(b, i + 1, right_len);
        for (int u: intervals[b][i].users) {
            add_user_in_interval(u, b, i + 1);
        }
    }

    void interval_merge() {
        CNT_CALL_INTERVAL_MERGE_EQUAL++;

        CHOOSE_INTERVAL(merge_verify(b, i));

        int old_score = total_score;
        int old_actions_size = actions.size();

        int right_len = intervals[b][i + 1].len;

        interval_do_merge(b, i);

        if (is_good(old_score)) {
            CNT_ACCEPTED_INTERVAL_MERGE_EQUAL++;
        } else {
            rollback(old_actions_size);
            ASSERT(old_score == total_score, "failed back score");
        }
    }

    void interval_split() {
        CNT_CALL_INTERVAL_SPLIT++;

        ASSERT(get_intervals_size() <= J, "failed intervals size");
        if (get_intervals_size() >= J) {
            return;
        }

        int old_score = total_score;
        int old_actions_size = actions.size();

        CHOOSE_INTERVAL(true);

        int right_len = rnd.get(0, intervals[b][i].len);

        interval_do_split(b, i, right_len);

        /*for (int b = 0; b < B; b++) {
            for (int i = 0; i < intervals[b].size(); i++) {
                for (int u: intervals[b][i].users) {
                    remove_user_in_interval(u, b, i);
                }
            }
        }

        vector<int> p(N);
        iota(p.begin(), p.end(), 0);
        sort(p.begin(), p.end(), [&](int lhs, int rhs) {
            return users_info[lhs].rbNeed > users_info[rhs].rbNeed;
        });
        for (int u: p) {
            user_do_new_interval(u);
        }*/


        if (is_good(old_score)) {
            CNT_ACCEPTED_INTERVAL_SPLIT++;
        } else {
            rollback(old_actions_size);
            ASSERT(old_score == total_score, "failed back score");
        }
    }

#define ACTION_WRAPPER(action_foo, action_id)     \
    int old_score = total_score;                  \
    cnt_edges[prev_action][action_id]++;          \
    action_foo();                                 \
    if (score_prev_action < total_score) {        \
        cnt_good_edges[prev_action][action_id]++; \
    }                                             \
    prev_action = action_id;                      \
    score_prev_action = old_score;

    ///======================
    ///===========USER=======
    ///======================

#define CHOOSE_USER(condition)                     \
    int u;                                         \
    {                                              \
        vector<int> ips;                           \
        for (int u = 0; u < N; u++) {              \
            auto [b, l, r] = get_user_position(u); \
            if (condition) {                       \
                ips.push_back(u);                  \
            }                                      \
        }                                          \
        if (ips.empty()) {                         \
            return;                                \
        }                                          \
        u = ips[rnd.get(0, ips.size() - 1)];       \
    }

#define USER_FOR_BEGIN(condition)              \
    for (int index = 0; index < 30; index++) { \
        int u = rnd.get(0, N - 1);             \
        auto [b, l, r] = get_user_position(u); \
        if (condition) {

    /*
#define USER_FOR_BEGIN(condition)                                   \
    shuffle(users_order.begin(), users_order.end(), rnd.generator); \
    for (int index = 0; index < N; index++) {                       \
        int u = users_order[index];                                 \
        auto [b, l, r] = get_user_position(u);                      \
        if (condition) {*/

#define USER_FOR_END \
    }                \
    }

    /*void user_add_interval() {
        int u = rnd.get(0, N - 1);
        int b = rnd.get(0, B - 1);
        if (intervals[b].empty()) {
            return;
        }
        int i = rnd.get(0, intervals[b].size() - 1);

        if (intervals[b][i].users.contains(u) || intervals[b][i].users.size() == L ||
            ((intervals[b][i].beam_msk >> users_info[u].beam) & 1) == 1) {
            return;
        }

        int old_score = total_score;
        add_user_in_interval(u, b, i);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_ADD_RIGHT++;
        } else {
            rollback();
            ASSERT(old_score == total_score, "failed back score");
        }
    }*/

    void user_do_new_interval(int u) {
        CNT_CALL_USER_NEW_INTERVAL++;

        //auto [old_b, old_l, old_r] = get_user_position(u);

        // remove user
        {
            for (int b = 0; b < B; b++) {
                for (int i = 0; i < intervals[b].size(); i++) {
                    if (intervals[b][i].users.contains(u)) {
                        remove_user_in_interval(u, b, i);
                    }
                }
            }
        }
        /*if (old_b != -1) {
            for (int i = old_l; i <= old_r; i++) {
                remove_user_in_interval(u, old_b, i);
            }
        }*/

        auto f = [&](int len) {
            if (len > users_info[u].rbNeed) {
                return (len - users_info[u].rbNeed);
            } else {
                return users_info[u].rbNeed - len;
            }
        };

        int best_b = -1, best_l = -1, best_r = -1, best_f = 1e9;
        for (int b = 0; b < B; b++) {
            for (int l = 0; l < intervals[b].size(); l++) {
                int sum_len = 0;
                for (int r = l; r < intervals[b].size(); r++) {
                    sum_len += intervals[b][r].len;

                    if (intervals[b][r].users.size() == L || ((intervals[b][r].beam_msk >> users_info[u].beam) & 1) == 1) {
                        break;
                    }

                    int cur_f = f(sum_len);

                    if (cur_f < best_f /*&& (b != old_b || r < old_l || old_r < l)*/) {
                        best_f = cur_f;
                        best_b = b;
                        best_l = l;
                        best_r = r;
                    }
                }
            }
        }

        if (best_b == -1) {
            return;
        }

        for (int i = best_l; i <= best_r; i++) {
            add_user_in_interval(u, best_b, i);
        }
    }

    void user_new_interval() {
        CNT_CALL_USER_NEW_INTERVAL++;

        int u = rnd.get(0, N - 1);
        //auto [old_b, old_l, old_r] = get_user_position(u);

        int old_score = total_score;
        int old_actions_size = actions.size();

        user_do_new_interval(u);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_NEW_INTERVAL++;
        } else {
            rollback(old_actions_size);
            ASSERT(old_score == total_score, "failed back score");
        }
    }

    /*void user_add_left() {
        CNT_CALL_USER_ADD_LEFT++;

        USER_FOR_BEGIN(l > 0 &&
                       intervals[b][l - 1].users.size() < L &&
                       ((intervals[b][l - 1].beam_msk >> users_info[u].beam) & 1) == 0)

        l--;
        int old_score = total_score;

        add_user_in_interval(u, b, l);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_ADD_LEFT++;
        } else {
            rollback();
            ASSERT(old_score == total_score, "failed back score");
        }
        USER_FOR_END
    }

    void user_add_right() {
        CNT_CALL_USER_ADD_RIGHT++;

        USER_FOR_BEGIN(r != -1 && r + 1 < intervals[b].size() &&
                       intervals[b][r + 1].users.size() < L &&
                       ((intervals[b][r + 1].beam_msk >> users_info[u].beam) & 1) == 0)

        r++;
        int old_score = total_score;

        add_user_in_interval(u, b, r);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_ADD_RIGHT++;
        } else {
            rollback();
            ASSERT(old_score == total_score, "failed back score");
        }
        USER_FOR_END
    }

    void user_remove_left() {
        CNT_CALL_USER_REMOVE_LEFT++;

        USER_FOR_BEGIN(l != -1)

        int old_score = total_score;

        remove_user_in_interval(u, b, l);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_REMOVE_LEFT++;
        } else {
            rollback();
            ASSERT(old_score == total_score, "failed back score");
        }

        USER_FOR_END
    }

    void user_remove_right() {
        CNT_CALL_USER_REMOVE_RIGHT++;

        USER_FOR_BEGIN(r != -1)

        int old_score = total_score;

        remove_user_in_interval(u, b, r);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_REMOVE_RIGHT++;
        } else {
            rollback();
            ASSERT(old_score == total_score, "failed back score");
        }
        USER_FOR_END
    }*/

    void user_do_swap(int u, int u2) {
        ASSERT(users_info[u].beam == users_info[u].beam, "no equals beams");

        total_score -= get_user_score(u) + get_user_score(u2);

        swap(user_id_to_u[users_info[u].id], user_id_to_u[users_info[u2].id]);

        swap(users_info[u].rbNeed, users_info[u2].rbNeed);
        swap(users_info[u].id, users_info[u2].id);

        total_score += get_user_score(u) + get_user_score(u2);
    }

    void user_swap() {
        for (int beam = 0; beam < 32; beam++) {
            // (sum_len, u)
            vector<pair<int, int>> ups;
            for (int u_id: users_with_equal_beam[beam]) {
                int u = user_id_to_u[u_id];
                ups.push_back({users_info[u].sum_len, u});
            }
            // без 981236
            shuffle(ups.begin(), ups.end(), rnd.generator);// 981748
            //sort(ups.begin(), ups.end()); // 981683
            for (int i = 0; i + 1 < ups.size(); i++) {
                CNT_CALL_USER_SWAP++;

                int u = ups[i].second;
                int u2 = ups[i + 1].second;

                int old_score = total_score;

                user_do_swap(u, u2);

                if (is_good(old_score)) {
                    CNT_ACCEPTED_USER_SWAP++;
                } else {
                    user_do_swap(u, u2);
                    ASSERT(old_score == total_score, "failed back score");
                }
            }
        }
    }

    void user_do_crop(int u) {
        CNT_CALL_USER_DO_CROP++;

        int best_b = -1, best_l = -1, best_r = -1, best_len = -1e9;
        for (int b = 0; b < B; b++) {
            for (int l = 0; l < intervals[b].size(); l++) {
                int len = 0;
                for (int r = l; r < intervals[b].size() && intervals[b][r].users.contains(u); r++) {
                    len += intervals[b][r].len;
                    if (best_len < len) {
                        best_len = len;
                        best_b = b;
                        best_l = l;
                        best_r = r;
                    }
                }
            }
        }

        for (int b = 0; b < B; b++) {
            for (int i = 0; i < intervals[b].size(); i++) {
                if (intervals[b][i].users.contains(u)) {
                    if (b == best_b) {
                        if (!(best_l <= i && i <= best_r)) {
                            remove_user_in_interval(u, b, i);
                        }
                    } else {
                        remove_user_in_interval(u, b, i);
                    }
                }
            }
        }
    }

    void user_crop() {
        CNT_CALL_USER_CROP++;

        for (int i = 0; i < 20; i++) {
            user_do_crop(rnd.get(0, N - 1));
        }
        /*for (int u = 0; u < N; u++) {
            user_do_crop(u);
        }*/
    }

    vector<Interval> annealing(vector<Interval> reservedRBs,
                               vector<UserInfo> userInfos) {
        temperature = 1;
        prev_action = 0;

        //int best_score = total_score;
        //auto best_ans = get_total_answer();

        for (int step = 0; step < STEPS; step++) {
            temperature = ((STEPS - step) * 1.0 / STEPS);
            //temperature *= 0.9999;

            //ASSERT(get_solution_score(N, M, K, J, L, reservedRBs, userInfos, get_total_answer()) == total_score, "invalid total_score");
            //if (THEORY_MAX_SCORE <= total_score) {
            //    break;
            //}

            int s = SELECTION_ACTION.select();
            if (s == 0) {
                ACTION_WRAPPER(user_new_interval, 0);
            } else if (s == 1) {
                //ACTION_WRAPPER(interval_move_left, 1);
                //ACTION_WRAPPER(user_add_left, 1);
            } else if (s == 2) {
                //ACTION_WRAPPER(interval_move_right, 2);
                //ACTION_WRAPPER(user_remove_left, 2);
            } else if (s == 3) {
                user_crop();
                //ACTION_WRAPPER(user_add_right, 3);
            } else if (s == 4) {
                //ACTION_WRAPPER(user_remove_right, 4);
            } else if (s == 5) {
                ACTION_WRAPPER(user_swap, 5);
            } else if (s == 6) {
                ACTION_WRAPPER(interval_increase_len, 6);
            } else if (s == 7) {
                ACTION_WRAPPER(interval_decrease_len, 7);
            } else if (s == 8) {
                ACTION_WRAPPER(interval_flow_over, 8);
            } else if (s == 9) {
                ACTION_WRAPPER(interval_merge, 9);
            } else if (s == 10) {
                ACTION_WRAPPER(interval_split, 10);
            } else if (s == 11) {
                ACTION_WRAPPER(interval_kek, 11);
            } else {
                ASSERT(false, "kek");
            }

            /*if (best_score < total_score) {
                best_score = total_score;
                best_ans = get_total_answer();
            }*/

            actions.clear();

            SNAP(snapshoter.write(get_total_answer(), "annealing"));
        }

        for (int u = 0; u < N; u++) {
            user_do_crop(u);
        }
        return get_total_answer();
        //return best_ans;
    }
};

vector<Interval> Solver_egor(int N, int M, int K, int J, int L,
                             const vector<Interval> &reservedRBs,
                             const vector<UserInfo> &userInfos, const std::vector<Interval> &solution, int random_seed) {
    EgorTaskSolver solver(N, M, K, J, L, reservedRBs, userInfos, solution, random_seed);
    auto answer = solver.annealing(reservedRBs, userInfos);
    ASSERT(solver.total_score == get_solution_score(N, M, K, J, L, reservedRBs, userInfos, answer), "invalid total_score");
    return answer;
}
