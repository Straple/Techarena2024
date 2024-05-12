#pragma once

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

void EgorTaskSolver::interval_flow_over() {
    auto choose_intervals = [&]() {
        vector<tuple<int, int, int>> ps;
        for (int b = 0; b < B; b++) {
            if (intervals[b].size() >= 2) {
                for (int i = 0; i < intervals[b].size(); i++) {
                    for (int j = i + 1; j < intervals[b].size(); j++) {
                        ps.push_back({b, i, j});
                    }
                }
            }
        }
        if (ps.empty()) {
            return tuple{-1, -1, -1};
        }
        return ps[rnd.get(0, ps.size() - 1)];
    };

    auto [b, i, j] = choose_intervals();

    // TODO: доделать V3
    /*auto flow_over = [&](int change) {
        auto xor_users = intervals[b][i].users ^ intervals[b][i + 1].users;
        for (int u: xor_users) {
            if (intervals[b][i].users.contains(u)) {
                // left

                metric.accepted -= min(users_info[u].rbNeed, users_info[u].sum_len);
                metric.overflow -= max(0, users_info[u].sum_len - users_info[u].rbNeed);

                users_info[u].sum_len += change;

                metric.accepted += min(users_info[u].rbNeed, users_info[u].sum_len);
                metric.overflow += max(0, users_info[u].sum_len - users_info[u].rbNeed);
            } else {
                // right
                metric.accepted -= min(users_info[u].rbNeed, users_info[u].sum_len);
                metric.overflow -= max(0, users_info[u].sum_len - users_info[u].rbNeed);

                users_info[u].sum_len -= change;

                metric.accepted += min(users_info[u].rbNeed, users_info[u].sum_len);
                metric.overflow += max(0, users_info[u].sum_len - users_info[u].rbNeed);
            }
        }
        intervals[b][i].len += change;
        metric.free_space += change * (L - intervals[b][i].users.size());
        metric.unused_space -= change;

        intervals[b][i + 1].len -= change;
        metric.free_space -= change * (L - intervals[b][i + 1].users.size());
        metric.unused_space += change;
    };*/

    int change = rnd.get(-intervals[b][i].len, intervals[b][j].len);
    /*auto end_users = intervals[b][i].users ^ (intervals[b][i].users & intervals[b][i + 1].users);

    int overflow = 0;
    for (int u: end_users) {
        overflow = max(overflow, users_info[u].sum_len - users_info[u].rbNeed);
    }

    overflow = min(overflow, intervals[b][i].len);

    int change = rnd.get(-overflow, intervals[b][i + 1].len);*/

    /*int change = 0;

    auto and_users = intervals[b][i].users & intervals[b][i + 1].users;
    auto left_end_right = intervals[b][i].users ^ and_users;
    auto right_end_left = intervals[b][i + 1].users ^ and_users;

    auto right_end_right = and_users;
    if (i + 2 < intervals[b].size()) {
        right_end_right = right_end_right & (right_end_right ^ intervals[b][i + 2].users);
    }

    auto left_end_left = and_users;
    if (i > 0) {
        left_end_left = left_end_left & (left_end_left ^ intervals[b][i - 1].users);
    }

    {
        int best_f = -1e9;
        for (int c = -intervals[b][i].len; c <= intervals[b][i + 1].len; c++) {
            int left_len = intervals[b][i].len + c;
            int right_len = intervals[b][i + 1].len - c;

            int overflow = 0;
            int free_space = 0;

            for (int u: left_end_right) {
                overflow += max(0, users_info[u].sum_len + c - users_info[u].rbNeed);
            }
            for (int u: right_end_left) {
                overflow += max(0, users_info[u].sum_len - c - users_info[u].rbNeed);
            }

            for (int u: right_end_right) {
                if (users_info[u].rbNeed <= users_info[u].sum_len - right_len) {
                    free_space += right_len;
                }
            }
            for(int u : left_end_left){
                if (users_info[u].rbNeed <= users_info[u].sum_len - left_len) {
                    free_space += left_len;
                }
            }

            int cur_f = -free_space + overflow;

            if (cur_f > best_f) {
                best_f = cur_f;
                change = c;
            }
        }
    }*/

    auto old_metric = metric;
    int old_actions_size = actions.size();

    //#define V3

#define V1


#ifdef V3
    // те юзеры, что есть в i и в i+1 одновременно
    // никак не поменяются
    // таких довольно много

    flow_over(change);

#endif

#ifdef V2
    IMPL_change_interval_len(b, i + 1, -change);
    IMPL_change_interval_len(b, i, change);
#endif

#ifdef V1
    if (change > 0) {
        change_interval_len(b, j, -change);
        change_interval_len(b, i, change);
    } else {
        change_interval_len(b, i, change);
        change_interval_len(b, j, -change);
    }

    SNAP_ACTION(
            "interval_flow_over " + to_string(b) + " " + to_string(i) + " " + to_string(j) + " " + to_string(change));
    // было 980452

    /*for (int u: right_end_right) {
        if (users_info[u].rbNeed <= users_info[u].sum_len) {
            remove_user_in_interval(u, b, i+1);
        }
    }
    for(int u : left_end_left){
        if (users_info[u].rbNeed <= users_info[u].sum_len) {
            remove_user_in_interval(u, b, i);
        }
    }*/

#endif

    if (is_good(old_metric)) {
        SNAP_ACTION("interval_flow_over " + to_string(b) + " " + to_string(i) + " " + to_string(j) + " " +
                    to_string(change) + " accepted");
    } else {
#ifdef V3
        flow_over(-change);
#endif

#ifdef V2
        IMPL_change_interval_len(b, i + 1, change);
        IMPL_change_interval_len(b, i, -change);
#endif

#ifdef V1
        rollback(old_actions_size);
        //rollback();
        //rollback();
#endif
        ASSERT(old_metric == metric, "failed back score");
    }
}

void EgorTaskSolver::interval_increase_len() {
    //CNT_CALL_INTERVAL_INCREASE_LEN++;

    CHOOSE_INTERVAL(get_block_len(b) < free_intervals[b].len());

    int change = rnd.get(1, min(10, (free_intervals[b].len() - get_block_len(b))));

    auto old_metric = metric;

    change_interval_len(b, i, change);

    SNAP_ACTION("interval_increase_len " + to_string(b) + " " + to_string(i) + " " + to_string(change));

    if (is_good(old_metric)) {
        SNAP_ACTION(
                "interval_increase_len " + to_string(b) + " " + to_string(i) + " " + to_string(change) + " accepted");
    } else {
        rollback();
        ASSERT(old_metric == metric, "failed back score");
    }
}

void EgorTaskSolver::interval_decrease_len() {
    //CNT_CALL_INTERVAL_DECREASE_LEN++;

    CHOOSE_INTERVAL(intervals[b][i].len > 0);

    int change = rnd.get(-intervals[b][i].len, -1);

    auto old_metric = metric;

    change_interval_len(b, i, change);

    SNAP_ACTION("interval_decrease_len " + to_string(b) + " " + to_string(i) + " " + to_string(change));

    if (is_good(old_metric)) {
        SNAP_ACTION(
                "interval_decrease_len " + to_string(b) + " " + to_string(i) + " " + to_string(change) + " accepted");
    } else {
        rollback();
        ASSERT(old_metric == metric, "failed back score");
    }
}

/*bool EgorTaskSolver::merge_verify(int b, int i) {
    return i + 1 < intervals[b].size();
}*/

void EgorTaskSolver::interval_do_merge(int b, int i) {
    //CNT_CALL_INTERVAL_DO_MERGE_EQUAL++;

    ASSERT(i + 1 < intervals[b].size(), "invalid merge");

    // TODO: ничего не дало
    /*auto and_msk = intervals[b][i].users & intervals[b][i + 1].users;
    auto xor_msk = intervals[b][i].users ^ intervals[b][i + 1].users;

    int right_len = intervals[b][i + 1].len;
    remove_interval(b, i + 1);
    change_interval_len(b, i, right_len);
    for (int u: intervals[b][i].users) {
        if(!and_msk.contains(u)){
            remove_user_in_interval(u, b, i);
        }
    }

    vector<int> ps;
    for (int u: xor_msk) {
        ps.push_back(u);
    }
    sort(ps.begin(), ps.end(), [&](int lhs, int rhs) {
        return users_info[lhs].rbNeed -  users_info[lhs].sum_len  < users_info[rhs].rbNeed - users_info[rhs].sum_len;
    });

    for (int u: ps) {
        if (intervals[b][i].users.size() + 1 <= L &&
            ((intervals[b][i].beam_msk >> users_info[u].beam) & 1) == 0
            && users_info[u].sum_len < users_info[u].rbNeed
        ) {
            add_user_in_interval(u, b, i);
        }
    }*/

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

void EgorTaskSolver::interval_do_split(int b, int i) {
    auto right_end_users = intervals[b][i].users;
    auto left_end_users = intervals[b][i].users;
    if (i + 1 < intervals[b].size()) {
        right_end_users = (right_end_users & intervals[b][i + 1].users) ^ right_end_users;
    }
    if (i > 0) {
        left_end_users = (left_end_users & intervals[b][i - 1].users) ^ left_end_users;
    }
    auto end_users = left_end_users & right_end_users;
    left_end_users = left_end_users ^ end_users;
    right_end_users = right_end_users ^ end_users;

    int best_left_len = -1;
    int best_right_len = -1;
    int best_f = -1;
    for (int left_len = 0; left_len <= intervals[b][i].len; left_len++) {
        int right_len = intervals[b][i].len - left_len;

        int free_space = 0;

        for (int u: end_users) {
            int x = 0;
            if (users_info[u].sum_len - right_len >= users_info[u].rbNeed) {
                x = max(x, right_len);
            }
            if (users_info[u].sum_len - left_len >= users_info[u].rbNeed) {
                x = max(x, left_len);
            }
            free_space += x;
        }

        for (int u: right_end_users) {
            if (users_info[u].sum_len - right_len >= users_info[u].rbNeed) {
                free_space += right_len;
            }
        }
        for (int u: left_end_users) {
            if (users_info[u].sum_len - left_len >= users_info[u].rbNeed) {
                free_space += left_len;
            }
        }

        int cur_f = free_space;

        if (cur_f > best_f) {
            best_f = cur_f;
            best_right_len = right_len;
            best_left_len = left_len;
        }
    }
    ASSERT(best_right_len != -1, "failed");

    insert_interval(b, i + 1);
    change_interval_len(b, i, -best_right_len);
    change_interval_len(b, i + 1, best_right_len);
    for (int u: intervals[b][i].users) {
        add_user_in_interval(u, b, i + 1);
    }

    for (int u: right_end_users) {
        if (users_info[u].sum_len - best_right_len >= users_info[u].rbNeed) {
            remove_user_in_interval(u, b, i + 1);
        }
    }

    for (int u: left_end_users) {
        if (users_info[u].sum_len - best_left_len >= users_info[u].rbNeed) {
            remove_user_in_interval(u, b, i);
        }
    }
    for (int u: end_users) {
        int x = 0;
        if (users_info[u].sum_len - best_left_len >= users_info[u].rbNeed) {
            x = max(x, best_left_len);
        }
        if (users_info[u].sum_len - best_right_len >= users_info[u].rbNeed) {
            x = max(x, best_right_len);
        }

        if (x == best_left_len) {
            remove_user_in_interval(u, b, i);
        } else if (x == best_right_len) {
            remove_user_in_interval(u, b, i + 1);
        } else {
            //ASSERT(false, "kek");
        }
    }
}

void EgorTaskSolver::interval_merge() {
    CHOOSE_INTERVAL(i + 1 < intervals[b].size());

    auto old_metric = metric;

    int old_actions_size = actions.size();

    interval_do_merge(b, i);

    SNAP_ACTION("interval_merge " + to_string(b) + " " + to_string(i));

    if (is_good(old_metric)) {
        SNAP_ACTION("interval_merge " + to_string(b) + " " + to_string(i) + " accepted");
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }
}

void EgorTaskSolver::interval_split() {
    ASSERT(get_intervals_size() <= J, "failed intervals size");
    if (get_intervals_size() >= J) {
        return;
    }

    auto old_metric = metric;

    int old_actions_size = actions.size();

    CHOOSE_INTERVAL(true);

    //int right_len = rnd.get(0, intervals[b][i].len);
    // грамотно выберем длину right_len

    interval_do_split(b, i);

    //981959
    // end_users = юзеры, который справа заканчиваются в i
    /*auto end_users = intervals[b][i].users;
    if (i + 1 < intervals[b].size()) {
        end_users = (end_users & intervals[b][i + 1].users) ^ end_users;
    }

    int best_right_len = -1;
    int best_f = -1;
    for (int left_len = 0; left_len <= intervals[b][i].len; left_len++) {
        int right_len = intervals[b][i].len - left_len;

        int cur_f = 0;
        for (int u: end_users) {
            if (users_info[u].sum_len - right_len >= users_info[u].rbNeed) {
                // мы можем расплитить и убрать этого юзера из правого интервала
                cur_f += right_len;
            }
        }

        if (cur_f > best_f) {
            best_f = cur_f;
            best_right_len = right_len;
        }
    }
    ASSERT(best_right_len != -1, "failed");

    interval_do_split(b, i, best_right_len);

    for (int u: end_users) {
        if (users_info[u].sum_len - best_right_len >= users_info[u].rbNeed) {
            remove_user_in_interval(u, b, i + 1);
        }
    }*/

    // TODO: попробовать удалить тех, которые и так выполнены
    // они будут в intervals[b][i], intervals[b][i+1]
    // 980385 -> 980478
    /*for (int u: intervals[b][i].users) {
        auto [_, l, r] = get_user_position(u);
        if (l == i && users_info[u].sum_len - intervals[b][i].len >= users_info[u].rbNeed) {
            remove_user_in_interval(u, b, i);
        }
        // TODO: это как будто не работает
        else if (r == i && users_info[u].sum_len - intervals[b][i + 1].len >= users_info[u].rbNeed) {
            remove_user_in_interval(u, b, i + 1);
        }
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

    SNAP_ACTION("interval_split " + to_string(b) + " " + to_string(i) + " " + to_string(best_left_len));

    if (is_good(old_metric)) {
        SNAP_ACTION(
                "interval_split " + to_string(b) + " " + to_string(i) + " " + to_string(best_left_len) + " accepted");
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }
}

void EgorTaskSolver::interval_merge_and_split() {
    ASSERT(false, "not used");

    auto old_metric = metric;

    int old_actions_size = actions.size();

    {
        CHOOSE_INTERVAL(i + 1 < intervals[b].size());
        interval_do_merge(b, i);
    }

    {
        ASSERT(get_intervals_size() < J, "failed intervals size");

        CHOOSE_INTERVAL(true);

        interval_do_split(b, i);
    }

    if (is_good(old_metric)) {
        SNAP_ACTION("interval_merge_and_split accepted");
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }
}