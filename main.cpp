#include <iostream>

///////// !!!
#define MY_DEBUG_MODE
///////// !!!

#ifdef MY_DEBUG_MODE

#define FAILED_ASSERT(message)                                                \
    {                                                                         \
        std::cerr << "assert failed at " __FILE__ << ":" << __LINE__ << '\n'; \
        std::cerr << "message: \"" << (message) << "\"\n";                    \
        std::exit(0);                                                         \
    }

#define ASSERT(condition, message) \
    if (!(condition))              \
    FAILED_ASSERT(message)

//_STL_VERIFY(condition, message) // don't work on CLion

#else

#define ASSERT(condition, message) condition /**THIS CONDITION VERY IMPORTANT */

#endif// MY_DEBUG_MODE

#pragma once

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

struct free_space{
    int start, end, len;
};

vector<Interval> Solver_artem(int N, int M, int K, int J, int L,
                        vector<Interval> reservedRBs,
                        vector<UserInfo> userInfos
){
    std::vector<free_space>free_spaces;
    {
        std::vector<bool>is_free(M+1, true);
        is_free.back() = false;
        int start = -1;
        for (size_t i = 0; i < reservedRBs.size(); i++){
            for (size_t g = reservedRBs[i].start; g < reservedRBs[i].end; g++){
                is_free[g] = false;
            }
        }

        for (int i = 0; i < is_free.size(); i++) {
            if (!is_free[i]) {
                if (start != i - 1) {
                    free_spaces.push_back({start + 1, i, i - start + 1});
                }
                start = i;
            }
        }

    }


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
    auto result = Solver_artem(testdata.N, testdata.M, testdata.K, testdata.J, testdata.L, testdata.reservedRBs,
                         testdata.userInfos);

    for (int i = 0; i < result.size(); i++) {
        for (int j = i + 1; j < result.size(); j++) {
            ASSERT(result[i].end <= result[j].start || result[j].end <= result[i].start,
                   "answer interval is invalid: intersect");
        }
    }

    vector<int> user_score(testdata.N);
    vector<int> user_min(testdata.N, 1e9);
    vector<int> user_max(testdata.N, -1e9);
    for (auto [start, end, users]: result) {
        // validate interval
        {
            ASSERT(users.size() <= testdata.L, "answer interval is invalid: users more than L");
            ASSERT(start < end, "answer interval is invalid: start >= end");
            for (auto [reserved_start, reserved_end, reserved_users]: testdata.reservedRBs) {
                ASSERT(end <= reserved_start || reserved_end <= start, "answer interval intersect with reservedRbs");
            }
        }
        set<int> unique_beams;
        for (int id: users) {
            user_score[id] += end - start;

            user_min[id] = min(user_min[id], start);
            user_max[id] = max(user_max[id], end);
            unique_beams.insert(testdata.userInfos[id].beam);
        }

        ASSERT(unique_beams.size() == users.size(), "answer interval is invalid: have equal user beams");
    }

    int sum_score = 0;
    for (int u = 0; u < testdata.N; u++) {
        sum_score += std::min(user_score[u], testdata.userInfos[u].rbNeed);

        ASSERT(user_score[u] == user_max[u] - user_min[u],
               "answer interval is invalid: user have no continuous interval");
    }
    return sum_score;
}

int main() {
    std::fstream input("open.txt");
    size_t test_cases;
    input >> test_cases;

    for (size_t test_case = 0; test_case < test_cases; test_case++) {
        auto data = read_test(input);
        auto res = get_solution_score(data);
        cout << "TEST: " << test_case << " | RES: " << res << endl;
    }

    return 0;
}
