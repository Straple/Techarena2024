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
        return len;//abs(len - users_info[u].rbNeed);
        /*if (len > users_info[u].rbNeed) {
            return (len - users_info[u].rbNeed);
        } else {
            return users_info[u].rbNeed - len;
        }*/
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

    //int u = rnd.get(0, N - 1);

    int u;
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
    }

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

void EgorTaskSolver::user_add_left() {

    USER_FOR_BEGIN(l > 0 &&
                   intervals[b][l - 1].users.size() < L &&
                   ((intervals[b][l - 1].beam_msk >> users_info[u].beam) & 1) == 0)

    l--;

    auto old_metric = metric;

    add_user_in_interval(u, b, l);

    //SNAP_ACTION("user_add_left " + to_string(u) + " " + to_string(b) + " " + to_string(l));

    if (is_good(old_metric)) {
        SNAP_ACTION("user_add_left " + to_string(u) + " " + to_string(b) + " " + to_string(l) + " accepted");
    } else {
        rollback();
        ASSERT(old_metric == metric, "failed back score");
    }
    USER_FOR_END
}

void EgorTaskSolver::user_add_right() {

    USER_FOR_BEGIN(r != -1 && r + 1 < intervals[b].size() &&
                   intervals[b][r + 1].users.size() < L &&
                   ((intervals[b][r + 1].beam_msk >> users_info[u].beam) & 1) == 0)

    r++;

    auto old_metric = metric;

    add_user_in_interval(u, b, r);

    //SNAP_ACTION("user_add_right " + to_string(u) + " " + to_string(b) + " " + to_string(r));

    if (is_good(old_metric)) {
        SNAP_ACTION("user_add_right " + to_string(u) + " " + to_string(b) + " " + to_string(r) + " accepted");
    } else {
        rollback();
        ASSERT(old_metric == metric, "failed back score");
    }
    USER_FOR_END
}

void EgorTaskSolver::user_remove_left() {

    USER_FOR_BEGIN(l != -1)

    auto old_metric = metric;

    remove_user_in_interval(u, b, l);
    //SNAP_ACTION("user_remove_left " + to_string(u) + " " + to_string(b) + " " + to_string(l));

    if (is_good(old_metric)) {
        SNAP_ACTION("user_remove_left " + to_string(u) + " " + to_string(b) + " " + to_string(l) + " accepted");
    } else {
        rollback();
        ASSERT(old_metric == metric, "failed back score");
    }

    USER_FOR_END
}

void EgorTaskSolver::user_remove_right() {

    USER_FOR_BEGIN(r != -1)

    auto old_metric = metric;

    remove_user_in_interval(u, b, r);
    //SNAP_ACTION("user_remove_right " + to_string(u) + " " + to_string(b) + " " + to_string(r));

    if (is_good(old_metric)) {
        SNAP_ACTION("user_remove_right " + to_string(u) + " " + to_string(b) + " " + to_string(r) + " accepted");
    } else {
        rollback();
        ASSERT(old_metric == metric, "failed back score");
    }
    USER_FOR_END
}

void EgorTaskSolver::user_do_swap_eq_beam(int u, int u2) {
    ASSERT(users_info[u].beam == users_info[u].beam, "no equals beams");

    auto &urbNeed = users_info[u].rbNeed;
    auto &u2rbNeed = users_info[u2].rbNeed;

    auto usum_len = users_info[u].sum_len;
    auto u2sum_len = users_info[u2].sum_len;

    metric.accepted += min(urbNeed, u2sum_len) - min(urbNeed, usum_len) +
                       min(u2rbNeed, usum_len) - min(u2rbNeed, u2sum_len);

    metric.overflow += max(0, usum_len - u2rbNeed) - max(0, usum_len - urbNeed) +
                       max(0, u2sum_len - urbNeed) - max(0, u2sum_len - u2rbNeed);

    swap(user_id_to_u[users_info[u].id], user_id_to_u[users_info[u2].id]);
    swap(urbNeed, u2rbNeed);
    swap(users_info[u].id, users_info[u2].id);
    swap(users_info[u].pos, users_info[u2].pos);
}

void EgorTaskSolver::user_remove_and_add() {
    for (int step = 0; step < 3; step++) {
        int u = rnd.get(0, N - 1);
        int u2 = rnd.get(0, N - 1);

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

void EgorTaskSolver::user_do_crop(int u) {
    ASSERT(false, "hello");

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

void EgorTaskSolver::beam_rebuild() {
    int beam;
    {
        vector<int> beams;
        for (int beam = 0; beam < 32; beam++) {
            if (!users_beam[beam].empty()) {
                beams.push_back(beam);
            }
        }
        if (beams.empty()) {
            return;
        }
        beam = beams[rnd.get(0, beams.size() - 1)];
    }

    int old_actions_size = actions.size();

    auto old_metric = metric;

    // (block, index)
    set<pair<int, int>> was_map;

    // удалим юзеров с этим beam
    for (int block = 0; block < B; block++) {
        for (int index = 0; index < intervals[block].size(); index++) {
            if ((intervals[block][index].beam_msk >> beam) & 1) {
                was_map.insert({block, index});
                int find_user = -1;
                for (int user: intervals[block][index].users) {
                    if (users_info[user].beam == beam) {
                        find_user = user;
                        break;
                    }
                }
                ASSERT(find_user != -1, "failed find");
                remove_user_in_interval(find_user, block, index);
            }
        }
    }

    for (int user: users_beam[beam]) {
        // поставим его жадно

        int best_block = -1, best_left = -1, best_right = -1, best_f = 1e9;

        for (int block = 0; block < B; block++) {
            for (int left = 0; left < intervals[block].size(); left++) {
                int len = 0;
                for (int right = left; right < intervals[block].size() && was_map.count({block, right}); right++) {

                    len += intervals[block][right].len;

                    int cur_f = abs(len - users_info[user].rbNeed);

                    if (cur_f < best_f) {
                        best_f = cur_f;
                        best_block = block;
                        best_left = left;
                        best_right = right;
                    }
                }
            }
        }

        if (best_block == -1) {
            continue;
        }

        for (int i = best_left; i <= best_right; i++) {
            add_user_in_interval(user, best_block, i);
            was_map.erase({best_block, i});
        }
    }

    if (is_good(old_metric)) {

    } else {
        rollback(old_actions_size);
        ASSERT(metric == old_metric, "failed metric");
    }
}