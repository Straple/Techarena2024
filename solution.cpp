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
// =========================TIMER=====================================================================================
// ===================================================================================================================

#include <iostream>

// тики ведутся с 1-го января 1970 г. 00:00:00 Всемирного времени

// вернет частоту обновления устройства
// для удобства работы оно в efloat
const double get_performance_frequency();

// вернет текущий тик
uint64_t get_ticks();

class Timer {
    uint64_t start_tick;

public:
    Timer();

    // вернет время между начальным тиком и текущим
    [[nodiscard]] double get() const;

    // обновит начальный тик
    void reset();

    // вернет тик начала отсчета
    [[nodiscard]] uint64_t get_tick() const;
};

std::ostream &operator<<(std::ostream &output, const Timer &time);

#include <iomanip>

#ifdef _WIN32

#include <windows.h>
// windows.h defined min and max macros
// this is bad
#undef min
#undef max

uint64_t calc_performance_frequency() {
    LARGE_INTEGER perf;
    ASSERT(QueryPerformanceFrequency(&perf), "call to QueryPerformanceFrequency fails");
    return perf.QuadPart;
}

uint64_t get_ticks() {
    LARGE_INTEGER ticks;
    ASSERT(QueryPerformanceCounter(&ticks), "call to QueryPerformanceCounter fails");
    return ticks.QuadPart;
}

#elif defined(__linux__) || defined(__APPLE__)

#include <sys/time.h>

uint64_t calc_performance_frequency() {
    return 1'000'000;// колво микросекунд в секунде
}

uint64_t get_ticks() {
    timeval ticks;

    // вернет -1 в случае ошибки
    ASSERT(gettimeofday(&ticks, NULL) == 0, "call to gettimeofday fails");
    return ticks.tv_sec * 1'000'000 + ticks.tv_usec;
}

#else
static_assert(false, "not supported operating system");
#endif

const double get_performance_frequency() {
    static const double performance_frequency = static_cast<double>(calc_performance_frequency());
    return performance_frequency;
}

Timer::Timer()
        : start_tick(get_ticks()) {
}

[[nodiscard]] double Timer::get() const {
    return static_cast<double>(get_ticks() - start_tick) / get_performance_frequency();
}

void Timer::reset() {
    start_tick = get_ticks();
}

[[nodiscard]] uint64_t Timer::get_tick() const {
    return start_tick;
}

std::ostream &operator<<(std::ostream &output, const Timer &time) {
    return output << std::setprecision(4) << time.get() << "s";
}

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

int BEAM_MAX_AMOUNT = 32;

int get_blocks_amount(int M, vector<Interval> reservedRBs) {
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

int
get_solution_score_light(int N, const vector<vector<Interval>> &ans, const vector<UserInfo> &userInfos,
                         std::vector<int> &suplied) {
    for (int i = 0; i < N; i++) {
        suplied[i] = 0;
    }
    for (int i = 0; i < ans.size(); i++) {
        for (int g = 0; g < ans[i].size(); g++) {
            for (auto user_id: ans[i][g].users) {
                suplied[user_id] += ans[i][g].end - ans[i][g].start;
            }
        }
    }
    int score = 0;
    for (int i = 0; i < N; i++) {
        score += std::min(userInfos[i].rbNeed, suplied[i]);
    }

    return score;
}

void optimize_one_gap(int N, int M, int K, int J, int L,
                      const vector<Interval> &reservedRBs,
                      const vector<UserInfo> &userInfos, vector<Interval> &solution, vector<int> &suplied,
                      set<int> &empty) {

    std::vector<int> mi(N, 10000);
    std::vector<int> ma(N, -10000);
    std::vector<std::vector<int>> beamOwnedBy(solution.size(), std::vector<int>(BEAM_MAX_AMOUNT, -1));
    for (int i = 0; i < solution.size(); i++) {
        for (auto user_id: solution[i].users) {
            mi[user_id] = min(mi[user_id], i);
            ma[user_id] = max(ma[user_id], i);
            beamOwnedBy[i][userInfos[user_id].beam] = user_id;
        }
    }
    std::vector<set<int>> mi_set(M);
    std::vector<set<int>> ma_set(M);
    for (int i = 0; i < N; i++) {
        if (mi[i] != 10000) {
            mi_set[mi[i]].insert(i);
        }
        if (ma[i] != -10000) {
            ma_set[ma[i]].insert(i);
        }
    }


    for (int iter = 0; iter < 3; iter++) {
        // Обрезаем хвост, отдаём голове
        for (int i = 0; i < N; i++) {
            if (mi[i] != 10000 && mi[i] != 0) { // можно сделать так что бь mi == 0
                int best_score_gain = -1;
                int best_receiver = -1;
                int current_len = solution[mi[i]].end - solution[mi[i]].start;
                int minus = min(suplied[i], userInfos[i].rbNeed) - min(suplied[i] - current_len, userInfos[i].rbNeed);
                bool is_empty_winner = false;
                for (auto &give_to: ma_set[mi[i] - 1]) {
                    if (userInfos[i].beam == userInfos[give_to].beam ||
                        beamOwnedBy[mi[i]][userInfos[give_to].beam] == -1) {
                        // Только одинаковые beam. Потом любые
                        int plus = min(suplied[give_to] + current_len, userInfos[give_to].rbNeed) -
                                   min(suplied[give_to], userInfos[give_to].rbNeed);
                        //cout << suplied[give_to] << " " << current_len << " " << userInfos[give_to].rbNeed << endl;
                        if (plus > best_score_gain) {
                            best_score_gain = plus;
                            best_receiver = give_to;
                            is_empty_winner = false;
                        }
                    }
                }
                for (auto &give_to: empty) {
                    if (userInfos[i].beam == userInfos[give_to].beam ||
                        beamOwnedBy[mi[i]][userInfos[give_to].beam] == -1) {
                        // Только одинаковые beam. Потом любые
                        int plus = min(suplied[give_to] + current_len, userInfos[give_to].rbNeed) -
                                   min(suplied[give_to], userInfos[give_to].rbNeed);
                        //cout << suplied[give_to] << " " << current_len << " " << userInfos[give_to].rbNeed << endl;
                        if (plus > best_score_gain) {
                            best_score_gain = plus;
                            best_receiver = give_to;
                            is_empty_winner = true;
                        }
                    }
                }
                if (best_score_gain > minus) {
                    //cerr << "+" << best_score_gain - minus << endl;
                    if (is_empty_winner) {
                        empty.erase(best_receiver);
                        //cerr << "IS EMPTY" << endl;
                    }
                    //cerr << "PUTTING" << best_receiver << "(" << userInfos[best_receiver].beam << ")" << " instead of " << i << "(" << userInfos[i].beam << ")" << " " << mi[i] << endl;
                    //cerr << "OPTIMIZING1" << endl;
                    auto iter = find(solution[mi[i]].users.begin(), solution[mi[i]].users.end(), i);
                    solution[mi[i]].users.erase(iter); // optimize_it
                    solution[mi[i]].users.push_back(best_receiver);

                    suplied[i] = suplied[i] - current_len;
                    suplied[best_receiver] = suplied[best_receiver] + current_len;
                    mi_set[mi[i]].erase(i);
                    if (!is_empty_winner) {
                        ma_set[ma[best_receiver]].erase(best_receiver);
                    }
                    beamOwnedBy[mi[i]][userInfos[i].beam] = -1;
                    mi[i]++;
                    if (is_empty_winner) {
                        mi[best_receiver] = mi[i] - 1;
                        ma[best_receiver] = mi[i] - 1;
                    } else {
                        ma[best_receiver]++;
                    }
                    beamOwnedBy[ma[best_receiver]][userInfos[best_receiver].beam] = best_receiver;

                    ma_set[ma[best_receiver]].insert(best_receiver);
                    if (mi[i] > ma[i]) {
                        //cerr << "EMPTYING " << i << endl;
                        empty.insert(i);
                        ma_set[ma[i]].erase(i);
                        if (mi[i] < M) {
                            mi_set[mi[i]].erase(i);
                        }
                        mi[i] = 10000;
                        ma[i] = -10000;
                    } else {
                        mi_set[mi[i]].insert(i);
                    }
                    // cerr << endl;


                }
            }
        }

        // Обрезаем голову, отдаём хвосту
        for (int i = 0; i < N; i++) {
            if (ma[i] != -10000 && ma[i] + 1 != solution.size()) { // можно сделать так что бь mi == 0
                int best_score_gain = -1;
                int best_receiver = -1;
                int current_len = solution[mi[i]].end - solution[mi[i]].start;
                int minus = min(suplied[i], userInfos[i].rbNeed) - min(suplied[i] - current_len, userInfos[i].rbNeed);
                bool is_empty_winner = false;
                for (auto &give_to: mi_set[ma[i] + 1]) {
                    if (userInfos[i].beam == userInfos[give_to].beam ||
                        beamOwnedBy[ma[i]][userInfos[give_to].beam] == -1) {
                        // Только одинаковые beam. Потом любые
                        int plus = min(suplied[give_to] + current_len, userInfos[give_to].rbNeed) -
                                   min(suplied[give_to], userInfos[give_to].rbNeed);
                        if (suplied[give_to] != userInfos[give_to].rbNeed) {
//                            cout << "WHOA WHOA" << endl;
//                            cout << suplied[give_to] << " " << current_len << " " << userInfos[give_to].rbNeed << endl;

                        }
                        if (plus > best_score_gain) {
//                            cout << "PLUSS " << plus << "|" << minus << endl;
                            best_score_gain = plus;
                            best_receiver = give_to;
                            is_empty_winner = false;
                        }
                    }
                }
                for (auto &give_to: empty) {
                    if (userInfos[i].beam == userInfos[give_to].beam ||
                        beamOwnedBy[ma[i]][userInfos[give_to].beam] == -1) {
                        // Только одинаковые beam. Потом любые
                        int plus = min(suplied[give_to] + current_len, userInfos[give_to].rbNeed) -
                                   min(suplied[give_to], userInfos[give_to].rbNeed);
                        //cout << suplied[give_to] << " " << current_len << " " << userInfos[give_to].rbNeed << endl;
                        if (plus > best_score_gain) {
                            best_score_gain = plus;
                            best_receiver = give_to;
                            is_empty_winner = true;
                        }
                    }
                }
                if (best_score_gain > minus) {
                    //cerr << "+" << best_score_gain - minus << endl;
                    //cerr << "OPTIMIZING2" << endl;
                    if (is_empty_winner) {
                        empty.erase(best_receiver);
                        //cerr << "EMPTY" << endl;
                    }
                    //cerr << "PUTTING" << best_receiver << "(" << userInfos[best_receiver].beam << ")" << " instead of " << i << "(" << userInfos[i].beam << ")" << " " << mi[i] << endl;
                    auto iter = find(solution[ma[i]].users.begin(), solution[ma[i]].users.end(), i);
                    solution[ma[i]].users.erase(iter); // optimize_it
                    solution[ma[i]].users.push_back(best_receiver);
                    suplied[i] = suplied[i] - current_len;
                    suplied[best_receiver] = suplied[best_receiver] + current_len;
                    ma_set[ma[i]].erase(i);
                    if (!is_empty_winner) {
                        mi_set[mi[best_receiver]].erase(best_receiver);
                    }
                    beamOwnedBy[ma[i]][userInfos[i].beam] = -1;
                    ma[i]--;
                    if (is_empty_winner) {
                        ma[best_receiver] = ma[i] + 1;
                        mi[best_receiver] = ma[i] + 1;
                    } else {
                        mi[best_receiver]--;
                    }
                    beamOwnedBy[mi[best_receiver]][userInfos[best_receiver].beam] = best_receiver;
                    mi_set[ma[best_receiver]].insert(best_receiver);

                    if (mi[i] > ma[i]) {
                        //cerr << "EMPTyING " << i << endl;
                        empty.insert(i);
                        if (ma[i] > 0) {
                            ma_set[ma[i]].erase(i);
                        }
                        mi_set[mi[i]].erase(i);
                        mi[i] = 10000;
                        ma[i] = -10000;
                    } else {
                        ma_set[mi[i]].insert(i);
                    }
                    //cerr << endl;
                }
            }
        }
    }


}

void optimize(int N, int M, int K, int J, int L,
              const vector<Interval> &reservedRBs,
              const vector<UserInfo> &userInfos, vector<vector<Interval>> &solution) {

    std::vector<int> suplied(N, 0);
    get_solution_score_light(N, solution, userInfos, suplied);
    set<int> empty;
    std::vector<bool> was(N, false);
    for (int i = 0; i < solution.size(); i++) {
        for (int g = 0; g < solution[i].size(); g++) {
            for (auto user_id: solution[i][g].users) {
                was[user_id] = true;
            }
        }
    }
    for (int i = 0; i < N; i++) {
        if (!was[i]) {
            empty.insert(i);
        }
    }
    for (int i = 0; i < solution.size(); i++) {
        optimize_one_gap(N, M, K, J, L, reservedRBs, userInfos, solution[i], suplied, empty);
    }
}

vector<vector<Interval>> Solver_artem(int N, int M, int K, int J, int L,
                                      vector<Interval> &reservedRBs,
                                      vector<UserInfo> &userInfos, std::vector<std::set<int>> &allowed_users_per_block
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
    sort(free_spaces.begin(), free_spaces.end(), [&](const MyInterval &lhs, const MyInterval &rhs) {
        if (lhs.end - lhs.start == rhs.end - rhs.start) {
            return lhs.start < rhs.start;
        } else {
            return lhs.end - lhs.start > rhs.end - rhs.start;
        }
    });

    vector<std::vector<Interval>> pre_answer(free_spaces.size());
    {
        int total_free_space_size = 0;
        for (auto free_space: free_spaces) {
            total_free_space_size += free_space.len();
        }
        int TARGET_LEN = total_free_space_size / J;
        std::vector<int> free_spaces_seperation_starts;
        for (auto free_space: free_spaces) {
            free_spaces_seperation_starts.push_back(free_space.start);
        }
        for (auto j = 0; j < J; j++) {
            // выбрать наибольший возможный, отрезать от него кусок TARGET_LEN;
            int selected_index = -1;
            int selected_size = -1;
            for (int i = 0; i < free_spaces.size(); i++) {
                int current_possible_len = free_spaces[i].end - free_spaces_seperation_starts[i];
                if (current_possible_len > selected_size) {
                    selected_size = current_possible_len;
                    selected_index = i;
                }
            }
            assert(selected_index != -1);
            int can_cut = selected_size; // если это не последний отрезаемый кусок. Тогда отрезаем всё что есть
            if (j + 1 != J) {
                can_cut = std::min(selected_size, TARGET_LEN);
            }
            int currect_start = free_spaces_seperation_starts[selected_index];
            pre_answer[selected_index].push_back({currect_start, currect_start + can_cut, {}});
            free_spaces_seperation_starts[selected_index] += can_cut;

        }
    }
    std::vector<int> rbSuplied(N, 0);
    std::set<int> used_users;
    int current_interval_iter = 0;
    std::set<pair<int, int>, std::greater<>> space_left_q;
    for (int i = 0; i < pre_answer.size(); i++) {
        int total_size = 0;
        for (int g = 0; g < pre_answer[i].size(); g++) {
            total_size += pre_answer[i][g].end - pre_answer[i][g].start;
        }
        if (total_size != 0) {
            space_left_q.insert({total_size, i});
        }
    }
    std::vector<std::vector<int>> beamOwnedBy(pre_answer.size(), std::vector<int>(BEAM_MAX_AMOUNT, -1));
    std::vector<std::set<int>> activeUsers(pre_answer.size());
    std::vector<int> current_sub_interval(pre_answer.size(), 0);
    while (!space_left_q.empty()) {
        int space_left = space_left_q.begin()->first;
        int pick_i = space_left_q.begin()->second;
        space_left_q.erase(space_left_q.begin());
        // юзаем OWNEDBY до конца!!!
        // считаем кол-во свободных слотов.

        // Набираем
        std::vector<std::pair<int, int>> candidates;
        for (auto &user: userInfos) {
            if (used_users.find(user.id) == used_users.end() && beamOwnedBy[pick_i][user.beam] == -1 &&
                allowed_users_per_block[pick_i].count(user.id)) {
                assert(rbSuplied[user.id] == 0);
                candidates.push_back({user.rbNeed - rbSuplied[user.id], user.id});
            }
        }
        sort(candidates.begin(), candidates.end(), greater<>());
        int get_more = L - activeUsers[pick_i].size();
        for (int g = 0; g < (int) candidates.size(); g++) {
            if (get_more == 0) break;
            if (beamOwnedBy[pick_i][userInfos[candidates[g].second].beam] == -1) {
                activeUsers[pick_i].insert(candidates[g].second);
                used_users.insert(candidates[g].second);
                beamOwnedBy[pick_i][userInfos[candidates[g].second].beam] = candidates[g].second;
                get_more--;
            }
        }

        std::set<int> to_delete;
        for (auto user_id: activeUsers[pick_i]) {
            rbSuplied[user_id] += pre_answer[pick_i][current_sub_interval[pick_i]].end -
                                  pre_answer[pick_i][current_sub_interval[pick_i]].start;
            pre_answer[pick_i][current_sub_interval[pick_i]].users.push_back(user_id);
            if (rbSuplied[user_id] > userInfos[user_id].rbNeed) {
                to_delete.insert(user_id);
            }
        }
        for (auto user_id: to_delete) {
            activeUsers[pick_i].erase(user_id);
            beamOwnedBy[pick_i][userInfos[user_id].beam] = -1;
        }
        space_left -= pre_answer[pick_i][current_sub_interval[pick_i]].end -
                      pre_answer[pick_i][current_sub_interval[pick_i]].start;
        current_sub_interval[pick_i]++;
        if (space_left != 0) {
            space_left_q.insert({space_left, pick_i});
        }
    }
    return pre_answer;
}

//TEST CASE: K=0 | tests: 666 | score: 90.227% | 618201/685162 | time: 42.606ms | max_time: 5.191ms | mean_time: 0.063973ms
//TEST CASE: K=1 | tests: 215 | score: 87.301% | 197117/225790 | time: 26.037ms | max_time: 3.674ms | mean_time: 0.121102ms
//TEST CASE: K=2 | tests: 80 | score: 86.6498% | 72064/83167 | time: 8.278ms | max_time: 0.436ms | mean_time: 0.103475ms
//TEST CASE: K=3 | tests: 39 | score: 86.8018% | 40138/46241 | time: 4.398ms | max_time: 0.391ms | mean_time: 0.112769ms
//TEST CASE: K=4 | tests: 0 | score: -nan% | 0/0 | time: 0ms | max_time: 0ms | mean_time: 0ms
//TOTAL: tests: 1000 | score: 89.1538% | 927520/1040360 | time: 81.319ms | max_time: 5.191ms | mean_time: 0.081319ms
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
    std::map<int, int> user_to_interval;
    for (int i = 0; i < pre_answer.size(); i++) {
        for (int g = 0; g < pre_answer[i].size(); g++) {
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
    for (int i = 0; i < pre_answer.size(); i++) {
        for (int g = 0; g < pre_answer[i].size(); g++) {
            if (pre_answer[i][g].users.size()) {
                answer.push_back(pre_answer[i][g]);
            }
        }
    }

    return answer;


}

struct EgorTaskSolver {
    ///============================
    /// task data
    ///============================

    int N;
    int M;
    int K;
    int J;
    int L;

    vector<MyInterval> free_intervals;

    ///============================
    /// internal data
    ///============================

    randomizer rnd;

    int total_score = 0;

    vector<vector<Interval>> intervals;

    vector<int> sum_intervals_len;

    struct MyUserInfo {
        // user info
        int id;
        int rbNeed;
        int beam;

        // interval position
        int block = -1;
        int left = -1;
        int right = -1;

        // score
        int sum_len = 0;

        int calc_score() const {
            return min(sum_len, rbNeed);
        }
    };

    vector<MyUserInfo> users_info;

    EgorTaskSolver(int NN, int MM, int KK, int JJ, int LL,
                   const vector<Interval> &reservedRBs,
                   const vector<UserInfo> &userInfos) : N(NN), M(MM), K(KK), J(JJ), L(LL) {

        users_info.resize(N);
        for (int u = 0; u < N; u++) {
            ASSERT(u == userInfos[u].id, "are you stupid or something?");
            users_info[u].id = userInfos[u].id;
            users_info[u].rbNeed = userInfos[u].rbNeed;
            users_info[u].beam = userInfos[u].beam;
        }

        // build free_intervals
        {
            vector<bool> is_free(M + 1, true);
            is_free.back() = false;

            for (const auto &[start, end, users]: reservedRBs) {
                for (int i = start; i < end; i++) {
                    is_free[i] = false;
                }
            }

            int start = -1;
            for (int i = 0; i < is_free.size(); i++) {
                if (!is_free[i]) {
                    if (start != i - 1) {
                        free_intervals.push_back({start + 1, i});
                    }
                    start = i;
                }
            }
        }

        // build J intervals
        {
            sum_intervals_len.resize(free_intervals.size());
            intervals.resize(free_intervals.size());

            int sum_free_len = 0;
            for (int block = 0; block < free_intervals.size(); block++) {
                sum_free_len += free_intervals[block].len();
            }
            int mean_len = sum_free_len / J;

            int p = 0;
            for (int j = 0; j < J; j++) {
                bool find = false;
                for (int block = 0; block < free_intervals.size(); block++) {
                    if (sum_intervals_len[block] + mean_len <= free_intervals[block].len()) {
                        find = true;
                        sum_intervals_len[block] += mean_len;
                        intervals[block].push_back(Interval{0, mean_len, vector<int>()});
                        break;
                    }
                }

                if (!find) {
                    intervals[p].push_back(Interval{0, 0, vector<int>()});
                    p = (p + 1) % free_intervals.size();
                }
            }

        }
    }

    vector<Interval> get_total_answer() {
        vector<Interval> answer;
        for (int block = 0; block < intervals.size(); block++) {
            int start = free_intervals[block].start;
            for (int interval = 0; interval < intervals[block].size(); interval++) {
                if (intervals[block][interval].end != 0) {
                    intervals[block][interval].start = start;
                    intervals[block][interval].end += start;
                    start = intervals[block][interval].end;

                    answer.push_back(intervals[block][interval]);
                }
            }
        }
        return answer;
    }

    bool have_equal_beam(int block, int interval, int beam) {
        ASSERT(0 <= block && block < intervals.size() && 0 <= interval && interval < intervals[block].size(),
               "invalid request");
        for (int u: intervals[block][interval].users) {
            if (users_info[u].beam == beam) {
                return true;
            }
        }
        return false;
    }

    ///===========================
    ///===========ACTIONS=========
    ///===========================

    void change_interval_len(int block, int interval, int change) {
        auto &[start, end, users] = intervals[block][interval];
        end += change;
        sum_intervals_len[block] += change;
        ASSERT(start <= end, "invalid interval");

#ifdef MY_DEBUG_MODE
        int sum_len = 0;
        for (int i = 0; i < intervals[block].size(); i++) {
            sum_len += length(intervals[block][i]);
        }
        ASSERT(sum_len <= free_intervals[block].len(), "len more than free interval");
        ASSERT(sum_len == sum_intervals_len[block], "failed calculating sum_intervals_len");
#endif

        for (int u: users) {
            total_score -= users_info[u].calc_score();
            users_info[u].sum_len += change;
            total_score += users_info[u].calc_score();
        }
    }

    void add_user_in_interval(int u, int block, int interval) {
        ASSERT(intervals[block][interval].users.size() + 1 <= L, "failed add");

        total_score -= users_info[u].calc_score();
        intervals[block][interval].users.push_back(u);
        users_info[u].sum_len += length(intervals[block][interval]);
        total_score += users_info[u].calc_score();
    }

    void remove_user_in_interval(int u, int block, int interval) {
        total_score -= users_info[u].calc_score();
        auto &users = intervals[block][interval].users;
        users.erase(find(users.begin(), users.end(), u));
        users_info[u].sum_len -= length(intervals[block][interval]);
        total_score += users_info[u].calc_score();
    }

    void add_right_interval_in_user(int u) {
        ASSERT(users_info[u].block != -1, "invalid u");
        ASSERT(users_info[u].right + 1 < intervals[users_info[u].block].size()
               && users_info[u].left <= users_info[u].right, "no right interval");

        users_info[u].right++;
        add_user_in_interval(u, users_info[u].block, users_info[u].right);
    }

    void remove_right_interval_in_user(int u) {
        ASSERT(users_info[u].block != -1, "invalid u");
        ASSERT(users_info[u].right >= 0 && users_info[u].left <= users_info[u].right, "no right interval");

        remove_user_in_interval(u, users_info[u].block, users_info[u].right);
        users_info[u].right--;
    }

    void add_left_interval_in_user(int u) {
        ASSERT(users_info[u].block != -1, "invalid u");
        ASSERT(users_info[u].left - 1 >= 0 && users_info[u].left <= users_info[u].right, "no left interval");

        users_info[u].left--;
        add_user_in_interval(u, users_info[u].block, users_info[u].left);
    }

    void remove_left_interval_in_user(int u) {
        ASSERT(users_info[u].block != -1, "invalid u");
        ASSERT(users_info[u].left >= 0 && users_info[u].left <= users_info[u].right, "no left interval");

        remove_user_in_interval(u, users_info[u].block, users_info[u].left);
        users_info[u].left++;
    }

    void new_user_interval(int u, int block, int interval) {
        ASSERT(users_info[u].block == -1, "user already have interval");
        users_info[u].block = block;
        users_info[u].left = users_info[u].right = interval;
        add_user_in_interval(u, block, interval);
    }

    void remove_all_user_interval(int u) {
        ASSERT(users_info[u].block != -1, "user already haven't interval");
        for (int interval = users_info[u].left; interval <= users_info[u].right; interval++) {
            remove_user_in_interval(u, users_info[u].block, interval);
        }
        users_info[u].block = users_info[u].left = users_info[u].right = -1;
    }

    ///==========================
    ///===========RANDOM=========
    ///==========================

    //TEST CASE: K=0 | tests: 666 | score: 71.275% | 488349/685162 | time: 3828.16ms | max_time: 11.694ms | mean_time: 5.74799ms
    //TEST CASE: K=1 | tests: 215 | score: 64.6162% | 145897/225790 | time: 1133.69ms | max_time: 7.791ms | mean_time: 5.27297ms
    //TEST CASE: K=2 | tests: 80 | score: 63.0971% | 52476/83167 | time: 417.995ms | max_time: 7.488ms | mean_time: 5.22494ms
    //TEST CASE: K=3 | tests: 39 | score: 57.9637% | 26803/46241 | time: 194.571ms | max_time: 7.034ms | mean_time: 4.989ms
    //TEST CASE: K=4 | tests: 0 | score: -nan% | 0/0 | time: 0ms | max_time: 0ms | mean_time: 0ms
    //TOTAL: tests: 1000 | score: 68.5844% | 713525/1040360 | time: 5574.42ms | max_time: 11.694ms | mean_time: 5.57442ms

    void annealing() {
        double temp = 1;
        auto is_good = [&](int old_score) {
            return total_score > old_score || rnd.get_d() < exp((total_score - old_score) / temp);
        };

        //cout << total_score << "->";
        //cout.flush();
        for (int step = 0; step < 100'000; step++) {
            temp *= 0.9999;

            if (rnd.get_d() < 0.3) {
                // update interval

                int block = rnd.get(0, free_intervals.size() - 1);
                if (intervals[block].empty()) {
                    continue;
                }

                int interval = rnd.get(0, intervals[block].size() - 1);

                int change = min(max(-length(intervals[block][interval]), (int) rnd.get(-10, 10)),
                                 free_intervals[block].len() - sum_intervals_len[block] -
                                 length(intervals[block][interval]));

                ASSERT(0 <= length(intervals[block][interval]) + change &&
                       length(intervals[block][interval]) + change + sum_intervals_len[block] <=
                       free_intervals[block].len(), "kek");

                int old_score = total_score;

                change_interval_len(block, interval, change);

                if (is_good(old_score)) {
                } else {
                    change_interval_len(block, interval, -change);
                    ASSERT(old_score == total_score, "failed back score");
                }

            } else if (rnd.get_d() < 0.5) {
                // update user

                int u = rnd.get(0, N - 1);

                if (users_info[u].block == -1) {
                    // no interval

                    int block = rnd.get(0, free_intervals.size() - 1);
                    if (intervals[block].empty()) {
                        continue;
                    }

                    int interval = rnd.get(0, intervals[block].size() - 1);

                    if (intervals[block][interval].users.size() + 1 <= L &&
                        !have_equal_beam(block, interval, users_info[u].beam)) {

                        int old_score = total_score;

                        new_user_interval(u, block, interval);

                        if (is_good(old_score)) {

                        } else {
                            remove_all_user_interval(u);
                            ASSERT(old_score == total_score, "failed back score");
                        }
                    }

                } else {

                    int block = users_info[u].block;
                    int right = users_info[u].right;
                    int left = users_info[u].left;
                    int beam = users_info[u].beam;

                    if (right + 1 < intervals[block].size() &&
                        intervals[block][right + 1].users.size() + 1 <= L &&
                        !have_equal_beam(block, right + 1, beam) &&
                        rnd.get_d() < 0.2) {

                        int old_score = total_score;

                        add_right_interval_in_user(u);

                        if (is_good(old_score)) {

                        } else {
                            remove_right_interval_in_user(u);
                            ASSERT(old_score == total_score, "failed back score");
                        }
                    } else if (left > 0 &&
                               intervals[block][left - 1].users.size() + 1 <= L &&
                               !have_equal_beam(block, left - 1, beam) &&
                               rnd.get_d() < 0.4) {

                        int old_score = total_score;

                        add_left_interval_in_user(u);

                        if (is_good(old_score)) {

                        } else {
                            remove_left_interval_in_user(u);
                            ASSERT(old_score == total_score, "failed back score");
                        }
                    } else if (left + 1 <= right &&
                               rnd.get_d() < 0.1) {

                        int old_score = total_score;

                        remove_left_interval_in_user(u);

                        if (is_good(old_score)) {

                        } else {
                            add_left_interval_in_user(u);
                            ASSERT(old_score == total_score, "failed back score");
                        }
                    } else if (left + 1 <= right &&
                               rnd.get_d() < 0.1) {

                        int old_score = total_score;

                        remove_right_interval_in_user(u);

                        if (is_good(old_score)) {

                        } else {
                            add_right_interval_in_user(u);
                            ASSERT(old_score == total_score, "failed back score");
                        }
                    } else {
                        int old_score = total_score;

                        remove_all_user_interval(u);

                        if (is_good(old_score)) {

                        } else {
                            new_user_interval(u, block, left);
                            for (int i = left + 1; i <= right; i++) {
                                add_right_interval_in_user(u);
                            }
                            ASSERT(old_score == total_score, "failed back score");
                        }
                    }
                }
            }
        }
        //cout << temp << endl;
        //cout << total_score << endl;
    }
};

//TEST CASE: K=0 | tests: 666 | score: 93.3172% | 639374/685162 | time: 612.187ms | max_time: 9.977ms | mean_time: 0.9192ms
//TEST CASE: K=1 | tests: 215 | score: 91.7999% | 207275/225790 | time: 235.996ms | max_time: 9.534ms | mean_time: 1.09766ms
//TEST CASE: K=2 | tests: 80 | score: 90.471% | 75242/83167 | time: 119.629ms | max_time: 8.073ms | mean_time: 1.49536ms
//TEST CASE: K=3 | tests: 39 | score: 90.6576% | 41921/46241 | time: 72.05ms | max_time: 6.908ms | mean_time: 1.84744ms
//TEST CASE: K=4 | tests: 0 | score: -nan% | 0/0 | time: 0ms | max_time: 0ms | mean_time: 0ms
//TOTAL: tests: 1000 | score: 92.6422% | 963812/1040360 | time: 1039.86ms | max_time: 9.977ms | mean_time: 1.03986ms
vector<Interval> Solver_egor(int N, int M, int K, int J, int L,
                             const vector<Interval> &reservedRBs,
                             const vector<UserInfo> &userInfos
) {
    EgorTaskSolver solver(N, M, K, J, L, reservedRBs, userInfos);

    solver.annealing();

    auto answer = solver.get_total_answer();
    ASSERT(get_solution_score(N, M, K, J, L, reservedRBs, userInfos, answer) == solver.total_score,
           "failed calculate total_score");

    return answer;
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

    auto egor_answer = Solver_egor(N, M, K, J, L, reservedRBs, userInfos);
    double egor_score = get_solution_score({N, M, K, J, L, reservedRBs, userInfos}, egor_answer);
    return egor_answer;
    /*auto artem_answer = Solver_Artem_grad(N, M, K, J, L, reservedRBs, userInfos);
    double artem_score = get_solution_score({N, M, K, J, L, reservedRBs, userInfos}, artem_answer);

    auto egor_answer = Solver_egor(N, M, K, J, L, reservedRBs, userInfos);
    double egor_score = get_solution_score({N, M, K, J, L, reservedRBs, userInfos}, egor_answer);

    if (artem_score > egor_score) {
        return artem_answer;
    } else {
        return egor_answer;
    }*/
}
