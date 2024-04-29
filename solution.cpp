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

// ===================================================================================================================
// =========================RANDOMIZER================================================================================
// ===================================================================================================================

#include <random>

class randomizer {
    std::mt19937_64 generator;

public:
    randomizer();

    explicit randomizer(uint64_t seed);

    // uint64_t
    uint64_t get();

    // int64_t [left, right]
    int64_t get(int64_t left, int64_t right);

    // double [0, 1]
    double get_d();

    // double [left, right]
    double get_d(double left, double right);

    double get_exp();
};

randomizer::randomizer() : generator(42) {
}

randomizer::randomizer(uint64_t seed) : generator(seed) {
}

uint64_t randomizer::get() {
    std::uniform_int_distribution<uint64_t> distrib;
    return distrib(generator);
}

int64_t randomizer::get(int64_t left, int64_t right) {
    ASSERT(left <= right, "invalid segment");
    std::uniform_int_distribution<int64_t> distrib(left, right);
    return distrib(generator);
}

double randomizer::get_d() {
    double p = static_cast<double>(get()) / static_cast<double>(UINT64_MAX);
    ASSERT(0 <= p && p <= 1, "invalid result");
    return p;
}

double randomizer::get_d(double left, double right) {
    ASSERT(left <= right, "invalid segment");
    double p = (get_d() * (right - left)) + left;
    ASSERT(left <= p && p <= right, "invalid result");
    return p;
}

double randomizer::get_exp() {
    return exp(get(-20, 20)) * get_d(-1, 1);
}

// ===================================================================================================================
// =========================SOLUTION==================================================================================
// ===================================================================================================================

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

struct MyInterval {
    int start, end;
    int len(){
        return end-start;
    }
};
int BEAM_MAX_AMOUNT = 32;
int get_blocks_amount(int M, vector<Interval> reservedRBs){
    int cnt = 0;
    std::vector<bool> is_free(M + 1, true);
    is_free.back() = false;
    int start = -1;
    for (size_t i = 0; i < reservedRBs.size(); i++) {
        for (size_t g = reservedRBs[i].start; g < reservedRBs[i].end; g++) {
            is_free[g] = false;
        }
    }

    for (int i = 0; i < is_free.size(); i++) {
        if (!is_free[i]) {
            if (start != i - 1) {
                cnt++;
            }
            start = i;
        }
    }
    return cnt;
}
vector<vector<Interval>> Solver_artem(int N, int M, int K, int J, int L,
                              vector<Interval>& reservedRBs,
                              vector<UserInfo>& userInfos, std::vector<std::set<int>>& allowed_users_per_block
) {
    std::vector<MyInterval> free_spaces;
    {
        std::vector<bool> is_free(M + 1, true);
        is_free.back() = false;
        int start = -1;
        for (size_t i = 0; i < reservedRBs.size(); i++) {
            for (size_t g = reservedRBs[i].start; g < reservedRBs[i].end; g++) {
                is_free[g] = false;
            }
        }

        for (int i = 0; i < is_free.size(); i++) {
            if (!is_free[i]) {
                if (start != i - 1) {
                    free_spaces.push_back({start + 1, i});
                }
                start = i;
            }
        }
    }
    sort(free_spaces.begin(), free_spaces.end(), [&](const MyInterval& lhs, const MyInterval& rhs){
        if (lhs.end-lhs.start == rhs.end-rhs.start){
            return lhs.start < rhs.start;
        } else {
            return lhs.end-lhs.start > rhs.end-rhs.start;
        }
    });

    vector<std::vector<Interval>> pre_answer(free_spaces.size());
    {
        int total_free_space_size = 0;
        for (auto free_space: free_spaces) {
            total_free_space_size += free_space.len();
        }
        int TARGET_LEN = total_free_space_size / J;
        std::vector<int>free_spaces_seperation_starts;
        for (auto free_space: free_spaces) {
            free_spaces_seperation_starts.push_back(free_space.start);
        }
        for (auto j = 0; j < J; j++){
            // выбрать наибольший возможный, отрезать от него кусок TARGET_LEN;
            int selected_index = -1;
            int selected_size = -1;
            for (int i = 0; i < free_spaces.size(); i++){
                int current_possible_len = free_spaces[i].end-free_spaces_seperation_starts[i];
                if (current_possible_len > selected_size){
                    selected_size = current_possible_len;
                    selected_index = i;
                }
            }
            assert(selected_index != -1);
            int can_cut = selected_size; // если это не последний отрезаемый кусок. Тогда отрезаем всё что есть
            if (j+1 != J){
                can_cut = std::min(selected_size, TARGET_LEN);
            }
            int currect_start = free_spaces_seperation_starts[selected_index];
            pre_answer[selected_index].push_back({currect_start, currect_start+can_cut, {}});
            free_spaces_seperation_starts[selected_index]+=can_cut;

        }
    }
    std::vector<int>rbSuplied(N,0);
    std::set<int> used_users;
    int current_interval_iter = 0;
    std::set<pair<int,int>, std::greater<>>space_left_q;
    for (int i = 0; i < pre_answer.size(); i++){
        int total_size = 0;
        for (int g = 0; g < pre_answer[i].size(); g++){
            total_size += pre_answer[i][g].end - pre_answer[i][g].start;
        }
        if (total_size != 0) {
            space_left_q.insert({total_size, i});
        }
    }
    std::vector<std::vector<int>>beamOwnedBy(pre_answer.size(), std::vector<int>(BEAM_MAX_AMOUNT, -1));
    std::vector<std::set<int>>activeUsers(pre_answer.size());
    std::vector<int>current_sub_interval(pre_answer.size(), 0);
    while (!space_left_q.empty()){
        int space_left = space_left_q.begin()->first;
        int pick_i = space_left_q.begin()->second;
        space_left_q.erase(space_left_q.begin());
        // юзаем OWNEDBY до конца!!!
        // считаем кол-во свободных слотов.

        // Набираем
        std::vector<std::pair<int,int>>candidates;
        for (auto& user: userInfos){
            if (used_users.find(user.id) == used_users.end() && beamOwnedBy[pick_i][user.beam] == -1 && allowed_users_per_block[pick_i].count(user.id)){
                assert(rbSuplied[user.id] == 0);
                candidates.push_back({user.rbNeed-rbSuplied[user.id],user.id});
            }
        }
        sort(candidates.begin(),candidates.end(), greater<>());
        int get_more = L - activeUsers[pick_i].size();
        for (int g = 0; g < (int)candidates.size(); g++){
            if (get_more == 0) break;
            if (beamOwnedBy[pick_i][userInfos[candidates[g].second].beam] == -1){
                activeUsers[pick_i].insert(candidates[g].second);
                used_users.insert(candidates[g].second);
                beamOwnedBy[pick_i][userInfos[candidates[g].second].beam] = candidates[g].second;
                get_more--;
            }
        }

        std::set<int>to_delete;
        for (auto user_id: activeUsers[pick_i]){
            rbSuplied[user_id] += pre_answer[pick_i][current_sub_interval[pick_i]].end-pre_answer[pick_i][current_sub_interval[pick_i]].start;
            pre_answer[pick_i][current_sub_interval[pick_i]].users.push_back(user_id);
            if (rbSuplied[user_id] > userInfos[user_id].rbNeed){
                to_delete.insert(user_id);
            }
        }
        for (auto user_id: to_delete){
            activeUsers[pick_i].erase(user_id);
            beamOwnedBy[pick_i][userInfos[user_id].beam] = -1;
        }
        space_left -= pre_answer[pick_i][current_sub_interval[pick_i]].end-pre_answer[pick_i][current_sub_interval[pick_i]].start;
        current_sub_interval[pick_i]++;
        if (space_left != 0){
            space_left_q.insert({space_left, pick_i});
        }
    }
    return pre_answer;
}

int get_solution_score_light(int N, vector<vector<Interval>>& ans, vector<UserInfo>& userInfos, std::vector<int>&suplied){
    for (int i = 0; i < N; i++){
        suplied[i] = 0;
    }
    for (int i = 0; i < ans.size(); i++){
        for (int g = 0; g < ans[i].size(); g++){
            for (auto user_id: ans[i][g].users){
                suplied[user_id]+=ans[i][g].end-ans[i][g].start;
            }
        }
    }
    int score = 0;
    for (int i = 0; i < N; i++){
        score += std::min(userInfos[i].rbNeed, suplied[i]);
    }

    return score;
}

vector<Interval> Solver_Artem_grad(int N, int M, int K, int J, int L,
                             vector<Interval> reservedRBs,
                             vector<UserInfo> userInfos) {

    int blocks_amount = get_blocks_amount(M, reservedRBs);
    std::vector<std::set<int>> pre_allowed(blocks_amount);
    for (int i = 0; i < N; i++) {
        for (int g = 0; g < blocks_amount; g++) {
            pre_allowed[g].insert(i);
        }
    }
    auto pre_answer = Solver_artem(N, M, K, J, L, reservedRBs, userInfos, pre_allowed);
    std::vector<std::set<int>> allowed(blocks_amount);
    std::map<int,int>user_to_interval;
    for (int i = 0; i < pre_answer.size(); i++) {
        for (int g = 0; g < pre_answer[i].size(); g++){
            for (auto user_id: pre_answer[i][g].users) {
                allowed[i].insert(user_id);
                user_to_interval[user_id] = i;
            }
        }
    }
//
//
//
//
//    auto f = [&](std::vector<std::set<int>>& allowed, std::vector<int>&suplied) {
//        auto ans = Solver_artem(N, M, K, J, L, reservedRBs, userInfos, allowed);
//        int score = get_solution_score_light(N, ans, userInfos, suplied);
//        return score;
//    };
//
//    vector<int>suplied(N, 0);
//    int iters = 500;
//    int current_score = f(allowed,suplied);
//    randomizer rnd;
//    while (iters--){
//        int a,b;
//        int preIter = 1000;
//        while (preIter--) {
//            a = rnd.get(0, N - 1);
//            b = rnd.get(0, N - 1);
//            if (a == b) continue;
//            int cnt = 0;
//            if (userInfos[a].rbNeed > suplied[a]) {
//                cnt++;
//            }
//            if (userInfos[b].rbNeed > suplied[b]) {
//                cnt++;
//            }
//            if (cnt == 1) break;
//        }
//        if (preIter == 0) break;
//        int a_block = user_to_interval[a];
//        int b_block = user_to_interval[b];
//        allowed[a_block].erase(a);
//        allowed[b_block].erase(b);
//
//        allowed[a_block].insert(b);
//        allowed[b_block].insert(a);
//        user_to_interval[a] = b_block;
//        user_to_interval[b] = a_block;
//
//        int new_score = f(allowed, suplied);
//        if (new_score > current_score){
//            current_score = new_score;
//        } else {
//
//            allowed[a_block].erase(b);
//            allowed[b_block].erase(a);
//
//            allowed[a_block].insert(a);
//            allowed[b_block].insert(b);
//            user_to_interval[a] = a_block;
//            user_to_interval[b] = b_block;
//            f(allowed, suplied);
//        }
//    }

    pre_answer = Solver_artem(N, M, K, J, L, reservedRBs, userInfos, allowed);
    vector<Interval> answer;
    for (int i = 0; i < pre_answer.size(); i++){
        for (int g = 0; g < pre_answer[i].size(); g++){
            if (pre_answer[i][g].users.size()){
                answer.push_back(pre_answer[i][g]);
            }
        }
    }

    return answer;






}

vector<Interval> Solver_egor(int N, int M, int K, int J, int L,
                             vector<Interval> reservedRBs,
                             vector<UserInfo> userInfos
) {
    std::vector<MyInterval> free_spaces;
    {
        std::vector<bool> is_free(M, true);
        is_free.back() = false;
        int start = -1;
        for (size_t i = 0; i < reservedRBs.size(); i++) {
            for (size_t g = reservedRBs[i].start; g < reservedRBs[i].end; g++) {
                is_free[g] = false;
            }
        }

        for (int i = 0; i < M; i++) {
            if (!is_free[i]) {
                if (start != i - 1) {
                    free_spaces.push_back({start + 1, i});
                }
                start = i;
            }
        }
    }

    struct user {
        int id;
        int start = -1, end = -1;
    };

    auto build = [&]() {
        vector<vector<user>> blocks(free_spaces.size());
        // TODO: smart deterministic build algorithm
        for (int u = 0; u < N; u++) {

        }
        return blocks;
    };

    vector<vector<user>> blocks = build();

    auto f = [&](const vector<vector<user>> &blocks) {

    };


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

vector<Interval> Solver(int N, int M, int K, int J, int L,
                        vector<Interval> reservedRBs,
                        vector<UserInfo> userInfos);

vector<Interval> Solver(const TestData &testdata) {
    return Solver(testdata.N, testdata.M, testdata.K, testdata.J, testdata.L, testdata.reservedRBs, testdata.userInfos);
}

int get_solution_score(const TestData &testdata, const vector<Interval> &answer) {
    ASSERT(answer.size() <= testdata.J, "answer intervals is invalid: count intervals more than J");

    for (int i = 0; i < answer.size(); i++) {
        for (int j = i + 1; j < answer.size(); j++) {
            ASSERT(answer[i].end <= answer[j].start || answer[j].end <= answer[i].start,
                   "answer interval is invalid: intersect");
        }
    }

    vector<int> user_score(testdata.N);
    vector<int> user_min(testdata.N, 1e9);
    vector<int> user_max(testdata.N, -1e9);
    for (auto [start, end, users]: answer) {
        // validate interval
        {
            ASSERT(!users.empty(), "interval should contain atlest 1 user");
            ASSERT(users.size() <= testdata.L, "answer interval is invalid: users more than L");
            ASSERT(start < end, "answer interval is invalid: start >= end");
            ASSERT(0 <= start && end <= testdata.M, "answer interval is invalid: incorrect interval");
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

        ASSERT(user_min[u] == 1e9 || user_score[u] == user_max[u] - user_min[u],
               "answer interval is invalid: user have no continuous interval");
    }
    return sum_score;
}

vector<Interval> Solver(int N, int M, int K, int J, int L,
                        vector<Interval> reservedRBs,
                        vector<UserInfo> userInfos) {
    auto answer = Solver_Artem_grad(N, M, K, J, L, reservedRBs, userInfos);
    get_solution_score({N, M, K, J, L, reservedRBs, userInfos}, answer); // TODO: for verify answer
    return answer;
}
