#include "bits/stdc++.h"

using namespace std;

struct UserInfo {
    int rbNeed;
    int beam;
    int id;
};

struct Interval {
    int start, end;
    std::vector<int> users;
};

vector<Interval> Solver(int N, int M, int K, int J, int L,
                        vector<Interval> reservedRBs,
                        vector<UserInfo> userInfos
) {

}

struct TestData {
    int N, M, K, J, L;
    vector<Interval> reservedRBs;
    vector<UserInfo> userInfos;
};

TestData read_test(istream &input) {
    int N, M, K, J, L;
    input >> N >> M >> K >> J >> L;

    vector<Interval> reservedRBs(K);
    for (auto &[start, end, users]: reservedRBs) {
        input >> start >> end;
    }

    vector<UserInfo> userInfos(N);
    for (int u = 0; u < N; u++) {
        userInfos[u].id = u;
        input >> userInfos[u].rbNeed >> userInfos[u].beam;
    }

    return TestData{N, M, K, J, L, reservedRBs, userInfos};
}

double get_solution_score(const TestData &testdata) {
    auto result = Solver(testdata.N, testdata.M, testdata.K, testdata.J, testdata.L, testdata.reservedRBs,
                         testdata.userInfos);

    vector<int> user_score(testdata.N);
    for (auto [start, end, users]: result) {
        for (int id: users) {
            user_score[id] += end - start;
        }
    }

    int sum_score = 0;
    for (int u = 0; u < testdata.N; u++) {
        sum_score += std::min(user_score[u], testdata.userInfos[u].rbNeed);
    }
    return sum_score;
}

int main() {


    return 0;
}
