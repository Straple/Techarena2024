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

    void rollback();

    void rollback(int size);

    EgorTaskSolver(int NN, int MM, int KK, int JJ, int LL,
                   const vector<Interval> &reservedRBs,
                   const vector<UserInfo> &userInfos,
                   vector<Interval> start_intervals, int random_seed);

    ///===========================
    ///===========ACTIONS=========
    ///===========================

    void IMPL_change_interval_len(int b, int i, int c);

    void change_interval_len(int b, int i, int c);

    void IMPL_add_user_in_interval(int u, int b, int i);

    void add_user_in_interval(int u, int b, int i);

    void IMPL_remove_user_in_interval(int u, int b, int i);

    void remove_user_in_interval(int u, int b, int i);

    void remove_interval(int b, int i);

    void insert_interval(int b, int i);

    ///==========================
    ///===========GETTERS========
    ///==========================

    [[nodiscard]] vector<Interval> get_total_answer() const;

    [[nodiscard]] tuple<int, int, int> get_user_position(int u) const;

    [[nodiscard]] int get_user_score(int u) const;

    [[nodiscard]] int get_block_len(int b) const;

    [[nodiscard]] int get_intervals_size() const;

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

    void interval_flow_over();

    void interval_increase_len();

    void interval_decrease_len();

    bool merge_verify(int b, int i);

    void interval_do_merge(int b, int i);

    void interval_do_split(int b, int i, int right_len);

    void interval_merge();

    void interval_split();

    ///======================
    ///===========USER=======
    ///======================

    void user_do_new_interval(int u);

    void user_new_interval();

    void user_do_swap(int u, int u2);

    void user_swap();

    void user_do_crop(int u);

    void user_crop();

    vector<Interval> annealing(vector<Interval> reservedRBs,
                               vector<UserInfo> userInfos);
};

#include "egor_getters.hpp"

#include "egor_actions.hpp"

#include "egor_intervals.hpp"

#include "egor_users.hpp"

#include "egor_impl.hpp"

vector<Interval> Solver_egor(int N, int M, int K, int J, int L,
                             const vector<Interval> &reservedRBs,
                             const vector<UserInfo> &userInfos, const std::vector<Interval> &solution, int random_seed);
