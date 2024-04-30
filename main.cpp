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

int get_theory_max_score(const TestData &data) {
    int max_score = 0;
    for (auto user: data.userInfos) {
        max_score += user.rbNeed;
    }
    int max_possible = data.M * data.L;
    for (const auto &reserved: data.reservedRBs) {
        max_possible -= (reserved.end - reserved.start) * data.L;
    }

    return min(max_score, max_possible);
}

int main() {
    test_case_info infos[5];

    constexpr int test_case_K_sizes[] = {666, 215, 80, 39, 0};

    for (int K = 0; K <= 4; K++) {
        cout << "TEST CASE: K=" << K << endl;
        string dir = "tests/case_K=" + to_string(K) + "/";
        infos[K].tests = test_case_K_sizes[K];
        for (int test = 0; test < test_case_K_sizes[K]; test++) {
            ifstream input(dir + to_string(test) + ".txt");
            TestData data;
            input >> data;

            Timer timer;

            auto intervals = Solver(data);

            double time = timer.get();
            infos[K].total_time += time;
            infos[K].max_test_time = max(infos[K].max_test_time, time);

            int score = get_solution_score(data, intervals);
            infos[K].total_score += score;
            infos[K].total_theory_score += get_theory_max_score(data);
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
}