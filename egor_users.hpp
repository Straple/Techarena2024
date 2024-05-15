#pragma once

///======================
///===========USER=======
///======================

/*#define CHOOSE_USER(condition)                     \
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
    }*/

void EgorTaskSolver::user_do_new_interval(int user) {
    {
        auto [old_b, old_l, old_r] = get_user_position(user);
        ASSERT(old_b == -1, "failed");
    }

    auto f = [&](int len) {
        return len;
        //abs(len - users_info[u].rbNeed);
    };

    int beam = users_info[user].beam;
    auto &users = users_beam[beam];

    int best_b = -1, best_l = -1, best_r = -1, best_f = -1e9;
    for (int b = 0; b < B; b++) {
        for (int l = 0; l < intervals[b].size(); l++) {
            int sum_len = 0;
            int pos = users.size() - 1;
            ASSERT(pos >= 0, "invalid pos");
            for (int r = l; r < intervals[b].size() &&
                            intervals[b][r].users.size() < L &&
                            ((intervals[b][r].beam_msk >> beam) & 1) == 0;
                 r++) {

                sum_len += intervals[b][r].len;

                /*int cur_f = min(users_info[0].rbNeed, sum_len);

                if (best_f < cur_f) {
                    best_f = cur_f;
                    best_b = b;
                    best_l = l;
                    best_r = r;
                }*/

                while (pos > 0 && users_info[users[pos]].rbNeed < sum_len) {
                    pos--;
                }

                int cur_f = min(users_info[users[pos]].rbNeed, sum_len);// - min(users_info[users[pos]].rbNeed, users_info[users[pos]].sum_len);// * 5 - max(0, sum_len - users_info[users[pos]].rbNeed);

                if (best_f < cur_f) {
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
        add_user_in_interval(user, best_b, i);
    }
}

/*void EgorTaskSolver::user_new_interval() {

    int u = rnd.get(0, N - 1);

    // TODO: только чуток херит score
    //int u;
    //{
    //    vector<pair<int, int>> ips;
    //    for(int user = 0; user < N; user++){
    //        ips.emplace_back(abs(users_info[user].sum_len - users_info[user].rbNeed),user);
    //    }
    //    if(ips.empty()){
    //        return;
    //    }
    //    sort(ips.begin(), ips.end(), greater<>());
    //    u = ips[rnd.get(0, min(10, (int)ips.size() - 1))].second;
    //}

    auto old_metric = metric;
    int old_actions_size = actions.size();

    user_do_new_interval(u);

    if (is_good(old_metric)) {
        SNAP_ACTION("user_new_interval " + to_string(u) + " accepted");
    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }
}*/

void EgorTaskSolver::user_remove_and_add() {
    /*for (int step = 0; step < 100; step++) {

        int u = rnd.get(0, N - 1);
        int u2 = rnd.get(0, N - 1);
        if (u == u2 || users_info[u].beam == users_info[u2].beam) {
            continue;
        }

        // swap u and u2

        int old_accepted = min(users_info[u].rbNeed, users_info[u].sum_len) +
                           min(users_info[u2].rbNeed, users_info[u2].sum_len);

        int new_accepted = min(users_info[u].rbNeed, users_info[u2].sum_len) +
                           min(users_info[u2].rbNeed, users_info[u].sum_len);

        if(new_accepted < old_accepted){
            continue;
        }

        int old_actions_size = actions.size();
        auto old_metric = metric;

        auto [old_b, old_l, old_r] = get_user_position(u);
        auto [old_b2, old_l2, old_r2] = get_user_position(u2);

        if (old_b != -1) {
            for (int i = old_l; i <= old_r; i++) {
                remove_user_in_interval(u, old_b, i);
            }
        }
        if (old_b2 != -1) {
            for (int i = old_l2; i <= old_r2; i++) {
                remove_user_in_interval(u2, old_b2, i);
            }
        }

        user_do_new_interval(u2);
        user_do_new_interval(u);

        if (is_good(old_metric)) {
            SNAP_ACTION("user_remove_and_add " + to_string(u) + " " + to_string(u2) + " accepted");
        } else {
            rollback(old_actions_size);
            ASSERT(metric == old_metric, "failed back metric");
        }
    }*/
    // 981412
    // return;

    /*for (int step = 0; step < 10; step++) {
        int u = rnd.get(0, N - 1);

        auto [old_b, old_l, old_r] = get_user_position(u);

        if (rnd.get_d() < temperature * 0.01) {
            if (old_b != -1) {
                for (int i = old_l; i <= old_r; i++) {
                    remove_user_in_interval(u, old_b, i);
                }
            }
        } else if (rnd.get_d() < 0.5) {
            // add left
            if (old_b != -1 && old_l > 0 &&
                intervals[old_b][old_l - 1].users.size() < L &&
                ((intervals[old_b][old_l - 1].beam_msk >> users_info[u].beam) & 1) == 0 &&
                users_info[u].sum_len < users_info[u].rbNeed) {

                add_user_in_interval(u, old_b, old_l - 1);
            }

            // add right
            else if (old_b != -1 && old_r + 1 < intervals[old_b].size() &&
                intervals[old_b][old_r + 1].users.size() < L &&
                ((intervals[old_b][old_r + 1].beam_msk >> users_info[u].beam) & 1) == 0 &&
                users_info[u].sum_len < users_info[u].rbNeed) {

                add_user_in_interval(u, old_b, old_r + 1);
            }

            // remove left
            else if (old_b != -1 &&
                users_info[u].sum_len - intervals[old_b][old_l].len >= users_info[u].rbNeed) {

                remove_user_in_interval(u, old_b, old_l);
            }

            // remove right
            else if (old_b != -1 &&
                users_info[u].sum_len - intervals[old_b][old_r].len >= users_info[u].rbNeed) {

                remove_user_in_interval(u, old_b, old_r);
            }
        }
        else{
            if (old_b != -1) {
                for (int i = old_l; i <= old_r; i++) {
                    remove_user_in_interval(u, old_b, i);
                }
            }

            user_do_new_interval(u);
        }
    }*/

    /*for (int u: unused_users) {
        user_do_new_interval(u);
    }*/

    // 981417
    /*for (int b = 0; b < B; b++) {
        for (int i = 0; i < intervals[b].size(); i++) {
            if (intervals[b][i].users.size() < L) {
                for (int u: unused_users) {
                    if (((intervals[b][i].beam_msk >> users_info[u].beam) & 1) == 0) {
                        add_user_in_interval(u, b, i);
                        break;
                    }
                }
            }
        }
    }*/

    /*for (int b = 0; b < B; b++) {
        for (int i = 0; i < intervals[b].size(); i++) {
            if (intervals[b][i].users.size() < L) {

            }
        }
    }*/

    //return;

    //user_RobinHood();

    //982788
    for (int step = 0; step < 3; step++) {
        int u = rnd.get(0, N - 1);
        int u2 = rnd.get(0, N - 1);

        if (u == u2 || users_info[u].beam == users_info[u2].beam) {
            continue;
        }

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

            rollback(old_actions_size);

            ASSERT(old_metric == metric, "failed back score");
        }
    }

    /*if(metric.accepted > old_metric.accepted) {
                auto [new_b, new_l, new_r] = get_user_position(u);
                auto [new_b2, new_l2, new_r2] = get_user_position(u2);

    cout << users_info[u].beam << ' ' << old_b << ' ' << old_l << ' ' << old_r << '\n';
    cout << users_info[u2].beam << ' ' << old_b2 << ' ' << old_l2 << ' ' << old_r2 << '\n';
    cout << users_info[u].beam << ' ' << new_b << ' ' << new_l << ' ' << new_r << '\n';
    cout << users_info[u2].beam << ' ' << new_b2 << ' ' << new_l2 << ' ' << new_r2 << '\n';
    cout << "============\n";
}*/
}

void EgorTaskSolver::user_RobinHood() {

    if (rnd.get_d() < 0.2) {
        vector<int> ips;
        for (int u: unused_users) {
            ips.push_back(u);
        }

        if (!ips.empty()) {
            int u = ips[rnd.get(0, ips.size() - 1)];
            user_do_new_interval(u);
        }
    }

    if (rnd.get_d() < 0.1) {
        int old_actions_size = actions.size();
        auto old_metric = metric;

        int u = rnd.get(0, N - 1);

        auto [b, l, r] = get_user_position(u);
        if(b != -1) {
            for (int i = l; i <= r; i++) {
                remove_user_in_interval(u, b, i);
            }
        }
        user_do_new_interval(u);

        if (is_good(old_metric)) {
        } else {
            rollback(old_actions_size);
            ASSERT(old_metric == metric, "failed back score");
        }
    }

    if (rnd.get_d() < 0.1) {
        // (block, index, user)
        vector<tuple<int, int, int>> ips;
        for (int block = 0; block < B; block++) {
            for (int index = 0; index + 1 < intervals[block].size(); index++) {

                auto unique_users = intervals[block][index + 1].users;
                unique_users = unique_users ^ (unique_users & intervals[block][index].users);

                for (int u: unique_users) {
                    if (users_info[u].sum_len < users_info[u].rbNeed &&
                        intervals[block][index].users.size() < L &&
                        ((intervals[block][index].beam_msk >> users_info[u].beam) & 1) == 0) {

                        ips.emplace_back(block, index, u);
                    }
                }
            }

            for (int index = 1; index < intervals[block].size(); index++) {
                auto unique_users = intervals[block][index - 1].users;
                unique_users = unique_users ^ (unique_users & intervals[block][index].users);
                for (int u: unique_users) {
                    if (users_info[u].sum_len < users_info[u].rbNeed &&
                        intervals[block][index].users.size() < L &&
                        ((intervals[block][index].beam_msk >> users_info[u].beam) & 1) == 0) {

                        ips.emplace_back(block, index, u);
                    }
                }
            }
        }

        if (!ips.empty()) {
            auto [b, i, u] = ips[rnd.get(0, ips.size() - 1)];

            add_user_in_interval(u, b, i);
        }
    }

    int block, index, u_left, u_right;
    block = -1;
    bool type;
    bool mode;
    {
        // (f, block, index, u_left, u_right)
        vector<tuple<int, int, int, int, int, bool>> ips;

        for (int block = 0; block < B; block++) {
            for (int index = 0; index < intervals[block].size(); index++) {
                auto unique_right_users = intervals[block][index].users;// Отдаём правый край.
                if (index + 1 < intervals[block].size()) {
                    auto and_users = intervals[block][index].users & intervals[block][index + 1].users;
                    unique_right_users = unique_right_users ^ and_users;
                }

                for (int u_right: unique_right_users) {
                    for (auto empty: unused_users) {
                        if (users_info[empty].beam == users_info[u_right].beam ||
                            ((intervals[block][index].beam_msk >> users_info[empty].beam) & 1) == 0) {
                            int accepted = min(users_info[u_right].rbNeed, users_info[u_right].sum_len - intervals[block][index].len) +
                                           min(users_info[empty].rbNeed, /*users_info[empty].sum_len*/ +intervals[block][index].len);
                            ips.emplace_back(accepted, block, index, u_right, empty, true);
                        }
                    }
                }
            }
        }

        for (int block = 0; block < B; block++) {
            for (int index = 0; index < intervals[block].size(); index++) {
                auto unique_left_users = intervals[block][index].users;
                if (index != 0) {
                    auto and_users = intervals[block][index].users & intervals[block][index + 1].users;
                    unique_left_users = unique_left_users ^ and_users;
                }

                for (int u_left: unique_left_users) {
                    for (auto empty: unused_users) {
                        if (users_info[empty].beam == users_info[u_left].beam ||
                            ((intervals[block][index].beam_msk >> users_info[empty].beam) & 1) == 0) {
                            int accepted = min(users_info[u_left].rbNeed, users_info[u_left].sum_len - intervals[block][index].len) +
                                           min(users_info[empty].rbNeed, /*users_info[empty].sum_len*/ +intervals[block][index].len);
                            ips.emplace_back(accepted, block, index, u_left, empty, true);
                        }
                    }
                }
            }
        }

        if (ips.empty()) {
            return;
        }

        //sort(ips.begin(), ips.end(), greater<>());
        //int p = 0;
        int p = rnd.get(0, ips.size() - 1);
        block = get<1>(ips[p]);
        index = get<2>(ips[p]);
        u_left = get<3>(ips[p]);
        u_right = get<4>(ips[p]);
        type = get<5>(ips[p]);
    }

    ASSERT(block != -1, "kek");

    int old_actions_size = actions.size();
    auto old_metric = metric;

    if (type) {
        remove_user_in_interval(u_left, block, index);
        add_user_in_interval(u_right, block, index);
    } else {
        ASSERT(false, "kek");
        remove_user_in_interval(u_left, block, index);
        add_user_in_interval(u_right, block, index);
    }

    if (is_good(old_metric)) {
        if (metric.accepted > old_metric.accepted) {
            //cout << "GOOOOOOD" << endl;
        }

    } else {
        rollback(old_actions_size);
        ASSERT(old_metric == metric, "failed back score");
    }
}
