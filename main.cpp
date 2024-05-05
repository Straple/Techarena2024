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
        SELECTION_ACTION.reset_rnd();
        /*for (int i = 0; i < SELECTION_ACTION.size(); i++) {
            SELECTION_ACTION[i].reset_rnd();
        }*/
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
    STEPS = 300;
    ans.push_back(do_test());
    STEPS = 400;
    ans.push_back(do_test());
    //STEPS = 600;
    //ans.push_back(do_test());
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
        logger << "SELECTION_ACTION:\n";
        logger << SELECTION_ACTION << '\n';
        /*for (int i = 0; i < 12; i++) {
            logger << i << ": " << SELECTION_ACTION[i] << '\n';
        }*/
        logger << endl;
        logger.flush();
    };

    //auto super_puper_ans = ans;
    log();

    for (int step = 0;; step++) {
        cout << step << ' ' << global_time << endl;

        bool ok = false;
        /*for (int x = 0; x < 100; x++) {
            int i = rnd.get(0, 11);
            int j = rnd.get(0, 11);
            int change = rnd.get(max(-SELECTION_ACTION[i].kit[j].second, -2), 2);
            if (change == 0) {
                change = 1;
            }
            SELECTION_ACTION[i].kit[j].second += change;

            auto new_ans = calc_train_score();
            if (compare(ans, new_ans)) {
                ok = true;
                ans = new_ans;
                log();
            } else {
                SELECTION_ACTION[i].kit[j].second -= change;
            }
        }*/
        //for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            for (int change = max(-SELECTION_ACTION.kit[j].second, -1); change <= 2; change++) {
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
        //}

        if (!ok) {
            cout << "OH NO, IT'S BAD" << endl;
            int j = rnd.get(0, 11);
            SELECTION_ACTION.kit[j].second++;
        }

        /*auto brute = [&](SelectionRandomizer &selection) {
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
        }*/
    }
}

int main() {
    /*vector<vector<int>> powers = {
            {57, 68, 61, 1, 3, 674, 4, 19, 5, 20, 13, 8},
            {18, 7, 23, 1, 1, 833, 0, 16, 7, 20, 8, 4},
            {16, 21, 6, 2, 1, 833, 0, 20, 5, 26, 11, 5},
            {185, 6, 263, 0, 0, 245, 0, 0, 0, 0, 0, 0},
            {174, 218, 8, 0, 0, 274, 0, 0, 0, 0, 0, 0},
            {179, 178, 159, 28, 28, 183, 36, 89, 63, 96, 66, 58},
            {10, 10, 15, 0, 0, 798, 0, 0, 0, 0, 0, 0},
            {15, 12, 14, 0, 0, 946, 0, 0, 0, 4, 1, 0},
            {2, 3, 7, 0, 0, 931, 0, 0, 0, 0, 0, 0},
            {15, 16, 26, 0, 0, 932, 0, 0, 0, 0, 0, 0},
            {5, 8, 12, 0, 0, 999, 0, 0, 0, 0, 0, 0},
            {4, 3, 5, 0, 0, 983, 0, 0, 0, 0, 0, 0},
    };
    for (int i = 0; i < 12; i++) {
        SELECTION_ACTION[i] = powers[i];
    }*/
    /*SELECTION_ACTION[0] = vector<int>{9, 11, 11, 4, 4, 12, 5, 6, 5, 6, 6, 5};
    SELECTION_ACTION[1] = vector<int>{9, 7, 9, 5, 6, 13, 4, 7, 7, 7, 7, 7};
    SELECTION_ACTION[2] = vector<int>{9, 9, 7, 6, 5, 14, 4, 8, 7, 8, 7, 7};
    SELECTION_ACTION[3] = vector<int>{6, 6, 8, 0, 0, 6, 0, 0, 0, 0, 0, 0};
    SELECTION_ACTION[4] = vector<int>{6, 8, 6, 0, 0, 7, 0, 0, 0, 0, 0, 0};
    SELECTION_ACTION[5] = vector<int>{12, 13, 13, 7, 7, 11, 7, 9, 8, 9, 9, 8};
    SELECTION_ACTION[6] = vector<int>{5, 6, 7, 0, 0, 7, 0, 0, 0, 0, 0, 0};
    SELECTION_ACTION[7] = vector<int>{6, 7, 7, 0, 0, 9, 0, 1, 0, 1, 1, 0};
    SELECTION_ACTION[8] = vector<int>{5, 7, 7, 0, 0, 8, 0, 0, 0, 0, 0, 0};
    SELECTION_ACTION[9] = vector<int>{6, 7, 7, 0, 0, 9, 0, 1, 0, 1, 1, 0};
    SELECTION_ACTION[10] = vector<int>{5, 7, 7, 0, 0, 9, 0, 1, 0, 1, 1, 0};
    SELECTION_ACTION[11] = vector<int>{5, 7, 7, 0, 0, 8, 0, 0, 0, 0, 0, 0};*/

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
    ofstream scores_output("scores2.txt");
    for (int K = 0; K <= 3; K++) {
        cout << "TEST CASE: K=" << K << endl;
        string dir = "tests/case_K=" + to_string(K) + "/";
        std::vector<pair<float, int>> tests_and_scores;
        infos[K].tests = test_case_K_sizes[K];
        for (int test = 0; test < test_case_K_sizes[K]; test++) {
            ifstream input(dir + to_string(test) + ".txt");
            TestData data;
            input >> data;


            cout << "test: " << test << "!" << endl;

            Timer timer;
            auto intervals = Solver(data);
            double time = timer.get();


            infos[K].total_time += time;
            infos[K].max_test_time = max(infos[K].max_test_time, time);

            int score = get_solution_score(data, intervals);
            score_per_test[K][test] = score;

            scores_output << K << ' ' << test << ' ' << score << endl;

            infos[K].total_score += score;
            int theor_max = get_theory_max_score(data);
            infos[K].total_theory_score += theor_max;

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

    // 11 13 13 5 5 14 6 7 6 7 7 6
    //11 9 11 7 7 16 5 9 8 9 8 8
    //11 10 8 7 7 17 5 9 8 9 9 8
    //7 8 10 0 0 8 0 0 0 0 0 0
    //7 10 8 0 0 8 0 0 0 0 0 0
    //14 16 16 9 8 13 9 11 10 11 10 10
    //6 8 8 0 0 9 0 0 0 0 0 0
    //7 9 9 0 0 11 0 1 1 2 2 1
    //6 8 8 0 0 10 0 1 0 1 1 0
    //7 9 9 0 0 11 0 2 1 2 2 1
    //6 9 9 0 0 11 0 2 0 2 1 1
    //7 8 8 0 0 10 0 1 0 0 0 0
    cout << "============================================================\n";
    cout << "{\n";
    for (int i = 0; i < 12; i++) {
        cout << "{";
        for (int j = 0; j < 12; j++) {
            long long sum = 0;
            for (int k = 0; k < 12; k++) {
                sum += cnt_edges[i][k];
            }
            int val = (int) (cnt_good_edges[i][j] * 10000.0 / sum);
            cout << val;
            if (j != 11) {
                cout << ", ";
            }
            //cout << cnt_good_edges[i][j] << '/' << cnt_edges[i][j] << ' ';
            //cout << (int) (cnt_good_edges[i][j] * 100.0 / cnt_edges[i][j]) << ' ';
        }
        cout << "},\n";
    }
    cout << "}\n";
    cout << "============================================================\n";
    cout << endl;

    for (int K = 0; K <= 4; K++) {
        cout << "TEST CASE: K=" << K << " | " << infos[K] << endl;
    }
    cout << "TOTAL: " << total_info << endl;
}

// 2803/43805 3860/43557 3893/43361 0/43553 0/43528 4663/44217 0/43801 484/43736 6/43767 492/43845 444/43882 0/43652
//1676/43501 479/43262 1586/43080 0/43269 0/43232 4640/43948 0/43505 424/43444 6/43450 470/43592 173/43608 0/43365
//1713/43297 1442/43071 494/42892 0/43068 0/43046 4699/43738 0/43319 399/43256 6/43268 454/43394 215/43422 1/43190
//4108/43503 5866/43266 5815/43081 0/43274 0/43234 4303/43948 0/43513 490/43446 5/43452 473/43593 752/43610 0/43369
//4130/43465 5929/43227 6015/43051 0/43239 0/43199 4431/43922 0/43480 488/43411 0/43424 475/43557 649/43580 1/43338
//2939/44173 3877/43957 3925/43727 0/43957 0/43900 4460/44569 0/44186 489/44139 3/44173 466/44217 487/44234 1/44021
//3180/43749 5250/43520 5173/43320 0/43504 0/43488 4722/44168 0/43744 475/43690 3/43707 463/43804 493/43831 0/43602
//2963/43683 3771/43450 3783/43263 0/43450 0/43428 4556/44123 0/43682 385/43623 11/43640 457/43756 449/43761 2/43537
//2906/43699 3846/43471 3807/43287 0/43468 0/43446 4541/44139 0/43707 495/43639 8/43660 508/43771 486/43783 0/43557
//2967/43820 3612/43572 3769/43379 0/43571 0/43549 4726/44235 0/43818 447/43753 9/43782 466/43869 447/43901 0/43672
//2462/43844 3655/43597 3663/43398 0/43599 0/43565 4659/44252 0/43836 447/43771 5/43808 505/43896 341/43917 0/43689
//3124/43594 3781/43353 3838/43181 0/43382 0/43334 4465/44045 0/43595 473/43545 3/43549 434/43676 432/43683 1/43464