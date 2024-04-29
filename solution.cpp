// ===================================================================================================================
// =========================ASSERT====================================================================================
// ===================================================================================================================
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
    vector<int> users;

    int len() const {
        return end - start;
    }
};

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


vector<Interval> Solver_artem(int N, int M, int K, int J, int L,
                              vector<Interval> reservedRBs,
                              vector<UserInfo> userInfos
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
    vector<Interval> answer;
    for (int i = 0; i < min((int) free_spaces.size(), J); i++) {
        answer.push_back({free_spaces[i].start, free_spaces[i].end, {}});
    }
    std::set<int> used_users;
    for (int i = 0; i < answer.size(); i++) {
        std::set<int> used_beams;
        for (auto user: userInfos) {
            if (answer[i].users.size() == L) {
                break;
            }
            if (used_users.find(user.id) == used_users.end() && used_beams.find(user.beam) == used_beams.end()) {
                used_beams.insert(user.beam);
                answer[i].users.push_back(user.id);
                used_users.insert(user.id);
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
        int id = -1;
        int len = 0;
    };

    randomizer rnd;

    auto build_blocks = [&]() {
        vector<vector<user>> blocks(free_spaces.size());
        // TODO: smart deterministic build algorithm
        for (int u = 0; u < N; u++) {
            int block = rnd.get(0, blocks.size() - 1);
            blocks[block].emplace_back(u, 0);
        }
        return blocks;
    };

    auto build_answer = [&](const vector<vector<user>> &blocks) {
        vector<vector<Interval>> block_answer(blocks.size());

        int cnt_total_intervals = 0;

        // (len, block, id)
        vector<tuple<int, int, int>> users;
        for (int block = 0; block < blocks.size(); block++) {
            block_answer[block].emplace_back(free_spaces[block].start, free_spaces[block].end, vector<int>());
            for (auto [id, len]: blocks[block]) {
                if (len > 0) {
                    users.emplace_back(len, block, id);
                }
            }
        }
        sort(users.begin(), users.end(), greater<>());

        for (auto [len, block, id]: users) {
            auto &intervals = block_answer[block];
            int best_i = -1;
            int best_len = 1e9;
            for (int i = 0; i < intervals.size(); i++) {

                // длина подходит
                if (len <= intervals[i].len() && intervals[i].users.size() < L) {
                    // можно ли поставить в этот интервал юзера?
                    bool ok = true;
                    for (auto u: intervals[i].users) {
                        if (userInfos[u].beam == userInfos[id].beam) {
                            ok = false;
                            break;
                        }
                    }

                    if (ok) {
                        if (intervals[i].len() < best_len) {
                            best_len = intervals[i].end - intervals[i].start;
                            best_i = i;
                        }
                    }
                }
            }

            if (best_i != -1) {
                if (len == intervals[best_i].len()) {
                    // full interval
                    if (intervals[best_i].users.empty()) {
                        if (cnt_total_intervals < J) {
                            cnt_total_intervals++;
                            intervals[best_i].users.push_back(id);
                        }
                    } else if (cnt_total_intervals <= J) {
                        intervals[best_i].users.push_back(id);
                    }
                } else {
                    // split
                    if (cnt_total_intervals < J) {
                        cnt_total_intervals++;
                        intervals.push_back(intervals[best_i]);
                        intervals[best_i].end = intervals[best_i].start + len;
                        intervals.back().start = intervals[best_i].end;
                        intervals[best_i].users.push_back(id);
                    }
                }
            }
        }

        vector<Interval> answer;
        for (int block = 0; block < blocks.size(); block++) {
            for (auto interval: block_answer[block]) {
                if (!interval.users.empty()) {
                    answer.push_back(interval);
                }
            }
        }

        // old build answer version
        /*vector<vector<int>> open(M), close(M);

        for (int block = 0; block < blocks.size(); block++) {
            for (auto [id, len]: blocks[block]) {
                if (start != -1) {
                    open[start].push_back(id);
                    close[end].push_back(id);
                }
            }
        }

        vector<int> users;
        int start = 0;
        for (int i = 0; i < M; i++) {
            if (!open[i].empty() || !close[i].empty()) {
                // need new interval
                if (!users.empty()) {
                    answer.emplace_back(start, i, users);
                }

                start = i;
            }

            for (int u: open[i]) {
                users.push_back(u);
            }
            for (int u: close[i]) {
                users.erase(find(users.begin(), users.end(), u));
            }
        }*/

        return answer;
    };

    auto calc_score = [&](const vector<vector<user>> &blocks) {
        return get_solution_score(N, M, K, J, L, reservedRBs, userInfos, build_answer(blocks));
        /*double score = 0;

        // old version calc_score
        vector<int> user_score(N, -1);
        vector<set<int>> beams(M);
        vector<set<int>> users(M);

        for (int block = 0; block < blocks.size(); block++) {
            for (auto [id, start, end]: blocks[block]) {
                if (start == -1) {
                    continue;
                }

                ASSERT(user_score[id] == -1, "already met this user");
                user_score[id] = min(userInfos[id].rbNeed, end - start);

                ASSERT(start < end, "invalid interval");
                ASSERT(free_spaces[block].start <= start && end <= free_spaces[block].end, "invalid interval");

                for (int i = start; i < end; i++) {
                    if (beams[i].count(userInfos[id].beam)) {
                        score -= 2;
                    } else {
                        beams[i].insert(userInfos[id].beam);
                    }

                    users[i].insert(id);
                }
            }
        }

        int cnt_intervals = 0;
        for (int i = 0; i + 1 < M; i++) {
            if (users[i] != users[i + 1]) {
                cnt_intervals++;
            }
        }

        if (cnt_intervals > J) {
            score -= (cnt_intervals - J) * 5;
        }

        for (int i = 0; i < M; i++) {
            if (users[i].size() > L) {
                score -= 0.9 * (users[i].size() - L);
            }
        }

        for (int u = 0; u < N; u++) {
            if (user_score[u] != -1) {
                score += user_score[u];
            }
        }
        return score;*/
    };

    vector<vector<user>> blocks = build_blocks();

    /*double cur_score = calc_score(blocks);
    double t = 1;
    for (int step = 0; step < 10000; step++) {
        int block = rnd.get(0, blocks.size() - 1);
        if (!blocks[block].empty()) {
            t *= 0.999;
            int i = rnd.get(0, blocks[block].size() - 1);

            int old_len = blocks[block][i].len;
            int new_len = max(0, (int)rnd.get(-10, 10));

            blocks[block][i].len = new_len;
            double new_score = calc_score(blocks);
            if (new_score > cur_score || rnd.get_d() < exp((new_score - cur_score) / t)) {
                cur_score = new_score;
            } else {
                blocks[block][i].len = old_len;
            }
        }
    }*/

    /*for (bool run = true; run;) {
        run = false;
        //cout << cur_score << ' ' << calc_score(blocks) << endl;

        for (int block = 0; block < blocks.size(); block++) {
            for (int i = 0; i < blocks[block].size(); i++) {
                auto &[id, len] = blocks[block][i];

                if (len > 0) {
                    int save_len = len;
                    len--;
                    double new_score = calc_score(blocks);
                    if (new_score > cur_score) {
                        cur_score = new_score;
                        run = true;
                    } else {
                        len = save_len;
                    }
                }

                {
                    int save_len = len;
                    len++;
                    double new_score = calc_score(blocks);
                    if (new_score > cur_score) {
                        cur_score = new_score;
                        run = true;
                    } else {
                        len = save_len;
                    }
                }

                {
                    int save_len = len;
                    len = rnd.get(0, userInfos[id].rbNeed);
                    double new_score = calc_score(blocks);
                    if (new_score > cur_score) {
                        cur_score = new_score;
                        run = true;
                    } else {
                        len = save_len;
                    }
                }
            }
        }
    }*/

    cout << cur_score << ' ' << calc_score(blocks) << '\n';

    return build_answer(blocks);
}

vector<Interval> Solver(int N, int M, int K, int J, int L,
                        vector<Interval> reservedRBs,
                        vector<UserInfo> userInfos);

vector<Interval> Solver(const TestData &testdata) {
    return Solver(testdata.N, testdata.M, testdata.K, testdata.J, testdata.L, testdata.reservedRBs, testdata.userInfos);
}

vector<Interval> Solver(int N, int M, int K, int J, int L,
                        vector<Interval> reservedRBs,
                        vector<UserInfo> userInfos) {
    auto answer = Solver_egor(N, M, K, J, L, reservedRBs, userInfos);
    //get_solution_score({N, M, K, J, L, reservedRBs, userInfos}, answer); // TODO: for verify answer
    return answer;
}
