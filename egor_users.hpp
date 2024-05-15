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

    for (int step = 0; step < 2; step++) {
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

            /*if(metric.accepted > old_metric.accepted) {
                auto [new_b, new_l, new_r] = get_user_position(u);
                auto [new_b2, new_l2, new_r2] = get_user_position(u2);

                cout << users_info[u].beam << ' ' << old_b << ' ' << old_l << ' ' << old_r << '\n';
                cout << users_info[u2].beam << ' ' << old_b2 << ' ' << old_l2 << ' ' << old_r2 << '\n';
                cout << users_info[u].beam << ' ' << new_b << ' ' << new_l << ' ' << new_r << '\n';
                cout << users_info[u2].beam << ' ' << new_b2 << ' ' << new_l2 << ' ' << new_r2 << '\n';
                cout << "============\n";
            }*/
        } else {

            rollback(old_actions_size);

            ASSERT(old_metric == metric, "failed back score");
        }
    }
}
