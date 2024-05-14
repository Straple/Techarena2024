//#define VERIFY_ARTEM_ANS

#include "solution.cpp"
///
#include "coach.hpp"

extern Snapshoter snapshoter;

void testMyBitSet() {
    int N = 10;
    int K = 100000;
    Timer t;

    uint64_t hash = 0;
    /*for (int i = 0; i < 10; i++) {

        MyBitSet S;
        for (int j = 0; j < K; j++) {

            {
                int x = rnd.get(0, 127);
                if (S.contains(x)) {
                    S.erase(x);
                } else {
                    S.insert(x);
                }
            }
            for (int x: S) {
                hash += x;
            }
        }
    }*/


    for (int i = 0; i < 10; i++) {

        MyBitSet S;
        set<int> set;
        for (int j = 0; j < K; j++) {
            ASSERT(S.size() == set.size(), "invalid size");

            hash += S.size();
            hash *= 25;

            {
                int x = rnd.get(0, 127);

                ASSERT(S.contains(x) == set.count(x), "invalid contains");

                if (S.contains(x)) {
                    S.erase(x);
                    set.erase(x);
                    hash += 10;
                } else {
                    S.insert(x);
                    set.insert(x);
                    hash += 15124;
                }
                hash *= 31;
            }

            hash += S.size();

            vector<int> xS, xset;
            for (int x: S) {
                xS.push_back(x);
                hash += x;
                hash *= 124;
            }
            for (int x: set) {
                xset.push_back(x);
            }
            if (xS != xset) {
                for (int x: xS) {
                    cout << x << ' ';
                }
                cout << '\n';
                for (int x: xset) {
                    cout << x << ' ';
                }
                cout << '\n';
                cout << endl;
            }
            ASSERT(xS == xset, "failed range-based-for");
        }
    }

    cout << "TIME: " << t << endl;
    cout << "HASH: " << hash << endl;//4058641092

    //TIME: 0.4607s
    //HASH: 6520965741607998596
    //      6520965741607998596
}

int main() {

    //testMyBitSet();
    //return 0;
    /*SelectionRandomizer ss(10);
    map<int, int> S;
    for (int i = 0; i < 100000; i++) {
        int x = ss.select();
        S[x]++;
        cout << x << '\n';
    }
    for (auto [x, cnt]: S) {
        cout << x << ": " << cnt << endl;
    }
    return 0;*/

    //train_egor_task_solver();
    //return 0;

    for (int i = 0; i < 4; i++) {
        if (i == 0) {
            STEPS = 500;
        } else if (i == 1) {
            STEPS = 1'000;
        } else if (i == 2) {
            STEPS = 2'000;
        } else if (i == 3) {
            STEPS = 5'000;
        }

        test_case_info infos[5];
        map<int, map<int, int>> score_per_test;
        //ofstream scores_output("scores2.txt");

        // (diff, score, K, test, METRIC_TYPE)
        vector<tuple<int, int, int, int, int>> best_diff;
        for (int K = 0; K <= 3; K++) {
            //cout << "TEST CASE: K=" << K << endl;
            string dir = "tests/case_K=" + to_string(K) + "/";
            std::vector<pair<float, int>> tests_and_scores;
            infos[K].tests = test_case_K_sizes[K];
            for (int test = 0; test < test_case_K_sizes[K]; test++) {
                ifstream input(dir + to_string(test) + ".txt");
                TestData data;
                input >> data;

                //cout << "test: " << test << "!" << endl;

                int theor_max = get_theory_max_score(data);
                SNAP(snapshoter = Snapshoter(K, test, theor_max, data, "METRIC_DETECTION2"));

                Timer timer;
                vector<Interval> best_intervals;

                METRIC_TYPE = 0;
                auto zero_intervals = Solver(data);
                //cout << get_solution_score(data, zero_intervals) << ' ';
                best_intervals = zero_intervals;
                best_diff.emplace_back(0, get_solution_score(data, zero_intervals), K, test, 0);
                for (int m = 1; m < METRIC_CNT; m++) {
                    METRIC_TYPE = m;
                    auto intervals = Solver(data);
                    //cout << get_solution_score(data, intervals) << ' ';
                    if (get_solution_score(data, intervals) > get_solution_score(data, best_intervals)) {
                        best_intervals = intervals;
                        best_diff.back() = {
                                get_solution_score(data, intervals) - get_solution_score(data, zero_intervals),
                                get_solution_score(data, intervals), K, test, m};
                    }
                }
                //cout << endl;
                double time = timer.get();

                infos[K].total_time += time;
                infos[K].max_test_time = max(infos[K].max_test_time, time);

                int score = get_solution_score(data, best_intervals);
                score_per_test[K][test] = score;

                //scores_output << K << ' ' << test << ' ' << score << endl;

                infos[K].total_score += score;
                infos[K].total_theory_score += theor_max;

                //            std::ofstream out("ans_data_art/case_K="+to_string(K)+"/"+to_string(test)+".txt");
                //            out << intervals.size() << endl;
                //            for (int i = 0; i < intervals.size(); i++){
                //                out << intervals[i].start << " " << intervals[i].end << endl;
                //                out << intervals[i].users.size() << endl;
                //                for (auto user_id: intervals[i].users ){
                //                    out << user_id << " ";
                //                }
                //                out << endl;
                //            }
                //            out.close();

                infos[K].total_time += time;
                infos[K].max_test_time = max(infos[K].max_test_time, time);

                score_per_test[K][test] = score;

                //cout << score <<  " _ " <<  get_theory_max_score(data) << endl;
                //tests_and_scores.push_back({(float)score/theor_max, test});
                //cout << "###############" << endl;
            }
            sort(tests_and_scores.begin(), tests_and_scores.end());
            for (int i = 0; i < min((int) tests_and_scores.size(), 10); i++) {
                cout << tests_and_scores[i].first << " " << tests_and_scores[i].second << endl;
            }
        }

        /*vector<int> metric_add_score(METRIC_CNT);
        sort(best_diff.begin(), best_diff.end());
        for (auto [diff, score, K, test, m]: best_diff) {
            cout << diff << ' ' << score << ' ' << K << ' ' << test << ' ' << m << endl;
            metric_add_score[m] += diff;
        }
        cout << "add score: ";
        for (int m = 0; m < METRIC_CNT; m++) {
            cout << metric_add_score[m] << ' ';
        }
        cout << endl;*/

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

        cout << "====================================\n";
        cout << "STEPS: " << STEPS << '\n';
        for (int K = 0; K <= 4; K++) {
            cout << "TEST CASE: K=" << K << " | " << infos[K] << endl;
        }
        cout << "TOTAL: " << total_info << endl;
    }

    cout << ABSYBDAYSBD << endl;
}