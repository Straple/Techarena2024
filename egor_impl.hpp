#pragma once

EgorTaskSolver::EgorTaskSolver(int NN, int MM, int KK, int JJ, int LL,
                               const vector<Interval> &reservedRBs,
                               const vector<UserInfo> &userInfos,
                               vector<Interval> start_intervals, int random_seed) : N(NN), M(MM), K(KK), J(JJ), L(LL) {

    rnd.generator = mt19937_64(random_seed);

    ASSERT(2 <= L && L <= 16, "invalid L");
    ASSERT(0 < J && J <= 16, "invalid J");

    users_info.resize(N);
    user_id_to_u.resize(N);

    {
        /*for (int beam = 0; beam < 32; beam++) {
            users_with_equal_beam_size[beam] = 0;
        }*/
        for (int u = 0; u < N; u++) {
            ASSERT(u == userInfos[u].id, "are you stupid or something?");
            ASSERT(0 <= userInfos[u].beam && userInfos[u].beam < 32, "invalid beam");
            users_info[u].id = userInfos[u].id;
            users_info[u].rbNeed = userInfos[u].rbNeed;
            users_info[u].beam = userInfos[u].beam;

            //users_with_equal_beam_size[userInfos[u].beam]++;
            user_id_to_u[users_info[u].id] = u;
            users_with_equal_beam[userInfos[u].beam].push_back(userInfos[u].id);
        }
    }

    // build free_intervals
    {
        vector<bool> is_free(M + 1, true);
        is_free[M] = false;
        for (const auto &[start, end, users]: reservedRBs) {
            for (int i = start; i < end; i++) {
                is_free[i] = false;
            }
        }

        int start = -1;
        for (int i = 0; i <= M; i++) {
            if (!is_free[i]) {
                if (start != i - 1) {
                    free_intervals.push_back({start + 1, i});
                    metric.unused_space += i - start - 1;
                }
                start = i;
            }
        }
        B = free_intervals.size();
    }

    {
        users_order.resize(N);
        iota(users_order.begin(), users_order.end(), 0);
        sort(users_order.begin(), users_order.end(), [&](int lhs, int rhs) {
            return users_info[lhs].rbNeed > users_info[rhs].rbNeed;
        });
    }

    // build from start_intervals
    {
        sort(start_intervals.begin(), start_intervals.end(), [&](const auto &lhs, const auto &rhs) {
            return lhs.start < rhs.start;
        });

        intervals.resize(B);

        for (auto [start, end, users]: start_intervals) {
            bool already_push = false;
            for (int block = 0; block < B; block++) {
                if (free_intervals[block].start <= start && end <= free_intervals[block].end) {
                    ASSERT(!already_push, "double push");
                    already_push = true;

                    //intervals[block].push_back({end - start, {}, 0});
                    intervals[block].push_back({0, {}, 0});                              /// !
                    change_interval_len(block, intervals[block].size() - 1, end - start);/// !

                    for (int u: users) {
                        add_user_in_interval(u, block, intervals[block].size() - 1);
                    }
                }
            }
            ASSERT(already_push, "no push");
        }
        for (int i = start_intervals.size(); i < J; i++) {
            intervals[rnd.get(0, B - 1)].push_back(SetInterval());
        }
    }
}


//#define ACTION_WRAPPER(action_foo, action_id)     \
    int old_score = total_score;                  \
    cnt_edges[prev_action][action_id]++;          \
    action_foo();                                 \
    if (score_prev_action < total_score) {        \
        cnt_good_edges[prev_action][action_id]++; \
    }                                             \
    prev_action = action_id;                      \
    score_prev_action = old_score;

#define ACTION_WRAPPER(action_foo, action_id) \
    action_foo();

vector<Interval> EgorTaskSolver::annealing(vector<Interval> reservedRBs,
                                           vector<UserInfo> userInfos) {
    temperature = 1;
    prev_action = 0;

    //int best_score = total_score;
    //auto best_ans = get_total_answer();

    for (int step = 0; step < STEPS; step++) {
        temperature = ((STEPS - step) * 1.0 / STEPS);
        //temperature *= 0.9999;

        //ASSERT(get_solution_score(N, M, K, J, L, reservedRBs, userInfos, get_total_answer()) == total_score, "invalid total_score");
        //if (THEORY_MAX_SCORE <= total_score) {
        //    break;
        //}

        int s = SELECTION_ACTION.select();
        if (s == 0) {
            ACTION_WRAPPER(user_new_interval, 0);
        } else if (s == 1) {
            ACTION_WRAPPER(user_add_left, 1);
        } else if (s == 2) {
            ACTION_WRAPPER(user_remove_left, 2);
        } else if (s == 3) {
            ACTION_WRAPPER(user_add_right, 3);
        } else if (s == 4) {
            ACTION_WRAPPER(user_remove_right, 4);
        } else if (s == 5) {
            ACTION_WRAPPER(user_swap, 5);
        } else if (s == 6) {
            ACTION_WRAPPER(interval_increase_len, 6);
        } else if (s == 7) {
            ACTION_WRAPPER(interval_decrease_len, 7);
        } else if (s == 8) {
            ACTION_WRAPPER(interval_flow_over, 8);
        } else if (s == 9) {
            ACTION_WRAPPER(interval_merge, 9);
        } else if (s == 10) {
            ACTION_WRAPPER(interval_split, 10);
        } else if (s == 11) {

        } else {
            ASSERT(false, "kek");
        }

        /*if (best_score < total_score) {
            best_score = total_score;
            best_ans = get_total_answer();
        }*/

        actions.clear();

        SNAP(snapshoter.write(get_total_answer(), "annealing"));
    }

    //for (int u = 0; u < N; u++) {
    //user_do_crop(u);
    //}
    return get_total_answer();
    //return best_ans;
}

vector<Interval> Solver_egor(int N, int M, int K, int J, int L,
                             const vector<Interval> &reservedRBs,
                             const vector<UserInfo> &userInfos, const std::vector<Interval> &solution, int random_seed) {
    EgorTaskSolver solver(N, M, K, J, L, reservedRBs, userInfos, solution, random_seed);
    auto answer = solver.annealing(reservedRBs, userInfos);
    ASSERT(solver.metric.accepted == get_solution_score(N, M, K, J, L, reservedRBs, userInfos, answer), "invalid total_score");
    return answer;
}