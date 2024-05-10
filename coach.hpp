constexpr int test_case_K_sizes[] = {666, 215, 80, 39, 0};

#include <atomic>
#include <thread>

randomizer rnd;

struct test_case_info {
    int tests = 0;
    int total_score = 0;
    int total_theory_score = 0;
    double total_time = 0;
    double max_test_time = 0;
};

ostream &operator<<(ostream &output, const test_case_info &info) {
    return output << "tests: " << info.tests
                  << " | score: " << info.total_score * 100.0 / info.total_theory_score << "% | "
                  << info.total_score << "/" << info.total_theory_score
                  << " | time: " << 1000 * info.total_time
                  << "ms | max_time: " << 1000 * info.max_test_time << "ms | mean_time: "
                  << 1000 * info.total_time / max(1, info.tests) << "ms";
}

vector<vector<pair<TestData, vector<Interval>>>> train_dataset;

// (train_score, score)
pair<long long, int> calc_train_score(const vector<int> &powers) {
    auto do_test = [&]() {
        int total_score = 0;
        long long train_score = 0;
        for (int K = 0; K <= 3; K++) {
            for (auto &[data, start_intervals]: train_dataset[K]) {
                //THEORY_MAX_SCORE = get_theory_max_score(data.N, data.M, data.K, data.J, data.L, data.reservedRBs, data.userInfos);
                int score1 = 0;
                {
                    //SELECTION_ACTION = powers;
                    //auto intervals = Solver_egor(data.N, data.M, data.K, data.J, data.L, data.reservedRBs, data.userInfos, start_intervals, 42, powers);
                    //score1 = get_solution_score(data, intervals);
                    EgorTaskSolver solver(data.N, data.M, data.K, data.J, data.L, data.reservedRBs, data.userInfos, start_intervals, 42, powers);
                    auto answer = solver.annealing(data.reservedRBs, data.userInfos);
                    score1 = get_solution_score(data, answer);
                    train_score += solver.TRAIN_SCORE;
                }
                int score2 = 0;
                {
                    //SELECTION_ACTION = powers;
                    //auto intervals = Solver_egor(data.N, data.M, data.K, data.J, data.L, data.reservedRBs, data.userInfos, start_intervals, 4128635, powers);
                    //score2 = get_solution_score(data, intervals);

                    EgorTaskSolver solver(data.N, data.M, data.K, data.J, data.L, data.reservedRBs, data.userInfos, start_intervals, 4128635, powers);
                    auto answer = solver.annealing(data.reservedRBs, data.userInfos);
                    score2 = get_solution_score(data, answer);
                    train_score += solver.TRAIN_SCORE;
                }
                total_score += (score1 + score2) / 2;
            }
        }
        train_score /= 2;
        return pair<long long, int>(train_score, total_score);
    };

    return do_test();
}

struct TrainItem {
    vector<int> powers;
    long long train_score = -1;
    long long score = -1;

    friend bool operator<(const TrainItem &lhs, const TrainItem &rhs) {
        return lhs.train_score > rhs.train_score;
    }
};

TrainItem merge_gens(TrainItem lhs, const TrainItem &rhs) {
    uint32_t msk = rnd.get(0, (1ULL << SELECTION_SIZE) - 1);
    for (int i = 0; i < SELECTION_SIZE; i++) {
        if ((msk >> i) & 1) {
            lhs.powers[i] = rhs.powers[i];
        }
    }

    if (rnd.get_d() < 0.3) {
        int i = rnd.get(0, SELECTION_SIZE - 1);
        lhs.powers[i] = max(0, lhs.powers[i] + (int) rnd.get(-10, 10));
    }

    return lhs;
}

void train_egor_task_solver() {
    train_dataset.assign(5, {});
    for (int K = 0; K <= 3; K++) {
        string dir = "tests/case_K=" + to_string(K) + "/";
        for (int test = 0; test < test_case_K_sizes[K]; test += 1) {
            ifstream input(dir + to_string(test) + ".txt");
            TestData data;
            input >> data;
            train_dataset[K].push_back({data, Solver_Artem_grad(data.N, data.M, data.K, data.J, data.L, data.reservedRBs, data.userInfos)});
        }
    }

    /*auto merge_score = [&](const vector<pair<int, int>> &ans) {
        double score = 0;
        long long k = 0;
        for (int i = 1; i <= ans.size(); i++) {
            k += i;
        }
        for (int i = 0; i < ans.size(); i++) {
            score += (i + 1) / double(k) * ans[i].first;
        }
        score /= ans[0].second;
        score *= 100;
        return score;
    };*/

    /*auto print_ans = [&](std::ostream &output, const vector<pair<int, int>> &ans) {
        output << "score: " << merge_score(ans) << '\n';
        for (auto [score, max]: ans) {
            output << (score * 100.0 / max) << "% " << score << '/' << max << '\n';
        }
        output << endl;
    };*/

    /*auto compare = [&](const vector<pair<int, int>> &old_ans, const vector<pair<int, int>> &new_ans) {
        return merge_score(old_ans) < merge_score(new_ans);
    };*/

    Timer global_time;

    ofstream logger("train_log.txt");

    long long ans_train_score = 0;
    long long ans_score = 0;

    auto log = [&](const vector<int> &powers) {
        cout << "NICE!!! " << global_time << endl;
        cout << "SCORE: " << ans_score << '\n';
        cout << "TRAIN_SCORE: " << ans_train_score << " | " << (long long) (ans_train_score * 1.0 / STEPS) << '\n';

        logger << "=========================\n";
        logger << "=========================\n";
        logger << "TIME: " << global_time << '\n';
        logger << "SCORE: " << ans_score << '\n';
        logger << "TRAIN_SCORE: " << ans_train_score << " | " << (long long) (ans_train_score * 1.0 / STEPS) << '\n';
        logger << "POWERS: ";
        for (int p: powers) {
            logger << p << ' ';
        }
        logger << '\n'
               << endl;
        logger.flush();
    };

    vector<TrainItem> Q;
    cout << "BUILDING..." << endl;
    for (int i = 0; i < 20; i++) {
        vector<int> powers(SELECTION_SIZE);
        for (int j = 0; j < SELECTION_SIZE; j++) {
            powers[j] = rnd.get(0, 100);
        }
        auto [train_score, score] = calc_train_score(powers);
        Q.push_back({powers, train_score, score});
    }
    sort(Q.begin(), Q.end());
    cout << "OK" << endl;

    for (int step = 0;; step++) {
        cout << "step: " << step << ' ' << global_time << ' ' << Q[0].score << ' ' << Q.back().score << endl;

        vector<TrainItem> newQ;

        for (int k = 0; k < 50; k++) {
            vector<int> powers(SELECTION_SIZE);
            for (int j = 0; j < SELECTION_SIZE; j++) {
                powers[j] = rnd.get(0, 100);
            }
            newQ.push_back({powers, -1, -1});
        }

        for (int k = 0; k < 200; k++) {
            int i = rnd.get(0, Q.size() - 1);
            int j = rnd.get(0, Q.size() - 1);
            auto new_item = merge_gens(Q[i], Q[j]);
            newQ.push_back(new_item);
        }

        for (int k = 0; k < 100; k++) {
            auto new_item = Q[rnd.get(0, min((int) Q.size() - 1, 5))];
            uint32_t msk = rnd.get(0, (1ULL << SELECTION_SIZE) - 1);
            for (int i = 0; i < SELECTION_SIZE; i++) {
                if ((msk >> i) & 1) {
                    new_item.powers[i]++;
                } else {
                    new_item.powers[i]--;
                    new_item.powers[i] = max(0, new_item.powers[i]);
                }
            }
            newQ.push_back(new_item);
        }
        vector<atomic<bool>> vis(newQ.size());
        auto do_work = [&]() {
            for (int i = 0; i < newQ.size(); i++) {
                bool expected = false;
                if (vis[i].compare_exchange_strong(expected, true)) {
                    auto [train_score, score] = calc_train_score(newQ[i].powers);
                    newQ[i].train_score = train_score;
                    newQ[i].score = score;
                }
            }
        };

        // TODO: multithread
        constexpr int threads_count = 16;
        vector<thread> threads(threads_count);
        for (int i = 0; i < threads_count; i++) {
            threads[i] = thread(do_work);
        }
        for (int i = 0; i < threads_count; i++) {
            threads[i].join();
        }

        for (auto &i: Q) {
            newQ.push_back(move(i));
        }
        sort(newQ.begin(), newQ.end());
        Q = move(newQ);

        while (Q.size() > 400) {
            Q.pop_back();
        }

        if (Q[0].train_score > ans_train_score) {
            ans_train_score = Q[0].train_score;
            ans_score = Q[0].score;
            log(Q[0].powers);
        }
    }

    /*for (int step = 0;; step++) {
        cout << step << ' ' << global_time << endl;

        bool ok = false;

        vector<tuple<int, int, int, int>> ips;
        for (int i = 0; i < 11; i++) {
            for (int j = 0; j < 11; j++) {

                for (int c1 = max(-SELECTION_ACTION.kit[i].second, -5); c1 <= 5; c1++) {
                    for (int c2 = max(-SELECTION_ACTION.kit[j].second, -5); c2 <= 5; c2++) {
                        if (!(c1 == 0 && c2 == 0)) {
                            ips.push_back({i, j, c1, c2});
                        }
                    }
                }
            }
        }
        shuffle(ips.begin(), ips.end(), rnd.generator);
        for (auto [i, j, c1, c2]: ips) {
            if (SELECTION_ACTION.kit[i].second + c1 >= 0 && SELECTION_ACTION.kit[j].second + c2 >= 0) {
                SELECTION_ACTION.kit[i].second += c1;
                SELECTION_ACTION.kit[j].second += c2;

                auto new_ans = calc_train_score();
                if (compare(ans, new_ans)) {
                    ok = true;
                    ans = new_ans;

                    cout << "UPDATE ANS: " << global_time << endl;
                    print_ans(cout, ans);

                    if (compare(super_ans, ans)) {
                        super_ans = ans;
                        log();
                    }
                } else {
                    SELECTION_ACTION.kit[i].second -= c1;
                    SELECTION_ACTION.kit[j].second -= c2;
                }
            }
        }

        if (!ok) {
            cout << "BAD" << endl;
            for (int i = 0; i < SELECTION_ACTION.kit.size(); i++) {
                SELECTION_ACTION.kit[i].second++;
            }
        }
    }*/
}
