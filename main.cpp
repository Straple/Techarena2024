#include "solution.cpp"

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
        EGOR_TASK_SOLVER_SELECTION_USER_ACTION.reset_rnd();
        EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION.reset_rnd();
        EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION.reset_rnd();
        int total_score = 0;
        int total_theor_max = 0;
        for (int K = 0; K <= 3; K++) {
            for (auto &[data, start_intervals]: train_dataset[K]) {
                auto intervals = Solver_egor(data.N, data.M, data.K, data.J, data.L, data.reservedRBs, data.userInfos, start_intervals);

                int score = get_solution_score(data, intervals);
                total_score += score;

                int theor_max = get_theory_max_score(data);
                total_theor_max += theor_max;
            }
        }
        return make_pair(total_score, total_theor_max);
    };

    vector<pair<int, int>> ans;

    STEPS = 100;
    ans.push_back(do_test());
    STEPS = 200;
    ans.push_back(do_test());
    STEPS = 400;
    ans.push_back(do_test());
    STEPS = 600;
    ans.push_back(do_test());
    //STEPS = 800;
    //ans.push_back(do_test());
    //STEPS = 1000;
    //ans.push_back(do_test());
    //STEPS = 2'000;
    //ans.push_back(do_test());
    //STEPS = 5'000;
    //ans.push_back(do_test());
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

    auto ans = calc_train_score();

    Timer global_time;

    ofstream logger("train_log.txt");

    auto log = [&]() {
        cout << "NICE!!! " << global_time << endl;
        print_ans(cout, ans);

        logger << "=========================\n";
        logger << "=========================\n";
        logger << "TIME: " << global_time << '\n';
        print_ans(logger, ans);
        logger << "USER:\n";
        logger << EGOR_TASK_SOLVER_SELECTION_USER_ACTION;
        logger << "INTERVAL:\n";
        logger << EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION;
        logger << "USER OR INTERVAL:\n";
        logger << EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION;
        logger << endl;
        logger.flush();
    };

    auto super_puper_ans = ans;

    for (int step = 0;; step++) {
        cout << step << ' ' << global_time << endl;

        auto brute = [&](SelectionRandomizer &selection) {
            auto best_ans = ans;
            long long best_x = 0;

            constexpr long long BASE = 2;

            long long MAX_MSK = 1;
            for (int i = 0; i < selection.kit.size(); i++) {
                MAX_MSK *= BASE;
            }

            for (long long x = 0; x < MAX_MSK; x++) {
                auto old_user = EGOR_TASK_SOLVER_SELECTION_USER_ACTION;
                auto old_interval = EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION;
                auto old_user_or_interval = EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION;


                long long msk = x;
                for (int i = 0; i < selection.kit.size(); i++, msk /= BASE) {
                    int change = (msk % BASE) == 0 ? 1 : -1;
                    selection.kit[i].second += change;
                    selection.kit[i].second = max(0, selection.kit[i].second);
                }

                auto new_ans = calc_train_score();
                if (compare(best_ans, new_ans)) {
                    best_ans = new_ans;
                    best_x = x;
                }

                EGOR_TASK_SOLVER_SELECTION_USER_ACTION = old_user;
                EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION = old_interval;
                EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION = old_user_or_interval;
            }

            if (compare(ans, best_ans)) {
                ans = best_ans;

                long long msk = best_x;
                for (int i = 0; i < selection.kit.size(); i++, msk /= BASE) {
                    int change = (msk % BASE) == 0 ? 1 : -1;
                    selection.kit[i].second += change;
                    selection.kit[i].second = max(0, selection.kit[i].second);
                }

                ASSERT(ans == calc_train_score(), "invalid ans");

                if (compare(super_puper_ans, ans)) {
                    super_puper_ans = ans;
                    log();
                }

                return true;
            } else {
                return false;
            }
        };

        if (brute(EGOR_TASK_SOLVER_SELECTION_USER_ACTION) |
            brute(EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION) |
            brute(EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION)) {

        } else {
            // bad

            for (int i = 0; i < EGOR_TASK_SOLVER_SELECTION_USER_ACTION.kit.size(); i++) {
                EGOR_TASK_SOLVER_SELECTION_USER_ACTION.kit[i].second++;
            }

            for (int i = 0; i < EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION.kit.size(); i++) {
                EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION.kit[i].second++;
            }

            for (int i = 0; i < EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION.kit.size(); i++) {
                EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION.kit[i].second++;
            }

            ans = calc_train_score();
        }

        /*for (int steps = 0; steps < 2; steps++) {

            double p = rnd.get_d();
            if (p < 0.4) {
                EGOR_TASK_SOLVER_SELECTION_USER_ACTION.random_kit();
            } else if (p < 0.8) {
                EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION.random_kit();
            } else {
                EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION.random_kit();
            }

            auto new_ans = calc_train_score();
            if (compare(new_ans)) {
                ans = new_ans;
                old_user = EGOR_TASK_SOLVER_SELECTION_USER_ACTION;
                old_interval = EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION;
                old_user_or_interval = EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION;
                log();
            }
        }

        EGOR_TASK_SOLVER_SELECTION_USER_ACTION = old_user;
        EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION = old_interval;
        EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION = old_user_or_interval;
        auto new_ans = calc_train_score();
        ASSERT(merge_score(ans) == merge_score(new_ans), "invalid version back");*/

        /*double p = rnd.get_d();
        if (p < 0.4) {
            auto old = EGOR_TASK_SOLVER_SELECTION_USER_ACTION;
            for (int steps = 0; steps < 10; steps++) {
                EGOR_TASK_SOLVER_SELECTION_USER_ACTION.random_kit();

                auto new_ans = calc_train_score();
                if (compare(new_ans)) {
                    ans = new_ans;
                    old = EGOR_TASK_SOLVER_SELECTION_USER_ACTION;
                    log();
                } else {
                    EGOR_TASK_SOLVER_SELECTION_USER_ACTION = old;
                }
            }
        } else if (p < 0.8) {
            auto old = EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION;
            EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION.random_kit();

            auto new_ans = calc_train_score();
            if (compare(new_ans)) {
                ans = new_ans;
                log();
            } else {
                EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION = old;
            }
        } else {
            auto old = EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION;
            EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION.random_kit();

            auto new_ans = calc_train_score();
            if (compare(new_ans)) {
                ans = new_ans;
                log();
            } else {
                EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION = old;
            }
        }*/
    }
}

int main() {
    //train_egor_task_solver();
    //return 0;
    //auto egor_answer = Solver_egor(N, M, K, J, L, reservedRBs, userInfos, artem_answer)
    //EgorTaskSolver solver(1, 10, 1, 1, 2, vector<Interval>{Interval{3, 5, {}}}, vector<UserInfo>{{4, 1, 0}}, {});
    //solver.change_interval_len(0, 3);
    //return 0;

    /*MyBitSet<1024> s;
    s.insert(63);
    s.insert(64);
    s.insert(65);
    s.insert(250);
    s.insert(514);
    s.insert(512);
    s.insert(513);
    s.insert(511);

    cout << s.size() << endl;
    cout << "=================\n";

    for (int i = 0; i < 1024; i++) {
        if (s.contains(i)) {
            cout << i << endl;
        }
    }
    cout << "=================\n";

    for (int x: s) {
        cout << x << endl;
    }

    return 0;*/
    test_case_info infos[5];
    map<int, map<int, int>> score_per_test;
    for (int K = 0; K <= 3; K++) {
        cout << "TEST CASE: K=" << K << endl;
        string dir = "tests/case_K=" + to_string(K) + "/";
        std::vector<pair<float, int>> tests_and_scores;
        infos[K].tests = test_case_K_sizes[K];
        for (int test = 0; test < test_case_K_sizes[K]; test++) {
            ifstream input(dir + to_string(test) + ".txt");
            TestData data;
            input >> data;

            Timer timer;
            //            cout << test << "!" << endl;
            auto intervals = Solver(data);

            std::ofstream out("ans_data_art/case_K=" + to_string(K) + "/" + to_string(test) + ".txt");
            out << intervals.size() << endl;
            for (int i = 0; i < intervals.size(); i++) {
                out << intervals[i].start << " " << intervals[i].end << endl;
                out << intervals[i].users.size() << endl;
                for (auto user_id: intervals[i].users) {
                    out << user_id << " ";
                }
                out << endl;
            }
            out.close();

            double time = timer.get();
            infos[K].total_time += time;
            infos[K].max_test_time = max(infos[K].max_test_time, time);

            int score = get_solution_score(data, intervals);
            score_per_test[K][test] = score;

            infos[K].total_score += score;
            int theor_max = get_theory_max_score(data);
            infos[K].total_theory_score += theor_max;

            //cout << score <<  " _ " <<  get_theory_max_score(data) << endl;
            //tests_and_scores.push_back({(float)score/theor_max, test});
        }
        sort(tests_and_scores.begin(), tests_and_scores.end());
        for (int i = 0; i < min((int) tests_and_scores.size(), 10); i++) {
            cout << tests_and_scores[i].first << " " << tests_and_scores[i].second << endl;
        }
    }

    test_case_info total_info;
    for (int K = 0; K <= 4; K++) {
        total_info.tests += infos[K].tests;
        total_info.total_score += infos[K].total_score;
        total_info.total_theory_score += infos[K].total_theory_score;
        total_info.total_time += infos[K].total_time;
        total_info.max_test_time = max(total_info.max_test_time, infos[K].max_test_time);
    }
    /*cout << "{";
    for (auto &[k, tests]: score_per_test) {
        for (auto &[test, score]: tests) {
            cout << "\"" << k << "_" << test << "\": " << score << ", ";
        }
    }
    cout << "}" << endl;*/

    {
        cout << "EGOR TASK SOLVER STATISTIC:\n";

#define CAT_IMPL(string1, string2) string1##string2

#define CAT(string1, string2) CAT_IMPL(string1, string2)

#define f(x) cout << #x ": " << x / 1000.0 << '\n';
#define g(x) cout << #x ": " << CAT(CNT_ACCEPTED_, x) * 100.0 / CAT(CNT_CALL_, x) << "% " << CAT(CNT_ACCEPTED_, x) / 1000 << "/" << CAT(CNT_CALL_, x) / 1000 << '\n';

        f(CNT_CALL_GET_LEFT_USER);
        f(CNT_CALL_GET_RIGHT_USER);
        cout << "=================\n";
        f(CNT_CALL_CHANGE_INTERVAL_LEN);
        cout << "=================\n";
        f(CNT_CALL_ADD_USER_IN_INTERVAL);
        f(CNT_CALL_REMOVE_USER_IN_INTERVAL);
        cout << "=================\n";
        g(INTERVAL_FLOW_OVER);
        g(INTERVAL_INCREASE_LEN);
        g(INTERVAL_DECREASE_LEN);
        g(INTERVAL_GET_FULL_FREE_SPACE);

        f(CNT_CALL_INTERVAL_DO_MERGE_EQUAL);
        f(CNT_CALL_INTERVAL_DO_SPLIT);

        g(INTERVAL_MERGE_EQUAL);
        g(INTERVAL_SPLIT);

        cout << "=================\n";
        g(USER_NEW_INTERVAL);
        g(USER_ADD_LEFT);
        g(USER_REMOVE_LEFT);
        g(USER_ADD_RIGHT);
        g(USER_REMOVE_RIGHT);

        f(CNT_CALL_USER_DO_CROP);

        g(USER_CROP);
        g(USER_SWAP);
        cout << "=================\n";
        cout << "TIME_ACCUM: " << TimeAccumWrapper::time_accum * 1000 << "ms " << TimeAccumWrapper::time_accum * 100 / total_info.total_time << "% counter: " << TimeAccumWrapper::counter << "\n";
        cout.flush();

#undef CAT_IMPL
#undef CAT
#undef f
#undef g
    }

    cout << "============================================================\n";
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            cout << cnt_good_edges[i][j] << '/' << cnt_edges[i][j] << ' ';
        }
        cout << '\n';
    }
    cout << "============================================================\n";
    cout << endl;

    for (int K = 0; K <= 4; K++) {
        cout << "TEST CASE: K=" << K << " | " << infos[K] << endl;
    }
    cout << "TOTAL: " << total_info << endl;
}