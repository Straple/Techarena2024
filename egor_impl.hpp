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

    // build from start_intervals
    {
        sort(start_intervals.begin(), start_intervals.end(), [&](const auto &lhs, const auto &rhs) {
            return lhs.start < rhs.start;
        });

        intervals.resize(B);

        for (auto [start, end, users]: start_intervals) {
            ASSERT(start < end, "invalid interval");
            bool already_push = false;
            for (int block = 0; block < B; block++) {
                if (free_intervals[block].start <= start && end <= free_intervals[block].end) {
                    ASSERT(!already_push, "double push");
                    already_push = true;

                    intervals[block].push_back({0, {}, 0});
                    change_interval_len(block, intervals[block].size() - 1, end - start);

                    for (int u: users) {
                        add_user_in_interval(u, block, intervals[block].size() - 1);
                    }
                }
            }
            ASSERT(already_push, "no push");
        }

        {
            vector<pair<int, int>> ips;
            for (int block = 0; block < B; block++) {
                int len = get_block_len(block);
                if (len < free_intervals[block].len()) {
                    if (!intervals[block].empty()) {
                        change_interval_len(block, intervals[block].size() - 1, free_intervals[block].len() - len);
                    } else {
                        ips.emplace_back(free_intervals[block].len(), block);
                    }
                }
            }

            sort(ips.begin(), ips.end(), greater<>());
            for (auto [len, block]: ips) {
                if (get_intervals_size() < J) {
                    intervals[block].push_back(SetInterval());
                    change_interval_len(block, 0, free_intervals[block].len());
                }
            }
        }
    }
}

#define ACTION_WRAPPER(action_foo, action_id) \
    action_foo();

int accepted_inc[10];
int accepted_more[10];

int ABSYBDAYSBD = 0;

int itt = 0;
vector<Interval> EgorTaskSolver::annealing(vector<Interval> reservedRBs,
                                           vector<UserInfo> userInfos) {
    itt++;
    temperature = 1;

    //#define SAVE_BEST_ANS

#ifdef SAVE_BEST_ANS
    int best_score = metric.accepted;
    auto best_users_info = users_info;
    auto best_intervals = intervals;
#endif

    //int best_f = 100 * metric.accepted - 10 * metric.unused_space - metric.overflow + metric.free_space;
    //int there_has_been_no_improvement_for_x_steps = 0;

    for (int step = 0; step < STEPS; step++) {
        temperature = ((STEPS - step) * 0.01 / STEPS);
        //cout << "TEMPER: " << temperature << endl;
        was_accepted = false;
        //temperature *= 0.9999;

        //cout << "step: " << step << endl;
        //if(step == 4999){
        //    cout << "HERE" << endl;
        //}

        //TRAIN_SCORE += best_score;

#ifdef MY_DEBUG_MODE
        for (int block = 0; block < B; block++) {
            for (int index = 0; index < intervals[block].size(); index++) {
                if (intervals[block][index].len == 0) {
                    ASSERT(false, "zero interval");
                }

                if(index + 1 < intervals[block].size()){
                    if(intervals[block][index].users == intervals[block][index + 1].users &&
                        intervals[block][index].users.size() == L){
                        ABSYBDAYSBD++;

                        //change_interval_len(block, index, intervals[block][index + 1].len);
                        //remove_interval(block, index + 1);
                    }
                }
            }
        }
#endif

        ASSERT(get_solution_score(N, M, K, J, L, reservedRBs, userInfos, get_total_answer()) == metric.accepted,
               "invalid total_score");
        if (THEORY_MAX_SCORE <= metric.accepted) {
            break;
        }
        auto old_metric = metric;

        int s = SELECTION_ACTION.select();
        if (s == 0) {
            ACTION_WRAPPER(user_remove_and_add, s);
        } else if (s == 1) {
            ACTION_WRAPPER(interval_flow_over, s);
        } else if (s == 2) {
            ACTION_WRAPPER(interval_split, s);
        } else {
            ASSERT(false, "kek");
        }

        if (metric.accepted > old_metric.accepted) {
            accepted_inc[s]++;
            accepted_more[s] += metric.accepted - old_metric.accepted;
        }

#ifdef SAVE_BEST_ANS
        if (best_score < metric.accepted) {
            best_score = metric.accepted;
            best_users_info = users_info;
            best_intervals = intervals;
        }
#endif

        actions.clear();

#ifdef VERIFY_ARTEM_ANS
        actions.clear();
        auto ans = get_total_answer();
        auto get_egor_blocked = ans_to_blocked_ans(M, K, reservedRBs, ans);
        optimize(N, M, K, J, L, reservedRBs, userInfos, get_egor_blocked, true);
        auto egor_answer = unblock_ans(get_egor_blocked);

        if (egor_answer.size() > J) {
            ans = get_total_answer();
            ::abort();
        }
#endif

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
