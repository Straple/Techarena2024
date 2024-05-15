#pragma once

///==========================
///===========INTERVAL=======
///==========================

#include <bitset>

#define CHOOSE_INTERVAL(condition, return_value)            \
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
            return return_value;                            \
        }                                                   \
        int k = rnd.get(0, ips.size() - 1);                 \
        b = ips[k].first;                                   \
        i = ips[k].second;                                  \
    }

void EgorTaskSolver::interval_flow_over() {

    //981995
    /*int b = rnd.get(0, B - 1);

    for (int step = 0; step < 100; step++) {

        int best_from = -1, best_to = -1, best_f = -1e9;
        for (int from = 0; from < intervals[b].size(); from++) {
            if (intervals[b][from].len == 0) {
                continue;
            }
            for (int to = 0; to < intervals[b].size(); to++) {
                if (from != to) {

                    auto and_users = intervals[b][from].users & intervals[b][to].users;
                    auto unique_i_users = intervals[b][from].users ^ and_users;
                    auto unique_j_users = intervals[b][to].users ^ and_users;

                    int accepted = 0;
                    int overflow = 0;

                    for (int u: unique_i_users) {
                        if (users_info[u].sum_len <= users_info[u].rbNeed) {
                            accepted--;
                        }
                        else{
                            overflow--;
                        }
                    }
                    for (int u: unique_j_users) {
                        if (users_info[u].sum_len < users_info[u].rbNeed) {
                            accepted++;
                        }
                        else{
                            overflow++;
                        }
                    }

                    int cur_f = accepted * 100 + overflow;

                    if (best_f <= cur_f) {
                        best_f = cur_f;
                        best_from = from;
                        best_to = to;
                    }
                }
            }
        }

        if (best_from == -1) {
            break;
        }

        change_interval_len(b, best_from, -1);
        change_interval_len(b, best_to, +1);
    }

    for (int i = (int) intervals[b].size() - 1; i >= 0; i--) {
        if (intervals[b][i].len == 0) {
            remove_interval(b, i);
        }
    }*/

    /*int b = rnd.get(0, B - 1);

    for (int step = 0; step < 100; step++) {
        // найти интервал, у которого можно хорошо спиздить длину 1
        // отдать ее другому интервалу

        int from = -1;
        {
            int best_f = -1e9;
            for (int i = 0; i < intervals[b].size(); i++) {
                if(intervals[b][i].len == 0){
                    continue;
                }
                int accepted = 0;
                for (int u: intervals[b][i].users) {
                    accepted -= users_info[u].sum_len <= users_info[u].rbNeed;
                }

                int cur_f = accepted;

                if (best_f <= cur_f) {
                    best_f = cur_f;
                    from = i;
                }
            }
            if(from == -1){
                break;
            }
        }

        int to = -1;
        {
            int best_f = -1e9;
            for (int i = 0; i < intervals[b].size(); i++) {
                int accepted = 0;
                for (int u: intervals[b][i].users) {
                    accepted += users_info[u].sum_len < users_info[u].rbNeed;
                }

                int cur_f = accepted;

                if (best_f < cur_f) {
                    best_f = cur_f;
                    to = i;
                }
            }
            if(to == -1){
                break;
            }
        }

        if (from == to) {
            break;
        }

        change_interval_len(b, from, -1);
        change_interval_len(b, to, +1);
    }

    for (int i = (int) intervals[b].size() - 1; i >= 0; i--) {
        if (intervals[b][i].len == 0) {
            remove_interval(b, i);
        }
    }*/

    /*sort(ips.begin(), ips.end());
        int sum = 0;
        for(auto [metric, block, i] : ips){
            sum += metric;
        }
        int x = rnd.get(0, sum);
        bool choose = false;
        for(auto [metric, bbbblock, iiii] : ips){
            x -= metric;
            if(x <= 0){
                choose = true;
                block = bbbblock;
                i = iiii;
            }
        }
        ASSERT(choose, "failed choose");*/

    // 982347
    int block, i;
    {
        // (metric, block, i)
        vector<tuple<int, int, int>> ips;
        for (int block = 0; block < B; block++) {
            if (intervals[block].size() > 1) {
                for (int i = 0; i < intervals[block].size(); i++) {
                    int overflow = 0;
                    for(int u : intervals[block][i].users){
                        overflow += max(0, users_info[u].sum_len - users_info[u].rbNeed);
                    }
                    ips.emplace_back(overflow, block, i);
                }
            }
        }
        if (ips.empty()) {
            return;
        }

        int p = rnd.get(0, ips.size() - 1);
        block = get<1>(ips[p]);
        i = get<2>(ips[p]);

        //int p = rnd.get(0, ips.size() - 1);
    }

    while (intervals[block][i].len > 0) {
        // выбрать кому лучше всего отдать длину +1
        int best_j = -1, best_f = 0;
        for (int j = 0; j < intervals[block].size(); j++) {
            if (j != i) {
                int accepted = 0;

                auto and_users = intervals[block][i].users & intervals[block][j].users;
                auto unique_i_users = intervals[block][i].users ^ and_users;
                auto unique_j_users = intervals[block][j].users ^ and_users;

                for (int u: unique_i_users) {
                    if (users_info[u].sum_len <= users_info[u].rbNeed) {
                        accepted--;
                    }
                }
                for (int u: unique_j_users) {
                    if (users_info[u].sum_len < users_info[u].rbNeed) {
                        accepted++;
                    }
                }

                int cur_f = accepted;

                if (best_f <= cur_f) {
                    best_f = cur_f;
                    best_j = j;
                }
            }
        }

        if (best_j == -1) {
            break;
        }

        change_interval_len(block, i, -1);
        change_interval_len(block, best_j, +1);
    }

    if (intervals[block][i].len == 0) {
        remove_interval(block, i);
    }

    /*int block, i, j, change;
    {
    // (f, block, i, j, change)
    vector<tuple<int, int, int, int,int>> ips;
    for (int block = 0; block < B; block++) {
        for (int i = 0; i < intervals[block].size(); i++) {
            for (int j = 0; j < intervals[block].size(); j++) {
                if (i != j) {

                    auto and_users = intervals[block][i].users & intervals[block][j].users;
                    auto unique_i = intervals[block][i].users ^ and_users;
                    auto unique_j = intervals[block][j].users ^ and_users;

                    for (int change = 1; change <= intervals[block][i].len; change++) {
                        int accepted = metric.accepted;
                        int overflow = metric.overflow;

                        for (int u: unique_i) {
                            accepted -= min(users_info[u].rbNeed, users_info[u].sum_len);
                            accepted += min(users_info[u].rbNeed, users_info[u].sum_len - change);
                        }

                        for (int u: unique_j) {
                            accepted -= min(users_info[u].rbNeed, users_info[u].sum_len);
                            accepted += min(users_info[u].rbNeed, users_info[u].sum_len + change);
                        }

                        int cur_f = accepted;

                        ips.emplace_back(cur_f, block, i, j, change);
                    }
                }
            }
        }
    }
    if (ips.empty()) {
        return;
    }
    sort(ips.begin(), ips.end(), greater<>());
    int p = rnd.get(0, min((int)ips.size() - 1, 3));
    block = get<1>(ips[p]);
    i = get<2>(ips[p]);
    j = get<3>(ips[p]);
    change = get<4>(ips[p]);
}*/

    // заберу у интервала i длину change, отдам ее интервалу j
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

    int best_change = rnd.get(-intervals[block][i].len, intervals[block][j].len);

    auto old_metric = metric;
    int old_actions_size = actions.size();

    change_interval_len(block, i, best_change);
    change_interval_len(block, j, -best_change);

    for(int unused_u : unused_users){
        for(int u : intervals[block][i].users){

            if(users_info[u].beam != users_info[unused_u].beam && ((intervals[block][i].beam_msk >> users_info[unused_u].beam) & 1) == 1){
                continue;
            }

            bool have_left = i > 0 && intervals[block][i-1].users.contains(u);
            bool have_right = i + 1 < intervals[block].size() && intervals[block][i+1].users.contains(u);

            if(have_left && have_right){
                continue;
            }

            if(min(users_info[u].rbNeed, users_info[u].sum_len) < min(users_info[unused_u].rbNeed, intervals[block][i].len)){
                remove_user_in_interval(u, block, i);
                add_user_in_interval(unused_u, block, i);
                break;
            }
        }


        if(!unused_users.contains(unused_u)){
            break;
        }

        for(int u : intervals[block][j].users){

            if(users_info[u].beam != users_info[unused_u].beam && ((intervals[block][j].beam_msk >> users_info[unused_u].beam) & 1) == 1){
                continue;
            }

            bool have_left = j > 0 && intervals[block][j-1].users.contains(u);
            bool have_right = j + 1 < intervals[block].size() && intervals[block][j+1].users.contains(u);

            if(have_left && have_right){
                continue;
            }

            if(min(users_info[u].rbNeed, users_info[u].sum_len) < min(users_info[unused_u].rbNeed, intervals[block][j].len)){
                remove_user_in_interval(u, block, j);
                add_user_in_interval(unused_u, block, j);
                break;
            }
        }
    }

    if (is_good(old_metric)) {
        SNAP_ACTION("interval_flow_over " + to_string(block) + " " + to_string(i) + " " + to_string(j) + " " +
                    to_string(change) + " accepted");

        if (i > j) {
            swap(i, j);
        }
        // i < j

        if (intervals[block][j].len == 0) {
            remove_interval(block, j);
        }
        if (intervals[block][i].len == 0) {
            remove_interval(block, i);
        }

#ifdef MY_DEBUG_MODE
        for (int block = 0; block < B; block++) {
            for (int index = 0; index < intervals[block].size(); index++) {
                if (intervals[block][index].len == 0) {
                    ASSERT(false, "zero interval");
                }
            }
        }
#endif
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }*/
}

void EgorTaskSolver::interval_do_merge(int b, int i) {
    ASSERT(false, "not used");

    ASSERT(i + 1 < intervals[b].size(), "invalid merge");

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
    std::vector<int> need_to_check;
    std::vector<int> plus_this(intervals[b][i].len, 0);
    std::vector<int> minus_this_after(intervals[b][i].len, 0);

    //    std::vector<int>need_to_check;
    bitset<512> have;
    int accumulated_lefts = 0;
    int accumulated_rights = 0;
    for (int u: right_end_users) {

        int right = users_info[u].sum_len - users_info[u].rbNeed;
        int left = intervals[b][i].len - right;
        if (left <= 0) {
            accumulated_rights++;
        }
        if (left >= 1 and left < intervals[b][i].len) {
            plus_this[left]++;
            if (!have[left]) {
                need_to_check.push_back(left);
                have[left] = true;
            }
        }
    }

    for (int u: left_end_users) {
        int left = users_info[u].sum_len - users_info[u].rbNeed;
        if (users_info[u].sum_len > users_info[u].rbNeed) {
            accumulated_lefts++;
        }
        if (left >= 1 and left < intervals[b][i].len) {
            minus_this_after[left]++;
            if (!have[left]) {
                need_to_check.push_back(left);
                have[left] = true;
            }
        }
    }
    if (!have[1]) {
        need_to_check.push_back(1);
        have[1] = true;
    }
    if (!have[intervals[b][i].len - 1]) {
        need_to_check.push_back(intervals[b][i].len - 1);
        have[intervals[b][i].len - 1] = true;
    }
    for (int u: end_users) {
        {
            int right = users_info[u].sum_len - users_info[u].rbNeed;
            int left = intervals[b][i].len - right;
            if (left >= 1 and left < intervals[b][i].len && !have[left]) {
                need_to_check.push_back(left);
                have[left] = true;
            }
        }
        {
            int left = users_info[u].sum_len - users_info[u].rbNeed;
            if (left >= 1 and left < intervals[b][i].len && !have[left]) {
                need_to_check.push_back(left);
                have[left] = true;
            }
        }
    }
    sort(need_to_check.begin(), need_to_check.end());

    if (false) {
        for (int left_len = 1; left_len < intervals[b][i].len; left_len++) {
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
    } else {
        for (int left_len: need_to_check) {
            int right_len = intervals[b][i].len - left_len;
            int free_space = 0;


            accumulated_rights += plus_this[left_len];
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
            int cur_f = free_space + accumulated_lefts * left_len + accumulated_rights * right_len;

            if (cur_f > best_f) {
                best_f = cur_f;
                best_right_len = right_len;
                best_left_len = left_len;
            }
            accumulated_lefts -= minus_this_after[left_len];
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

    // попытаемся добавить юзеров, которых нет в освободившиеся места

#ifdef MY_DEBUG_MODE
    MyBitSet set;
    for (int user = 0; user < N; user++) {
        if (users_info[user].sum_len == 0) {
            set.insert(user);
        }
    }
    ASSERT(unused_users == set, "invalid unused users");
#endif

    for (int u: unused_users) {
        bool may_left = intervals[b][i].users.size() < L && ((intervals[b][i].beam_msk >> users_info[u].beam) & 1) == 0;
        bool may_right = intervals[b][i + 1].users.size() < L && ((intervals[b][i + 1].beam_msk >> users_info[u].beam) & 1) == 0;

        if (may_left) {
            add_user_in_interval(u, b, i);
        }
        if (may_right) {
            add_user_in_interval(u, b, i + 1);
        }
        //user_do_new_interval(u);
    }
}

/*void EgorTaskSolver::interval_do_split(int b, int i) {
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
    for (int left_len = 1; left_len < intervals[b][i].len; left_len++) {
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

    // попытаемся добавить юзеров, которых нет в освободившиеся места

#ifdef MY_DEBUG_MODE
    MyBitSet set;
    for (int user = 0; user < N; user++) {
        if (users_info[user].sum_len == 0) {
            set.insert(user);
        }
    }
    ASSERT(unused_users == set, "invalid unused users");
#endif


    // add unused users
    for (int u: unused_users) {
        bool may_left = intervals[b][i].users.size() < L && ((intervals[b][i].beam_msk >> users_info[u].beam) & 1) == 0;
        bool may_right = intervals[b][i + 1].users.size() < L && ((intervals[b][i + 1].beam_msk >> users_info[u].beam) & 1) == 0;

        if (may_left) {
            add_user_in_interval(u, b, i);
        }
        if (may_right) {
            add_user_in_interval(u, b, i + 1);
        }
    }
}*/
// replace
/*for (int unused_u: unused_users) {
        for (int u: intervals[b][i].users) {
            bool have_left = (i == 0 || intervals[b][i - 1].users.contains(u));
            bool have_right = (i + 1 == intervals[b].size() || intervals[b][i + 1].users.contains(u));

if (have_left && have_right) {
    continue;
}

int old_accepted = min(users_info[u].sum_len, users_info[u].rbNeed);
int new_accepted = min(users_info[u].sum_len - intervals[b][i].len, users_info[u].rbNeed) +
                   min(users_info[unused_u].rbNeed, intervals[b][i].len);

if (old_accepted < new_accepted && ((intervals[b][i].beam_msk >> users_info[unused_u].beam) & 1) == 0) {
    remove_user_in_interval(u, b, i);
    add_user_in_interval(unused_u, b, i);
    break;
}
}

if (!unused_users.contains(unused_u)) {
    continue;
}

for (int u: intervals[b][i + 1].users) {
    bool have_left = intervals[b][i].users.contains(u);
    bool have_right = (i + 2 == intervals[b].size() || intervals[b][i + 2].users.contains(u));

    if (have_left && have_right) {
        continue;
    }

    int old_accepted = min(users_info[u].sum_len, users_info[u].rbNeed);
    int new_accepted = min(users_info[u].sum_len - intervals[b][i + 1].len, users_info[u].rbNeed) +
                       min(users_info[unused_u].rbNeed, intervals[b][i + 1].len);

    if (old_accepted < new_accepted && ((intervals[b][i + 1].beam_msk >> users_info[unused_u].beam) & 1) == 0) {
        remove_user_in_interval(u, b, i + 1);
        add_user_in_interval(unused_u, b, i + 1);
        break;
    }
}
}*/

/*right_end_users = intervals[b][i].users;
left_end_users = intervals[b][i].users;
if (i + 1 < intervals[b].size()) {
right_end_users = (right_end_users & intervals[b][i + 1].users) ^ right_end_users;
}
if (i > 0) {
left_end_users = (left_end_users & intervals[b][i - 1].users) ^ left_end_users;
}
end_users = left_end_users & right_end_users;
left_end_users = left_end_users ^ end_users;
right_end_users = right_end_users ^ end_users;

for (int u: right_end_users) {
for (int unused_u: unused_users) {
    int old_accepted = min(users_info[u].sum_len, users_info[u].rbNeed);
    int new_accepted = min(users_info[u].sum_len - intervals[b][i + 1].len, users_info[u].rbNeed) +
                       min(users_info[unused_u].rbNeed, intervals[b][i + 1].len);
    if (old_accepted < new_accepted) {
        remove_user_in_interval(u, b, i + 1);
        add_user_in_interval(unused_u, b, i + 1);
        break;
    }
}
}*/

/*for (int u = 0; u < N; u++) {
bool may_left = intervals[b][i].users.size() < L && ((intervals[b][i].beam_msk >> users_info[u].beam) & 1) == 0;
bool may_right = intervals[b][i + 1].users.size() < L && ((intervals[b][i + 1].beam_msk >> users_info[u].beam) & 1) == 0;

int len = (may_left ? intervals[b][i].len : 0) +
          (may_right ? intervals[b][i + 1].len : 0);

if(min(users_info[u].rbNeed, users_info[u].sum_len) <
min(users_info[u].rbNeed, len)){

for(int b = 0; b < B; b++){
for(int i = 0; i < intervals[b].size(); i++){
    if(intervals[b][i].users.contains(u)){
        remove_user_in_interval(u, b, i);
    }
}
}

if(may_left){
add_user_in_interval(u, b, i);
}
if(may_right){
add_user_in_interval(u, b, i + 1);
}
}
}*/

void EgorTaskSolver::interval_merge() {
    ASSERT(false, "not used");
    CHOOSE_INTERVAL(i + 1 < intervals[b].size(), );

    auto old_metric = metric;

    int old_actions_size = actions.size();

    interval_do_merge(b, i);

    if (is_good(old_metric)) {
        SNAP_ACTION("interval_merge " + to_string(b) + " " + to_string(i) + " accepted");
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }
}

bool EgorTaskSolver::interval_split_IMPL() {

    if (get_intervals_size() == J) {
        // нужно добыть интервал для split
        interval_do_free();
        if (get_intervals_size() == J) {
            return false;
        }
    }

    CHOOSE_INTERVAL(intervals[b][i].len > 1, false);

    interval_do_split(b, i);

    return true;
}

void EgorTaskSolver::interval_split() {
    ASSERT(get_intervals_size() <= J, "failed intervals size");

    int old_actions_size = actions.size();
    auto old_metric = metric;

    if (interval_split_IMPL() && is_good(old_metric)) {
        SNAP_ACTION("interval_split accepted");

#ifdef MY_DEBUG_MODE
        for (int block = 0; block < B; block++) {
            for (int index = 0; index < intervals[block].size(); index++) {
                if (intervals[block][index].len == 0) {
                    ASSERT(false, "zero interval");
                }
            }
        }
#endif
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }
}

void EgorTaskSolver::interval_do_free() {
    // попытаемся смержить два одинаковых интервала
    for (int block = 0; block < B; block++) {
        for (int index = 0; index + 1 < intervals[block].size(); index++) {
            if (intervals[block][index].users == intervals[block][index + 1].users) {
                change_interval_len(block, index, intervals[block][index + 1].len);
                remove_interval(block, index + 1);
                return;
            }
        }
    }

    // TODO: рандомно выбирать интервал почему-то дает больше баллов

    // (metric, block, index)
    vector<tuple<int, int, int>> ips;
    for (int block = 0; block < B; block++) {
        for (int index = 0; index < intervals[block].size(); index++) {
            // если мы удалим этот интервал, то сколько accepted будет?

            int accepted = 0;//metric.accepted;

            int overflow = 0;//metric.overflow;

            for (int user: intervals[block][index].users) {
                //accepted -= min(users_info[user].sum_len, users_info[user].rbNeed);
                accepted += min(users_info[user].sum_len - intervals[block][index].len, users_info[user].rbNeed);

                //overflow -= max(0, users_info[user].sum_len - users_info[user].rbNeed);
                overflow += max(0, users_info[user].sum_len - intervals[block][index].len - users_info[user].rbNeed);
            }

            ips.emplace_back(accepted, block, index);
        }
    }
    //sort(ips.begin(), ips.end(), greater<>());
    if (ips.empty()) {
        return;
    }

    auto [_, block, index] = ips[rnd.get(0, ips.size() - 1)];

    int len = intervals[block][index].len;

    ASSERT(len <= free_intervals[block].len(), "kek");
    //ASSERT(get_block_len(block) == free_intervals[block].len(), "kek");

    if (intervals[block].size() == 1) {
        //ASSERT(false, "don't touch him");
        return;
    }

    remove_interval(block, index);

    ASSERT(!intervals[block].empty(), "kek");

    // распределить наиболее оптимально len

    change_interval_len(block, rnd.get(0, (int) intervals[block].size() - 1), len);

    /*int best_index = -1, best_f = -1e9;
    for (int index = 0; index < intervals[block].size(); index++) {
        int accepted = 0;

        for (int u: intervals[block][index].users) {
            accepted += min(len, users_info[u].rbNeed - users_info[u].sum_len);
        }

        int cur_f = accepted;

        if (best_f < cur_f) {
            best_f = cur_f;
            best_index = index;
        }
    }

    ASSERT(best_index != -1, "invalid best_index");

    change_interval_len(block, best_index, len);*/

    // TODO: медленно, но хорошо дает скор
    /*for (; len > 0; len--) {

        int best_index = -1, best_f = -1e9;
        for (int index = 0; index < intervals[block].size(); index++) {
            int accepted = 0;

            for (int u: intervals[block][index].users) {
                if (users_info[u].sum_len < users_info[u].rbNeed) {
                    accepted++;
                }
            }

            int cur_f = accepted;

            if (best_f < cur_f) {
                best_f = cur_f;
                best_index = index;
            }
        }

        ASSERT(best_index != -1, "invalid best_index");

        change_interval_len(block, best_index, +1);
    }*/

    /*while(len > 0){

        // (accepted, min_add_len, index)
        vector<tuple<int, int, int>> ips;
        for (int index = 0; index < intervals[block].size(); index++) {
            int cnt = 0;
            int min_add_len = 10000;

            for (int u: intervals[block][index].users) {
                if (users_info[u].sum_len < users_info[u].rbNeed) {
                    min_add_len = min(min_add_len, users_info[u].rbNeed - users_info[u].sum_len);
                    cnt++;
                }
            }

            ips.emplace_back(cnt * min_add_len, min_add_len, index);
        }
        sort(ips.begin(), ips.end(), greater<>());

        ASSERT(!ips.empty(), "ips empty");

        auto [accepted, min_add_len, index] = ips[0];

        int x = min(min_add_len, len);
        ASSERT(x != 0, "invalid x");
        len -= x;
        change_interval_len(block, index, x);

        //ASSERT(best_index != -1, "invalid best_index");
        //change_interval_len(block, best_index, +1);
    }*/

    /*if (intervals[block].size() == 1) {
        //ASSERT(false, "don't touch him");
        return;
        remove_interval(block, 0);
    } else {
        remove_interval(block, index);
        change_interval_len(block, rnd.get(0, intervals[block].size() - 1), len);
    }*/

    /*if (index > 0 && index + 1 < intervals[block].size()) {

        int best_left_len = 0;

        ASSERT(best_left_len != -1, "failed");

        int right_len = len - best_left_len;

        remove_interval(block, index);
        change_interval_len(block, index - 1, best_left_len);
        change_interval_len(block, index, right_len);

    } else if (index > 0) {
        remove_interval(block, index);
        change_interval_len(block, index - 1, len);
    } else if (index + 1 < intervals[block].size()) {
        remove_interval(block, index);
        change_interval_len(block, index, len);
    } else {
        remove_interval(block, index);
    }*/
}
