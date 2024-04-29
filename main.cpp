#include "solution.cpp"

int main() {
    std::fstream input("open.txt");
    size_t test_cases;
    input >> test_cases;
    int total_score = 0;
    int total_max = 0;
    std::vector<pair<float,int>>lowest_completeness;
    int K_ZERO = 0;
    for (size_t test_case = 0; test_case < test_cases; test_case++) {
        auto data = read_test(input);
        auto intervals = Solver(data);
        auto res = get_solution_score(data, intervals);
        int max_score = 0;
        for (auto user: data.userInfos) {
            max_score += user.rbNeed;
        }
        int max_possible = data.M * data.L;
        for (auto reserved: data.reservedRBs){
            max_possible -= (reserved.end-reserved.start)*data.L;
        }
        std::vector<int>mx_scores;
        for (auto user: data.userInfos){
            mx_scores.push_back(user.rbNeed);
        }
        sort(mx_scores.begin(),mx_scores.end(), greater<>());
        int mx_sum = 0;
        for (int i = 0; i < min(data.J*data.L, (int)mx_scores.size()); i++){
            mx_sum+=mx_scores[i];
        }

        int realMAXpossible = min(min(max_possible,max_score),mx_sum);
        K_ZERO+=data.K==0;
        total_max += realMAXpossible;
        total_score += res;
        cout << "TEST: " << test_case << " | RES: " << res << "/" << realMAXpossible << " | " << float(res) / realMAXpossible * 100
             << "%" << endl;
        lowest_completeness.push_back({float(res) / realMAXpossible * 100, test_case});
    }
    cout << "------------" << endl;
    cout << "RES: " << total_score << "/" << total_max << " | " << float(total_score) / total_max * 100 << "%" << endl;
//    sort(lowest_completeness.begin(), lowest_completeness.end());
//    for (int i = 0; i < min((int)test_cases, 30); i++){
//        cout << lowest_completeness[i].first  << "% test: " << lowest_completeness[i].second << endl;
//    }
    return 0;
}