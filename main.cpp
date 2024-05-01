#include "solution.cpp"

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

int main() {
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

    constexpr int test_case_K_sizes[] = {666, 215, 80, 39, 0};

    for (int K = 0; K <= 4; K++) {
        cout << "TEST CASE: K=" << K << endl;
        string dir = "tests/case_K=" + to_string(K) + "/";
        std::vector<pair<float,int>>tests_and_scores;
        infos[K].tests = test_case_K_sizes[K];
        for (int test = 0; test < test_case_K_sizes[K]; test++) {
            ifstream input(dir + to_string(test) + ".txt");
            TestData data;
            input >> data;

            Timer timer;

            auto intervals = Solver(data, test);

            double time = timer.get();
            infos[K].total_time += time;
            infos[K].max_test_time = max(infos[K].max_test_time, time);

            int score = get_solution_score(data, intervals);
            infos[K].total_score += score;
//            cout << score <<  " _ " <<  get_theory_max_score(data) << endl;
            int theor_max = get_theory_max_score(data);
            infos[K].total_theory_score += get_theory_max_score(data);
            tests_and_scores.push_back({(float)score/theor_max, test});
        }
        sort(tests_and_scores.begin(), tests_and_scores.end());
        for (int i = 0; i < min((int)tests_and_scores.size(), 10); i++){
            cout << tests_and_scores[i].first << " " << tests_and_scores[i].second << endl;
        }
    }

    test_case_info total_info;
    for (int K = 0; K <= 4; K++) {
        cout << "TEST CASE: K=" << K << " | " << infos[K] << endl;
        total_info.tests += infos[K].tests;
        total_info.total_score += infos[K].total_score;
        total_info.total_theory_score += infos[K].total_theory_score;
        total_info.total_time += infos[K].total_time;
        total_info.max_test_time = max(total_info.max_test_time, infos[K].max_test_time);
    }
    cout << "TOTAL: " << total_info << endl;
    cout << "ABADACEDABRA: " << ABADACEDABRA << endl;
}