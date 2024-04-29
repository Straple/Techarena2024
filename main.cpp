#include "solution.cpp"

int main() {
    std::fstream input("open.txt");
    size_t test_cases;
    input >> test_cases;
    int total_score = 0;
    int total_max = 0;
    for (size_t test_case = 0; test_case < test_cases; test_case++) {
        auto data = read_test(input);
        auto intervals = Solver(data);
        auto res = get_solution_score(data, intervals);
        int max_score = 0;
        for (auto user: data.userInfos) {
            max_score += user.rbNeed;
        }
        total_score += res;
        total_max += max_score;
        cout << "TEST: " << test_case << " | RES: " << res << "/" << max_score << " | " << float(res) / max_score * 100
             << "%" << endl;
    }
    cout << "------------" << endl;
    cout << "RES: " << total_score << "/" << total_max << " | " << float(total_score) / total_max * 100 << "%" << endl;

    return 0;
}