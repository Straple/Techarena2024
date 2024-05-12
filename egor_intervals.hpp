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

    // заберу у интервала i длину change, отдам ее интервалу j

    // было
    // 981863

    int block, i, j;
    {
        vector<tuple<int, int, int>> ips;
        for (int block = 0; block < B; block++) {
            for (int i = 0; i < intervals[block].size(); i++) {
                for (int j = 0; j < intervals[block].size(); j++) {
                    if (i != j) {
                        ips.emplace_back(block, i, j);
                    }
                }
            }
        }
        if (ips.empty()) {
            return;
        }
        int p = rnd.get(0, ips.size() - 1);
        block = get<0>(ips[p]);
        i = get<1>(ips[p]);
        j = get<2>(ips[p]);
    }

    int change = rnd.get(-intervals[block][i].len, intervals[block][j].len);

    auto old_metric = metric;
    int old_actions_size = actions.size();
    //CNT_CALL_FLOW_OVER++;

    change_interval_len(block, i, change);
    change_interval_len(block, j, -change);

    //SNAP_ACTION(
    //        "interval_flow_over " + to_string(b) + " " + to_string(i) + " " + to_string(j) + " " +
    //        to_string(change));

    if (is_good(old_metric)) {
        SNAP_ACTION("interval_flow_over " + to_string(block) + " " + to_string(i) + " " + to_string(j) + " " +
                    to_string(change) + " accepted");
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }

    /*int block, i, j;
    {
        vector<tuple<int, int, int, int>> ips;
        for (int block = 0; block < B; block++) {
            for (int i = 0; i < intervals[block].size(); i++) {
                for (int j = 0; j < intervals[block].size(); j++) {
                    if (i != j) {
                        //ips.emplace_back(block, i, j);

                        auto and_users = intervals[block][i].users & intervals[block][j].users;
                        auto unique_i = intervals[block][i].users ^ and_users;
                        auto unique_j = intervals[block][j].users ^ and_users;

                        int accepted = 0;
                        int overflow = 0;

                        for (int u: unique_i) {
                            accepted += min(users_info[u].rbNeed, users_info[u].sum_len);
                            overflow += max(0, users_info[u].sum_len - users_info[u].rbNeed);
                        }
                        for (int u: unique_j) {
                            accepted += min(users_info[u].rbNeed, users_info[u].sum_len);
                            overflow += max(0, users_info[u].sum_len - users_info[u].rbNeed);
                        }

                        int cur_f = overflow;

                        ips.emplace_back(cur_f, block, i, j);
                    }
                }
            }
        }
        if (ips.empty()) {
            return;
        }
        sort(ips.begin(), ips.end(), greater<>());
        int p = 0;//rnd.get(0, ips.size() - 1);
        block = get<1>(ips[p]);
        i = get<2>(ips[p]);
        j = get<3>(ips[p]);
    }

    int change = rnd.get(-intervals[block][i].len, intervals[block][j].len);

    auto old_metric = metric;
    int old_actions_size = actions.size();
    CNT_CALL_FLOW_OVER++;

    change_interval_len(block, i, change);
    change_interval_len(block, j, -change);

    SNAP_ACTION(
            "interval_flow_over " + to_string(b) + " " + to_string(i) + " " + to_string(j) + " " +
            to_string(change));

    if (is_good(old_metric)) {
        SNAP_ACTION("interval_flow_over " + to_string(b) + " " + to_string(i) + " " + to_string(j) + " " +
                    to_string(change) + " accepted");
        CNT_ACCEPTED_FLOW_OVER++;
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }*/

    //STEPS=5000
    //981497
    /*int block, i, j;
    {
        vector<tuple<int, int, int>> ips;
        for (int block = 0; block < B; block++) {
            for (int i = 0; i < intervals[block].size(); i++) {
                for (int j = 0; j < intervals[block].size(); j++) {
                    if (i != j) {
                        ips.emplace_back(block, i, j);
                    }
                }
            }
        }
        if (ips.empty()) {
            return;
        }
        int p = rnd.get(0, ips.size() - 1);
        block = get<0>(ips[p]);
        i = get<1>(ips[p]);
        j = get<2>(ips[p]);
    }

    auto and_users = intervals[block][i].users & intervals[block][j].users;
    auto unique_i = intervals[block][i].users ^ and_users;
    auto unique_j = intervals[block][j].users ^ and_users;

    int best_f = -1e9;
    int best_change = 0;
    for (int change = 1; change <= intervals[block][i].len; change++) {
        int accepted = 0;
        int overflow = 0;

        for (int u: unique_i) {
            accepted += min(users_info[u].rbNeed, users_info[u].sum_len - change);
            overflow += max(0, users_info[u].sum_len - change - users_info[u].rbNeed);
        }
        for (int u: unique_j) {
            accepted += min(users_info[u].rbNeed, users_info[u].sum_len + change);
            overflow += max(0, users_info[u].sum_len + change - users_info[u].rbNeed);
        }
        for (int u: and_users) {
            accepted += min(users_info[u].rbNeed, users_info[u].sum_len);
            overflow += max(0, users_info[u].sum_len - users_info[u].rbNeed);
        }

        int cur_f = accepted * 100 - overflow;

        if (best_f < cur_f) {
            best_f = cur_f;
            best_change = change;
        }
    }

    auto old_metric = metric;
    int old_actions_size = actions.size();
    CNT_CALL_FLOW_OVER++;

    change_interval_len(block, i, -best_change);
    change_interval_len(block, j, best_change);

    SNAP_ACTION(
            "interval_flow_over " + to_string(b) + " " + to_string(i) + " " + to_string(j) + " " +
            to_string(change));

    if (is_good(old_metric)) {
        SNAP_ACTION("interval_flow_over " + to_string(b) + " " + to_string(i) + " " + to_string(j) + " " +
                    to_string(change) + " accepted");
        CNT_ACCEPTED_FLOW_OVER++;
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }*/

    // bad
    // STEPS=5000
    // 980874
    /*for (int block = 0; block < B; block++) {
        int best_block = -1, best_i = -1, best_j = -1, best_change = 0, best_f = -1e9;

        for (int i = 0; i < intervals[block].size(); i++) {
            for (int j = 0; j < intervals[block].size(); j++) {
                if (i == j) {
                    continue;
                }

                auto and_users = intervals[block][i].users & intervals[block][j].users;
                auto unique_i = intervals[block][i].users ^ and_users;
                auto unique_j = intervals[block][j].users ^ and_users;

                for (int change = 1; change <= intervals[block][i].len; change++) {
                    int accepted = 0;
                    int overflow = 0;

                    for (int u: unique_i) {
                        accepted += min(users_info[u].rbNeed, users_info[u].sum_len - change);
                        overflow += max(0, users_info[u].sum_len - change - users_info[u].rbNeed);
                    }
                    for (int u: unique_j) {
                        accepted += min(users_info[u].rbNeed, users_info[u].sum_len + change);
                        overflow += max(0, users_info[u].sum_len + change - users_info[u].rbNeed);
                    }
                    for(int u : and_users){
                        accepted += min(users_info[u].rbNeed, users_info[u].sum_len);
                        overflow += max(0, users_info[u].sum_len - users_info[u].rbNeed);
                    }

                    int cur_f = accepted - overflow;

                    if (best_f < cur_f) {
                        best_f = cur_f;
                        best_block = block;
                        best_i = i;
                        best_j = j;
                        best_change = change;
                    }
                }
            }
        }

        if (best_block == -1) {
            continue;
        }

        auto old_metric = metric;
        int old_actions_size = actions.size();
        CNT_CALL_FLOW_OVER++;

        change_interval_len(best_block, best_i, -best_change);
        change_interval_len(best_block, best_j, best_change);

        SNAP_ACTION(
                "interval_flow_over " + to_string(b) + " " + to_string(i) + " " + to_string(j) + " " +
                to_string(change));

        if (is_good(old_metric)) {
            SNAP_ACTION("interval_flow_over " + to_string(b) + " " + to_string(i) + " " + to_string(j) + " " +
                        to_string(change) + " accepted");
            CNT_ACCEPTED_FLOW_OVER++;
        } else {
            rollback(old_actions_size);
            ASSERT(old_metric == metric, "failed back score");
        }
    }*/
}

void EgorTaskSolver::interval_increase_len() {
    //CNT_CALL_INTERVAL_INCREASE_LEN++;

    CHOOSE_INTERVAL(get_block_len(b) < free_intervals[b].len());

    int change = rnd.get(1, min(10, (free_intervals[b].len() - get_block_len(b))));

    auto old_metric = metric;

    change_interval_len(b, i, change);

    //SNAP_ACTION("interval_increase_len " + to_string(b) + " " + to_string(i) + " " + to_string(change));
    //if (is_good(old_metric, false)){
//
    //}
//    if (is_good(old_metric, false)){
//        cout << "INCREASE" << endl;
//    };

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

    //SNAP_ACTION("interval_decrease_len " + to_string(b) + " " + to_string(i) + " " + to_string(change));


    //if (is_good(old_metric, false)){
    //    cout << "DECREASE" << endl;
    //}

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

    //SNAP_ACTION("interval_merge " + to_string(b) + " " + to_string(i));

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

    //SNAP_ACTION("interval_split " + to_string(b) + " " + to_string(i) + " " + to_string(best_left_len));


    //if (is_good(old_metric, false)){
        //cout << "SPLIT" << endl;
    //}

    if (is_good(old_metric)) {
        SNAP_ACTION(
                "interval_split " + to_string(b) + " " + to_string(i) + " " + " accepted");
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