constexpr int test_case_K_sizes[] = {666, 215, 80, 39, 0};

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

// (score, theor_max)
vector<pair<int, int>> calc_train_score() {
    auto do_test = [&]() {
        //for (int i = 0; i < SELECTION_ACTION.size(); i++) {
        //    SELECTION_ACTION[i].reset_rnd();
        //}
        int total_score = 0;
        int total_theor_max = 0;
        for (int K = 0; K <= 3; K++) {
            for (auto &[data, start_intervals]: train_dataset[K]) {
                THEORY_MAX_SCORE = get_theory_max_score(data.N, data.M, data.K, data.J, data.L, data.reservedRBs, data.userInfos);
                int score1 = 0;
                {
                    SELECTION_ACTION.reset_rnd();
                    auto intervals = Solver_egor(data.N, data.M, data.K, data.J, data.L, data.reservedRBs, data.userInfos, start_intervals, 42);

                    score1 = get_solution_score(data, intervals);
                }
                int score2 = 0;
                {
                    SELECTION_ACTION.reset_rnd();
                    auto intervals = Solver_egor(data.N, data.M, data.K, data.J, data.L, data.reservedRBs, data.userInfos, start_intervals, 4128635);

                    score2 = get_solution_score(data, intervals);
                }
                total_score += (score1 + score2) / 2;

                int theor_max = get_theory_max_score(data);
                total_theor_max += theor_max;
            }
        }
        return make_pair(total_score, total_theor_max);
    };

    vector<pair<int, int>> ans;

    //STEPS = 400;
    //ans.push_back(do_test());
    //STEPS = 600;
    //ans.push_back(do_test());
    STEPS = 800;
    ans.push_back(do_test());
    return ans;
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

    randomizer rnd;

    auto merge_score = [&](const vector<pair<int, int>> &ans) {
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
    };

    auto print_ans = [&](std::ostream &output, const vector<pair<int, int>> &ans) {
        output << "score: " << merge_score(ans) << '\n';
        for (auto [score, max]: ans) {
            output << (score * 100.0 / max) << "% " << score << '/' << max << '\n';
        }
        output << endl;
    };

    auto compare = [&](const vector<pair<int, int>> &old_ans, const vector<pair<int, int>> &new_ans) {
        return merge_score(old_ans) < merge_score(new_ans);
    };

    Timer global_time;

    ofstream logger("train_log.txt");

    auto super_ans = calc_train_score();

    auto log = [&]() {
        cout << "NICE!!! " << global_time << endl;
        print_ans(cout, super_ans);

        logger << "=========================\n";
        logger << "=========================\n";
        logger << "TIME: " << global_time << '\n';
        print_ans(logger, super_ans);
        logger << "SELECTION_ACTION: ";
        logger << SELECTION_ACTION << '\n';
        //for (int i = 0; i < 12; i++) {
        //    logger << i << ": " << SELECTION_ACTION[i] << '\n';
        //}
        logger << endl;
        logger.flush();
    };

    auto ans = super_ans;

    //auto super_puper_ans = ans;
    log();

    for (int step = 0;; step++) {
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

        /*for (int j = 0; j < 11; j++) {
            for (int change = max(-SELECTION_ACTION.kit[j].second, -2); change <= 2; change++) {
                if (change != 0) {
                    SELECTION_ACTION.kit[j].second += change;

                    auto new_ans = calc_train_score();
                    if (compare(ans, new_ans)) {
                        ok = true;
                        ans = new_ans;
                        log();
                    } else {
                        SELECTION_ACTION.kit[j].second -= change;
                    }
                }
            }
        }

        if (!ok) {
            cout << "OH NO, IT'S BAD" << endl;
            int j = rnd.get(0, 10);
            SELECTION_ACTION.kit[j].second++;
        }*/
    }
}