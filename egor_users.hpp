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

void EgorTaskSolver::user_do_new_interval(int u) {
    //CNT_CALL_USER_NEW_INTERVAL++;

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
        return abs(len - users_info[u].rbNeed);
        /*if (len > users_info[u].rbNeed) {
            return (len - users_info[u].rbNeed);
        } else {
            return users_info[u].rbNeed - len;
        }*/
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

                if(users_info[u].rbNeed <= sum_len){
                    break;
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

void EgorTaskSolver::user_new_interval() {
    //CNT_CALL_USER_NEW_INTERVAL++;

    int u = rnd.get(0, N - 1);

    auto old_metric = metric;
    int old_actions_size = actions.size();

    user_do_new_interval(u);

    if (is_good(old_metric)) {
        //CNT_ACCEPTED_USER_NEW_INTERVAL++;
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }
}

void EgorTaskSolver::user_add_left() {
    //CNT_CALL_USER_ADD_LEFT++;

    USER_FOR_BEGIN(l > 0 &&
                   intervals[b][l - 1].users.size() < L &&
                   ((intervals[b][l - 1].beam_msk >> users_info[u].beam) & 1) == 0)

    l--;

    auto old_metric = metric;

    add_user_in_interval(u, b, l);

    if (is_good(old_metric)) {
        //CNT_ACCEPTED_USER_ADD_LEFT++;
    } else {
        rollback();
        ASSERT(old_metric == metric, "failed back score");
    }
    USER_FOR_END
}

void EgorTaskSolver::user_add_right() {
    //CNT_CALL_USER_ADD_RIGHT++;

    USER_FOR_BEGIN(r != -1 && r + 1 < intervals[b].size() &&
                   intervals[b][r + 1].users.size() < L &&
                   ((intervals[b][r + 1].beam_msk >> users_info[u].beam) & 1) == 0)

    r++;

    auto old_metric = metric;

    add_user_in_interval(u, b, r);

    if (is_good(old_metric)) {
        //CNT_ACCEPTED_USER_ADD_RIGHT++;
    } else {
        rollback();
        ASSERT(old_metric == metric, "failed back score");
    }
    USER_FOR_END
}

void EgorTaskSolver::user_remove_left() {
    //CNT_CALL_USER_REMOVE_LEFT++;

    USER_FOR_BEGIN(l != -1)

    auto old_metric = metric;

    remove_user_in_interval(u, b, l);

    if (is_good(old_metric)) {
        //CNT_ACCEPTED_USER_REMOVE_LEFT++;
    } else {
        rollback();
        ASSERT(old_metric == metric, "failed back score");
    }

    USER_FOR_END
}

void EgorTaskSolver::user_remove_right() {
    //CNT_CALL_USER_REMOVE_RIGHT++;

    USER_FOR_BEGIN(r != -1)

    auto old_metric = metric;

    remove_user_in_interval(u, b, r);

    if (is_good(old_metric)) {
        //CNT_ACCEPTED_USER_REMOVE_RIGHT++;
    } else {
        rollback();
        ASSERT(old_metric == metric, "failed back score");
    }
    USER_FOR_END
}

void EgorTaskSolver::user_do_swap_eq_beam(int u, int u2) {
    ASSERT(users_info[u].beam == users_info[u].beam, "no equals beams");

    metric.accepted -= min(users_info[u].rbNeed, users_info[u].sum_len);
    metric.accepted -= min(users_info[u2].rbNeed, users_info[u2].sum_len);

    metric.overflow -= max(0, users_info[u].sum_len - users_info[u].rbNeed);
    metric.overflow -= max(0, users_info[u2].sum_len - users_info[u2].rbNeed);

    swap(user_id_to_u[users_info[u].id], user_id_to_u[users_info[u2].id]);

    swap(users_info[u].rbNeed, users_info[u2].rbNeed);
    swap(users_info[u].id, users_info[u2].id);

    metric.accepted += min(users_info[u].rbNeed, users_info[u].sum_len);
    metric.accepted += min(users_info[u2].rbNeed, users_info[u2].sum_len);

    metric.overflow += max(0, users_info[u].sum_len - users_info[u].rbNeed);
    metric.overflow += max(0, users_info[u2].sum_len - users_info[u2].rbNeed);
}

void EgorTaskSolver::user_swap_eq_beam() {
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
            //CNT_CALL_USER_SWAP++;

            int u = ups[i].second;
            int u2 = ups[i + 1].second;

            auto old_metric = metric;

            user_do_swap_eq_beam(u, u2);

            if (is_good(old_metric)) {
                //CNT_ACCEPTED_USER_SWAP++;
            } else {
                user_do_swap_eq_beam(u, u2);
                ASSERT(old_metric == metric, "failed back score");
            }
        }
    }
}

void EgorTaskSolver::user_do_remove_and_add(int u, int u2) {
    auto remove_user = [&](int u) {
        auto [b, l, r] = get_user_position(u);
        if (b != -1) {
            for (int i = l; i <= r; i++) {
                remove_user_in_interval(u, b, i);
            }
        }
    };

    auto add_user_in = [&](int u, int b, int l, int r) {
        if(b == -1){
            return;
        }
        while (l <= r && ((intervals[b][l].beam_msk >> users_info[u].beam) & 1) == 1) {
            l++;
        }
        for (int i = l; i <= r  && ((intervals[b][i].beam_msk >> users_info[u].beam) & 1) == 0; i++) {
            add_user_in_interval(u, b, i);
        }
    };

    remove_user(u);
    remove_user(u2);
    user_do_new_interval(u);
    user_do_new_interval(u2);
}

// 980484 -> 981383
void EgorTaskSolver::user_remove_and_add() {
    for (int step = 0; step < 3; step++) {
        int u = rnd.get(0, N - 1);
        int u2 = rnd.get(0, N - 1);
        //CNT_CALL_USER_SWAP++;

        int old_actions_size = actions.size();
        auto old_metric = metric;

        user_do_remove_and_add(u, u2);

        if (is_good(old_metric)) {

        } else {
            rollback(old_actions_size);
            ASSERT(old_metric == metric, "failed back score");
        }
    }
}

void EgorTaskSolver::user_do_crop(int u) {
    ASSERT(false, "hello");
    //CNT_CALL_USER_DO_CROP++;

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

void EgorTaskSolver::user_crop() {
    ASSERT(false, "hello");
    //CNT_CALL_USER_CROP++;

    for (int i = 0; i < 20; i++) {
        user_do_crop(rnd.get(0, N - 1));
    }
    /*for (int u = 0; u < N; u++) {
        user_do_crop(u);
    }*/
}
