#pragma once

#include "assert.hpp"
#include <algorithm>
#include <map>
#include <set>
#include <vector>
using namespace std;

struct UserInfo {
    int rbNeed;
    int beam;
    int id;
};

struct Interval {
    int start, end;
    vector<int> users;
};

int length(const Interval &inter) {
    return inter.end - inter.start;
}

struct MyInterval {
    int start, end;

    int len() const {
        return end - start;
    }
};

struct TestData {
    int N, M, K, J, L;
    vector<Interval> reservedRBs;
    vector<UserInfo> userInfos;
};
std::vector<Interval> unblock_ans(const std::vector<std::vector<Interval>> &ans) {
    vector<Interval> answer;
    for (int i = 0; i < ans.size(); i++) {
        for (int g = 0; g < ans[i].size(); g++) {
            if (ans[i][g].users.size()) {
                answer.push_back(ans[i][g]);
            }
        }
    }
    return answer;
}

int get_theory_max_score(int N, int M, int K, int J, int L, const vector<Interval> &reservedRBs,
                         const vector<UserInfo> &userInfos) {
    int max_score = 0;
    std::vector<pair<int, int>> rbNeeded;
    std::vector<pair<int, int>> reserved;
    for (int i = 0; i < K; i++) {
        reserved.push_back({reservedRBs[i].start, reservedRBs[i].end});
    }
    sort(reserved.begin(), reserved.end());
    int max_res_len = M;
    if (K) {
        max_res_len = max(reserved[0].first, M - reserved.back().second);
    }
    for (int i = 1; i < reserved.size(); i++) {
        max_res_len = max(max_res_len, reserved[i].first - reserved[i - 1].second);
    }
    for (auto user: userInfos) {
        rbNeeded.push_back({min(user.rbNeed, max_res_len), user.beam});
    }
    sort(rbNeeded.begin(), rbNeeded.end(), greater<>());
    int taken = 0;
    map<int, int> beams_taken;
    map<int, int> len_per_beam;
    std::vector<vector<int>>rbNeedPerBeam(32, vector<int>());
    for (int i = 0; i < N; i++) {
        rbNeedPerBeam[userInfos[i].beam].push_back(userInfos[i].rbNeed);
    }

    for (int i = 0; i < 32; i++) {
        sort(rbNeedPerBeam[i].begin(), rbNeedPerBeam[i].end(), greater<>());
        for (int g = 0; g < rbNeedPerBeam[i].size(); g++){
            if (g == J){
                break;
            }
            len_per_beam[i]+=rbNeedPerBeam[i][g];
        }
    }
    //    for (int i = 0; i < N; i++) {
    //        len_per_beam[userInfos[i].beam] += userInfos[i].rbNeed;
    //    }
    int realL = 0;
    std::vector<bool>was(32, false);
    for (auto& u: userInfos){
        was[u.beam] = true;
    }
    for (int i = 0; i < 32; i++) realL+=was[i];
    L = min(L, realL);
    int ma_len_uniq_beam = 0;
    int sum_res_len = 0;
    for (const auto &reserved: reservedRBs) {
        sum_res_len += reserved.end - reserved.start;
    }

    for (auto [beam, len]: len_per_beam) {
        ma_len_uniq_beam += min(len, M - sum_res_len);
    }

    for (int i = 0; i < N; i++) {
        beams_taken[rbNeeded[i].second]++;
        if (beams_taken[rbNeeded[i].second] == (J - 1) * L) {
            continue;
        }
        //1033704
        //1033702
        max_score += rbNeeded[i].first;
        taken++;
        if (taken == J * L) {
            break;
        }
    }

    int max_possible = M * L;
    for (const auto &reserved: reservedRBs) {
        max_possible -= (reserved.end - reserved.start) * L;
    }
    //    cout << max_score << " " << max_possible << " " << ma_len_uniq_beam << endl;
    return min(min(max_score, max_possible), ma_len_uniq_beam);
}

int get_theory_max_score(const TestData &data) {
    return get_theory_max_score(data.N, data.M, data.K, data.J, data.L, data.reservedRBs, data.userInfos);
}

istream &operator>>(istream &input, TestData &data) {
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

    data = TestData{N, M, K, J, L, reservedRBs, userInfos};
    return input;
}

ostream &operator<<(ostream &output, const TestData &data) {
    output << data.N << ' ' << data.M << ' ' << data.K << ' ' << data.J << ' ' << data.L << '\n';
    for (auto &[start, end, users]: data.reservedRBs) {
        output << start << ' ' << end << '\n';
    }
    for (int u = 0; u < data.N; u++) {
        output << data.userInfos[u].rbNeed << ' ' << data.userInfos[u].beam << '\n';
    }
    return output;
}

int get_solution_score(int N, int M, int K, int J, int L,
                       const vector<Interval> &reservedRBs,
                       const vector<UserInfo> &userInfos, const vector<Interval> &answer) {
    ASSERT(answer.size() <= J, "answer intervals is invalid: count intervals more than J");

    for (int i = 0; i < answer.size(); i++) {
        for (int j = i + 1; j < answer.size(); j++) {
            ASSERT(answer[i].end <= answer[j].start || answer[j].end <= answer[i].start,
                   "answer interval is invalid: intersect");
        }
    }

    vector<int> user_score(N);
    vector<int> user_min(N, 1e9);
    vector<int> user_max(N, -1e9);
    for (auto [start, end, users]: answer) {
        // validate interval
        {
            ASSERT(!users.empty(), "users is empty");
            ASSERT(users.size() <= L, "answer interval is invalid: users more than L");
            ASSERT(start < end, "answer interval is invalid: start >= end");
            ASSERT(0 <= start && end <= M, "answer interval is invalid: incorrect interval");
            for (auto [reserved_start, reserved_end, reserved_users]: reservedRBs) {
                ASSERT(end <= reserved_start || reserved_end <= start, "answer interval intersect with reservedRbs");
            }
        }
        set<int> unique_beams;
        for (int id: users) {
            user_score[id] += end - start;

            user_min[id] = min(user_min[id], start);
            user_max[id] = max(user_max[id], end);
            unique_beams.insert(userInfos[id].beam);
        }

        ASSERT(unique_beams.size() == users.size(), "answer interval is invalid: have equal user beams");
    }

    int sum_score = 0;
    for (int u = 0; u < N; u++) {
        sum_score += std::min(user_score[u], userInfos[u].rbNeed);

        ASSERT(user_min[u] == 1e9 || user_score[u] == user_max[u] - user_min[u],
               "answer interval is invalid: user have no continuous interval");
    }
    return sum_score;
}

int get_solution_score(const TestData &testdata, const vector<Interval> &answer) {
    return get_solution_score(testdata.N, testdata.M, testdata.K, testdata.J, testdata.L, testdata.reservedRBs,
                              testdata.userInfos, answer);
}

std::vector<vector<Interval>> ans_to_blocked_ans(int M, int K, const vector<Interval> &reservedRBs, vector<Interval> &solution) {
    std::vector<bool> is_free(M + 1, true);
    is_free.back() = false;
    int start = -1;
    for (size_t i = 0; i < reservedRBs.size(); i++) {
        for (size_t g = reservedRBs[i].start; g < reservedRBs[i].end; g++) {
            is_free[g] = false;
        }
    }

    std::vector<pair<int, int>> free_intervals;
    for (int i = 0; i < is_free.size(); i++) {
        if (!is_free[i]) {
            if (start != i - 1) {
                free_intervals.push_back({start + 1, i});
            }
            start = i;
        }
    }
    std::vector<std::vector<Interval>> ans(free_intervals.size());
    for (auto interval: solution) {
        int i = 0;
        for (auto [l, r]: free_intervals) {
            if (interval.start >= l && interval.end <= r) {
                ans[i].push_back(interval);
                break;
            }
            i++;
        }
    }
    return ans;
}

std::map<int, int> reduce_users(int &N, int J, vector<UserInfo> &userInfos) {
    std::map<int, int> back_mapping;
    std::vector<UserInfo> newUserInfos;
    int last_id = 0;
    std::vector<pair<int, pair<int, int>>> users_arr;
    std::vector<int> beamGot(32);
    for (int i = 0; i < N; i++) {
        users_arr.push_back({userInfos[i].rbNeed, {userInfos[i].beam, i}});
    }
    sort(users_arr.begin(), users_arr.end(), greater<>());
    for (int i = 0; i < N; i++) {
        beamGot[users_arr[i].second.first]++;
        if (beamGot[users_arr[i].second.first] <= J) {
            newUserInfos.push_back({users_arr[i].first, users_arr[i].second.first, last_id});
            back_mapping[last_id] = users_arr[i].second.second;
            last_id++;
        }
    }
    N = last_id;
    userInfos = newUserInfos;
    return back_mapping;
}
void normalize_id(std::vector<Interval> &normalize_it, std::map<int, int> &back_mapping) {
    for (int i = 0; i < normalize_it.size(); i++) {
        for (int g = 0; g < normalize_it[i].users.size(); g++) {
            normalize_it[i].users[g] = back_mapping[normalize_it[i].users[g]];
        }
    }
}

int get_time_ms() {
    return (clock() * 1000) / CLOCKS_PER_SEC;
}