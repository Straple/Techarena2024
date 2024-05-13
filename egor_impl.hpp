#pragma once

EgorTaskSolver::EgorTaskSolver(int NN, int MM, int KK, int JJ, int LL,
                               const vector<Interval> &reservedRBs,
                               const vector<UserInfo> &userInfos,
                               vector<Interval> start_intervals, int random_seed, vector<int> powers,
                               vector<int> metric_coefs) : N(NN), M(MM),
                                                           K(KK), J(JJ),
                                                           L(LL),
                                                           SELECTION_ACTION(
                                                                   powers),
                                                           METRICS_COEF(metric_coefs) {

    rnd.generator = mt19937_64(random_seed);

    ASSERT(METRICS_COEF.size() == METRIC_COEFFS_SIZE, "invalid size");
    ASSERT(2 <= L && L <= 16, "invalid L");
    ASSERT(0 < J && J <= 16, "invalid J");

    users_info.resize(N);
    user_id_to_u.resize(N);

    {
        for (int u = 0; u < N; u++) {
            ASSERT(u == userInfos[u].id, "are you stupid or something?");
            ASSERT(0 <= userInfos[u].beam && userInfos[u].beam < 32, "invalid beam");
            users_info[u].id = userInfos[u].id;
            users_info[u].rbNeed = userInfos[u].rbNeed;
            users_info[u].beam = userInfos[u].beam;
        }
        for (int u = 0; u < N; u++) {
            user_id_to_u[users_info[u].id] = u;
            users_beam[userInfos[u].beam].push_back(u);
        }

        // TODO: можно еще для каждого beam оставить только J первых
        for (int beam = 0; beam < 32; beam++) {
            sort(users_beam[beam].begin(), users_beam[beam].end(), [&](int lhs, int rhs) {
                return users_info[lhs].rbNeed > users_info[rhs].rbNeed;
            });
            for (int i = 0; i < users_beam[beam].size(); i++) {
                users_info[users_beam[beam][i]].pos = i;
            }
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

#define ACTION_WRAPPER(action_foo, action_id) \
    action_foo();

//#define ACTION_WRAPPER(action_foo, action_id) \
    action_foo();
int accepted_inc[10];
int accepted_more[10];

int itt = 0;
vector<Interval> EgorTaskSolver::annealing(vector<Interval> reservedRBs,
                                           vector<UserInfo> userInfos) {
    itt++;
    temperature = 1;
    prev_action = 0;

//#define SAVE_BEST_ANS

#ifdef SAVE_BEST_ANS
    int best_score = metric.accepted;
    auto best_users_info = users_info;
    auto best_intervals = intervals;
#endif

    int best_f = 100 * metric.accepted - 10 * metric.unused_space - metric.overflow + metric.free_space;
    int there_has_been_no_improvement_for_x_steps = 0;

    for (int step = 0; step < STEPS; step++) {
        temperature = ((STEPS - step) * 0.01 / STEPS);
        //cout << "TEMPER: " << temperature << endl;
        was_accepted = false;
        //temperature *= 0.9999;

        //cout << "step: " << step << endl;
        //if(step == 4999){
        //    cout << "HERE" << endl;
        //}

        /*if (step > STEPS / 2) {
            if (best_f >= 100 * metric.accepted - 10 * metric.unused_space - metric.overflow + metric.free_space) {
                there_has_been_no_improvement_for_x_steps++;
                if (there_has_been_no_improvement_for_x_steps > 200) {
                    break;
                }
            } else {
                there_has_been_no_improvement_for_x_steps = 0;
                best_f = 100 * metric.accepted - 10 * metric.unused_space - metric.overflow + metric.free_space;
            }
        }*/

        //if(step > STEPS / 2)
        /*{
            if (best_f >= 100 * metric.accepted - 10 * metric.unused_space - metric.overflow + metric.free_space) {
                there_has_been_no_improvement_for_x_steps++;
                if (there_has_been_no_improvement_for_x_steps > 300) {
                    there_has_been_no_improvement_for_x_steps = 0;
                    earthquake();
                    best_f = 100 * metric.accepted - 10 * metric.unused_space - metric.overflow + metric.free_space;
                }
            } else {
                there_has_been_no_improvement_for_x_steps = 0;
                best_f = 100 * metric.accepted - 10 * metric.unused_space - metric.overflow + metric.free_space;
            }
        }*/

        //977151

        /*if (step < STEPS / 2) {
            METRIC_TYPE = 1;
        } else {
            METRIC_TYPE = 0;
        }*/

        //TRAIN_SCORE += best_score;

        ASSERT(get_solution_score(N, M, K, J, L, reservedRBs, userInfos, get_total_answer()) == metric.accepted,
               "invalid total_score");
        if (THEORY_MAX_SCORE <= metric.accepted) {
            break;
        }
        auto old_metric = metric;

        int s = SELECTION_ACTION.select();
        if (s == 0) {
            ACTION_WRAPPER(user_new_interval, s);
        } else if (s == 1) {
            ACTION_WRAPPER(user_add_left, s);
        } else if (s == 2) {
            ACTION_WRAPPER(user_remove_left, s);
        } else if (s == 3) {
            ACTION_WRAPPER(user_add_right, s);
        } else if (s == 4) {
            ACTION_WRAPPER(user_remove_right, s);
        } else if (s == 5) {
            ACTION_WRAPPER(user_remove_and_add, s);
        } else if (s == 6) {
            ACTION_WRAPPER(interval_increase_len, s);
        } else if (s == 7) {
            ACTION_WRAPPER(interval_flow_over, s);
        } else if (s == 8) {
            ACTION_WRAPPER(interval_merge, s);
        } else if (s == 9) {
            ACTION_WRAPPER(interval_split, s);
        } else if (s == 10) {
            ACTION_WRAPPER(beam_rebuild, s);
        } else {
            ASSERT(false, "kek");
        }
        if (metric.accepted > old_metric.accepted){
            accepted_inc[s]++;
            accepted_more[s]+=metric.accepted-old_metric.accepted;
        }

        //if(metric.accepted == old_metric.accepted){
        //    CNT_ACCEPTED_EQ++;
        //}

#ifdef SAVE_BEST_ANS
        if (best_score < metric.accepted) {
            best_score = metric.accepted;
            best_users_info = users_info;
            best_intervals = intervals;
        }
#endif

        actions.clear();

        //SNAP(snapshoter.write(get_total_answer(), "annealing"));
    }
//    cout << itt << endl;
    /*if (itt == 610){
        for (int i = 0; i < 10; i++){
            cout << accepted_inc[i] << " ";
        }
        cout << endl;
        for (int i = 0; i < 10; i++){
            cout << accepted_more[i] << " ";
        }
        cout << endl;
    }*/

#ifdef SAVE_BEST_ANS
    users_info = best_users_info;
    intervals = best_intervals;
    return get_total_answer();
#else
    return get_total_answer();
#endif
}

vector<Interval> Solver_egor(int N, int M, int K, int J, int L,
                             const vector<Interval> &reservedRBs,
                             const vector<UserInfo> &userInfos, const std::vector<Interval> &solution, int random_seed,
                             vector<int> powers, vector<int> metric_coefs) {
    EgorTaskSolver solver(N, M, K, J, L, reservedRBs, userInfos, solution, random_seed, powers, metric_coefs);
    auto answer = solver.annealing(reservedRBs, userInfos);
    //ASSERT(solver.metric.accepted == get_solution_score(N, M, K, J, L, reservedRBs, userInfos, answer), "invalid total_score");
    return answer;
}
