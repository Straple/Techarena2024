#pragma once

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
    for (int index = 0; index < 1; index++) {  \
        int u = rnd.get(0, N - 1);             \
        auto [b, l, r] = get_user_position(u); \
        if (condition) {

#define USER_FOR_END \
    }                \
    }

void EgorTaskSolver::user_do_new_interval(int u) {

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
        return len;
        //abs(len - users_info[u].rbNeed);
    };

    int best_b = -1, best_l = -1, best_r = -1, best_f = -1e9;
    for (int b = 0; b < B; b++) {
        for (int l = 0; l < intervals[b].size(); l++) {
            int sum_len = 0;
            for (int r = l; r < intervals[b].size(); r++) {
                sum_len += intervals[b][r].len;

                if (intervals[b][r].users.size() == L || ((intervals[b][r].beam_msk >> users_info[u].beam) & 1) == 1) {
                    break;
                }

                int cur_f = f(sum_len);

                if (best_f < cur_f) {
                    best_f = cur_f;
                    best_b = b;
                    best_l = l;
                    best_r = r;
                }

                /*auto &users = users_beam[users_info[u].beam];

                if (sum_len > users_info[users[0]].sum_len) {
                    break;
                }*/
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

void EgorTaskSolver::user_new_interval() {

    int u = rnd.get(0, N - 1);

    // TODO: только чуток херит score
    /*int u;
    {
        vector<pair<int, int>> ips;
        for(int user = 0; user < N; user++){
            ips.emplace_back(abs(users_info[user].sum_len - users_info[user].rbNeed),user);
        }
        if(ips.empty()){
            return;
        }
        sort(ips.begin(), ips.end(), greater<>());
        u = ips[rnd.get(0, min(10, (int)ips.size() - 1))].second;
    }*/

    auto old_metric = metric;
    int old_actions_size = actions.size();

    user_do_new_interval(u);

    if (is_good(old_metric)) {
        SNAP_ACTION("user_new_interval " + to_string(u) + " accepted");
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }
}


void EgorTaskSolver::user_remove_and_add() {
    for (int step = 0; step < 3; step++) {
        int u = rnd.get(0, N - 1);
        int u2 = rnd.get(0, N - 1);

        /*auto a = get_user_position(u);
        auto b = get_user_position(u2);

        if(get<0>(a) != get<0>(b)){
            continue;
        }*/

        int old_actions_size = actions.size();
        auto old_metric = metric;

        auto [old_b, old_l, old_r] = get_user_position(u);
        if (old_b != -1) {
            for (int i = old_l; i <= old_r; i++) {
                remove_user_in_interval(u, old_b, i);
            }
        }

        auto [old_b2, old_l2, old_r2] = get_user_position(u2);
        if (old_b2 != -1) {
            for (int i = old_l2; i <= old_r2; i++) {
                remove_user_in_interval(u2, old_b2, i);
            }
        }

        // add
        user_do_new_interval(u2);
        user_do_new_interval(u);

        //SNAP_ACTION("user_remove_and_add " + to_string(u) + " " + to_string(u2));

        if (is_good(old_metric)) {
            SNAP_ACTION("user_remove_and_add " + to_string(u) + " " + to_string(u2) + " accepted");
        } else {

            auto [new_b, new_l, new_r] = get_user_position(u);
            if (new_b != -1) {
                for (int i = new_l; i <= new_r; i++) {
                    remove_user_in_interval(u, new_b, i);
                }
            }

            auto [new_b2, new_l2, new_r2] = get_user_position(u2);
            if (new_b2 != -1) {
                for (int i = new_l2; i <= new_r2; i++) {
                    remove_user_in_interval(u2, new_b2, i);
                }
            }

            if (old_b != -1) {
                for (int i = old_l; i <= old_r; i++) {
                    add_user_in_interval(u, old_b, i);
                }
            }
            if (old_b2 != -1) {
                for (int i = old_l2; i <= old_r2; i++) {
                    add_user_in_interval(u2, old_b2, i);
                }
            }

            ASSERT(old_metric == metric, "failed back score");
        }
    }
}