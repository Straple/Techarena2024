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
    CNT_CALL_INTERVAL_FLOW_OVER++;

    CHOOSE_INTERVAL(i + 1 < intervals[b].size());

    int change = rnd.get(-intervals[b][i].len, intervals[b][i + 1].len);

    auto old_metric = metric;

    if (change > 0) {
        change_interval_len(b, i + 1, -change);
        change_interval_len(b, i, change);
    } else {
        change_interval_len(b, i, change);
        change_interval_len(b, i + 1, -change);
    }

    if (is_good(old_metric)) {
        CNT_ACCEPTED_INTERVAL_FLOW_OVER++;
    } else {
        rollback();
        rollback();
        ASSERT(old_metric == metric, "failed back score");
    }
}

void EgorTaskSolver::interval_increase_len() {
    CNT_CALL_INTERVAL_INCREASE_LEN++;

    CHOOSE_INTERVAL(get_block_len(b) < free_intervals[b].len());

    int change = rnd.get(1, min(10, (free_intervals[b].len() - get_block_len(b))));

    auto old_metric = metric;

    change_interval_len(b, i, change);

    if (is_good(old_metric)) {
        CNT_ACCEPTED_INTERVAL_INCREASE_LEN++;
    } else {
        rollback();
        ASSERT(old_metric == metric, "failed back score");
    }
}

void EgorTaskSolver::interval_decrease_len() {
    CNT_CALL_INTERVAL_DECREASE_LEN++;

    CHOOSE_INTERVAL(intervals[b][i].len > 0);

    int change = rnd.get(-intervals[b][i].len, -1);

    auto old_metric = metric;

    change_interval_len(b, i, change);

    if (is_good(old_metric)) {
        CNT_ACCEPTED_INTERVAL_DECREASE_LEN++;
    } else {
        rollback();
        ASSERT(old_metric == metric, "failed back score");
    }
}

bool EgorTaskSolver::merge_verify(int b, int i) {
    return i + 1 < intervals[b].size();
}

void EgorTaskSolver::interval_do_merge(int b, int i) {
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

void EgorTaskSolver::interval_do_split(int b, int i, int right_len) {
    CNT_CALL_INTERVAL_DO_SPLIT++;

    insert_interval(b, i + 1);
    change_interval_len(b, i, -right_len);
    change_interval_len(b, i + 1, right_len);
    for (int u: intervals[b][i].users) {
        add_user_in_interval(u, b, i + 1);
    }
}

void EgorTaskSolver::interval_merge() {
    CNT_CALL_INTERVAL_MERGE_EQUAL++;

    CHOOSE_INTERVAL(merge_verify(b, i));

    auto old_metric = metric;

    int old_actions_size = actions.size();

    int right_len = intervals[b][i + 1].len;

    interval_do_merge(b, i);

    if (is_good(old_metric)) {
        CNT_ACCEPTED_INTERVAL_MERGE_EQUAL++;
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }
}

void EgorTaskSolver::interval_split() {
    CNT_CALL_INTERVAL_SPLIT++;

    ASSERT(get_intervals_size() <= J, "failed intervals size");
    if (get_intervals_size() >= J) {
        return;
    }

    auto old_metric = metric;

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


    if (is_good(old_metric)) {
        CNT_ACCEPTED_INTERVAL_SPLIT++;
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }
}
