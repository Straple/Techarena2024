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
// =========================MY_BITS_SET===============================================================================
// ===================================================================================================================

template<std::size_t reserved_size>
class MyBitSet {
    using word_type = uint64_t;
    static constexpr std::size_t bits_cnt = sizeof(word_type) * 8;
    static constexpr std::size_t bits_size = (reserved_size + bits_cnt - 1) / bits_cnt;
    word_type bits[bits_size]{};

    static_assert(reserved_size % bits_cnt == 0, "invalid reserved_size");

    void flip(int x) {
        bits[x / bits_cnt] ^= (uint64_t(1) << (x % bits_cnt));
    }

public:

    void insert(int x) {
        ASSERT(0 <= x && x < reserved_size, "invalid x");
        ASSERT(!contains(x), "x already insert");

        flip(x);
    }

    void erase(int x) {
        ASSERT(0 <= x && x < reserved_size, "invalid x");
        ASSERT(contains(x), "x already erase");

        flip(x);
    }

    bool contains(int x) const {
        ASSERT(0 <= x && x < reserved_size, "invalid x");
        return (bits[x / bits_cnt] >> (x % bits_cnt)) & 1;
    }

    bool empty() const {
        for (int i = 0; i < bits_size; i++) {
            if (bits[i] != 0) {
                return false;
            }
        }
        return true;
    }

    int size() const {
        int sz = 0;
        for (int i = 0; i < bits_size; i++) {
            sz += __builtin_popcountll(bits[i]);
        }
        return sz;
    }

    class Iterator {
        int x = 0;
        const MyBitSet &object;

    public:

        using difference_type = std::ptrdiff_t;
        using value_type = int;
        using pointer = int;
        using reference = int;
        using iterator_category = std::forward_iterator_tag;

        Iterator(int X, const MyBitSet &OBJECT)
                : x(X), object(OBJECT) {
        }

        Iterator &operator++() {
            if (x == reserved_size) {
                return *this;
            }

            uint64_t val = object.bits[x / bits_cnt] >> (x % bits_cnt);
            val ^= 1;
            if (val == 0) {
                x += bits_cnt - x % bits_cnt;

                while (x / bits_cnt < bits_size) {
                    uint64_t val = object.bits[x / bits_cnt] >> (x % bits_cnt);
                    if (val == 0) {
                        x += bits_cnt - x % bits_cnt;
                    } else {
                        x += __builtin_ctzll(val);
                        break;
                    }
                }
            } else {
                x += __builtin_ctzll(val);
            }

            return *this;
        }

        const int operator*() const {
            return x;
        }

        friend bool
        operator==(const Iterator &lhs, const Iterator &rhs) {
            return lhs.x == rhs.x && &lhs.object == &rhs.object;
        }

        friend bool
        operator!=(const Iterator &lhs, const Iterator &rhs) {
            return !(lhs == rhs);
        }
    };

    Iterator begin() {
        int x = 0;
        while (x < bits_size && bits[x] == 0) {
            x++;
        }
        x = x * bits_cnt + (x != bits_size ? __builtin_ctzll(bits[x]) : 0);
        return Iterator(x, *this);
    }

    Iterator end() {
        return Iterator(reserved_size, *this);
    }
};

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

    vector <Interval> reservedRBs(K);
    for (auto &[start, end, users]: reservedRBs) {
        input >> start >> end;
    }

    vector <UserInfo> userInfos(N);
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

        /// TODO: down
        //ASSERT(user_min[u] == 1e9 || user_score[u] == user_max[u] - user_min[u],
        //       "answer interval is invalid: user have no continuous interval");
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

vector<vector<Interval>> Solver_artem(int N, int M, int K, int J, int L,
                                      vector<Interval> &reservedRBs,
                                      vector<UserInfo> &userInfos, float coef
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

    vector <std::vector<Interval>> pre_answer(free_spaces.size());
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
            // РІС‹Р±СЂР°С‚СЊ РЅР°РёР±РѕР»СЊС€РёР№ РІРѕР·РјРѕР¶РЅС‹Р№, РѕС‚СЂРµР·Р°С‚СЊ РѕС‚ РЅРµРіРѕ РєСѓСЃРѕРє TARGET_LEN;
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
            int can_cut = selected_size; // РµСЃР»Рё СЌС‚Рѕ РЅРµ РїРѕСЃР»РµРґРЅРёР№ РѕС‚СЂРµР·Р°РµРјС‹Р№ РєСѓСЃРѕРє. РўРѕРіРґР° РѕС‚СЂРµР·Р°РµРј РІСЃС‘ С‡С‚Рѕ РµСЃС‚СЊ
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
    std::set<pair < int, int>, std::greater<>>
    space_left_q;
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
        // СЋР·Р°РµРј OWNEDBY РґРѕ РєРѕРЅС†Р°!!!
        // СЃС‡РёС‚Р°РµРј РєРѕР»-РІРѕ СЃРІРѕР±РѕРґРЅС‹С… СЃР»РѕС‚РѕРІ.

        // РќР°Р±РёСЂР°РµРј
        //-------------------------
        std::vector<std::pair<int, int>> candidates;
        std::set<int> to_delete;
        if (coef != -1.0) {
            for (auto user_id: activeUsers[pick_i]) {
                float curr_len = pre_answer[pick_i][current_sub_interval[pick_i]].end -
                                 pre_answer[pick_i][current_sub_interval[pick_i]].start;
                float need_more = userInfos[user_id].rbNeed - rbSuplied[user_id];
                if (need_more < curr_len * coef) {
                    to_delete.insert(user_id);
                }
            }


            int have_full = 0;
            for (auto &user: userInfos) {
                if (used_users.find(user.id) == used_users.end()) {
                    if (beamOwnedBy[pick_i][user.beam] == -1 &&
                        user.rbNeed - rbSuplied[user.id] > pre_answer[pick_i][current_sub_interval[pick_i]].end -
                                                           pre_answer[pick_i][current_sub_interval[pick_i]].start) {
                        have_full++;
                    }
                    if (beamOwnedBy[pick_i][user.beam] != -1) {
                        int owned_by_id = beamOwnedBy[pick_i][user.beam];
                        if (user.rbNeed - rbSuplied[user.id] > userInfos[owned_by_id].rbNeed - rbSuplied[owned_by_id]) {
                            have_full++;
                        }
                    }
                }
            }
            std::vector<pair < int, int>>
            to_delete_sorted;
            for (auto user_id: to_delete) {
                to_delete_sorted.push_back({userInfos[user_id].rbNeed - rbSuplied[user_id], user_id});
            }
            sort(to_delete_sorted.begin(), to_delete_sorted.end());
            if (have_full) {
                for (auto [coeffi, user_id]: to_delete_sorted) {
                    activeUsers[pick_i].erase(user_id);
                    beamOwnedBy[pick_i][userInfos[user_id].beam] = -1;
                    have_full--;
                    if (have_full == 0) {
                        break;
                    }
                }
            }
            candidates.clear();
        }
        for (auto &user: userInfos) {
            if (used_users.find(user.id) == used_users.end() && beamOwnedBy[pick_i][user.beam] == -1) {
                assert(rbSuplied[user.id] == 0);
                candidates.push_back({user.rbNeed - rbSuplied[user.id], user.id});
            }
        }
        //-----------------------




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

        to_delete.clear();
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

int
get_solution_score_light(int N, vector<vector<Interval>> &ans, vector<UserInfo> &userInfos, std::vector<int> &suplied) {
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
                      vector<Interval> &reservedRBs,
                      vector<UserInfo> &userInfos, vector<Interval> &solution, vector<int> &suplied, set<int> &empty) {

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
    std::vector<set<int>>
            mi_set(M);
    std::vector<set<int>>
            ma_set(M);
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
              vector<Interval> &reservedRBs,
              vector<UserInfo> &userInfos, vector<vector<Interval>> &solution) {

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


vector<Interval> Solver_Artem_grad(int N, int M, int K, int J, int L,
                                   vector<Interval> reservedRBs,
                                   vector<UserInfo> userInfos) {

    vector<int> suplied(N, 0);
    auto pre_answer = Solver_artem(N, M, K, J, L, reservedRBs, userInfos, 0.75);
    optimize(N, M, K, J, L, reservedRBs, userInfos, pre_answer);
    auto pre_answer2 = Solver_artem(N, M, K, J, L, reservedRBs, userInfos, -1.0);
    optimize(N, M, K, J, L, reservedRBs, userInfos, pre_answer2);
    auto fs = get_solution_score_light(N, pre_answer, userInfos, suplied);
    auto ss = get_solution_score_light(N, pre_answer2, userInfos, suplied);
    if (ss > fs) {
        pre_answer = pre_answer2;
    }
    vector <Interval> answer;
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

    struct SetInterval {
        // position
        int block = -1;
        int len = 0;

        // info
        MyBitSet<128> users;
        uint32_t beam_msk = 0;
    };

    vector<SetInterval> intervals;

    struct MyUserInfo {
        // user info
        int id = -1;
        int rbNeed = -1;
        int beam = -1;

        // score
        int sum_len = 0;

        int calc_score() const {
            if (sum_len > rbNeed) {
                return rbNeed;
            } else {
                return sum_len;
            }
        }
    };

    vector<MyUserInfo> users_info;

    // проходит по intervals
    // инициализирует .block так, чтобы длины не превышали free_interval
    // если .block = -1, то этот интервал никуда не влез
    void build_blocks() {
        int cur_block = 0;
        int cur_len = 0;
        for (int i = 0; i < intervals.size(); i++) {
            if (cur_block < free_intervals.size()) {
                if (cur_len + intervals[i].len > free_intervals[cur_block].len()) {
                    // не помещаемся
                    cur_block++;
                    cur_len = 0;
                    i--;
                } else {
                    cur_len += intervals[i].len;
                    intervals[i].block = cur_block;
                }
            } else {
                intervals[i].block = -1;
            }
        }
    }

    EgorTaskSolver(int NN, int MM, int KK, int JJ, int LL,
                   const vector<Interval> &reservedRBs,
                   const vector<UserInfo> &userInfos) : N(NN), M(MM), K(KK), J(JJ), L(LL) {

        users_info.resize(N);
        for (int u = 0; u < N; u++) {
            ASSERT(u == userInfos[u].id, "are you stupid or something?");
            ASSERT(0 <= userInfos[u].beam && userInfos[u].beam < 32, "invalid beam");
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
            int sum_free_len = 0;
            for (int block = 0; block < free_intervals.size(); block++) {
                sum_free_len += free_intervals[block].len();
            }
            int mean_len = sum_free_len / J;

            for (int j = 0; j < J; j++) {
                intervals.push_back(SetInterval{0, mean_len, {}});
            }
            build_blocks();
        }
    }

    [[nodiscard]] vector<Interval> get_total_answer() {
        vector <Interval> answer;
        int start = free_intervals[0].start;
        int cur_block = 0;
        for (int i = 0; i < intervals.size(); i++) {
            if (intervals[i].block == -1) {
                continue;
            }

            if (intervals[i].block != cur_block) {
                // new block
                cur_block = intervals[i].block;
                start = free_intervals[cur_block].start;
            }

            if (!intervals[i].users.empty() && intervals[i].len != 0) {
                answer.push_back({start, start + intervals[i].len, {}});
                for (int u: intervals[i].users) {
                    answer.back().users.push_back(u);
                }
                start += intervals[i].len;
            }
        }

        /*for (int block = 0; block < intervals.size(); block++) {
            int start = free_intervals[block].start;
            for (int interval = 0; interval < intervals[block].size(); interval++) {
                if (intervals[block][interval].end != 0 && !intervals[block][interval].users.empty()) {
                    answer.push_back({intervals[block][interval].start, intervals[block][interval].end, {}});
                    for (int u: intervals[block][interval].users) {
                        answer.back().users.push_back(u);
                    }
                    answer.back().start = start;
                    answer.back().end += start;
                    start = answer.back().end;
                }
            }
        }*/
        return answer;
    }

    ///===========================
    ///===========ACTIONS=========
    ///===========================

    int get_left_user(int u) {
        for (int i = 0; i < intervals.size(); i++) {
            if (intervals[i].users.contains(u)) {
                return i;
            }
        }
        return -1;
    }

    int get_right_user(int u) {
        for (int i = (int) intervals.size() - 1; i >= 0; i--) {
            if (intervals[i].users.contains(u)) {
                return i;
            }
        }
        return -1;
    }

    void change_interval_len(int interval, int change) {
        auto &[block, len, users, beam_msk] = intervals[interval];

        if (block != -1) {
            for (int u: users) {
                total_score -= users_info[u].calc_score();
                users_info[u].sum_len += change;
                total_score += users_info[u].calc_score();
            }
        }

        len += change;
        ASSERT(0 <= len, "invalid interval");

        int old_first_bad_block = intervals.size();
        for (int i = 0; i < intervals.size(); i++) {
            if (intervals[i].block == -1) {
                old_first_bad_block = i;
                break;
            }
        }

        build_blocks();

        while (0 < old_first_bad_block && intervals[old_first_bad_block - 1].block == -1) {
            old_first_bad_block--;

            for (int u: intervals[old_first_bad_block].users) {
                total_score -= users_info[u].calc_score();
                users_info[u].sum_len -= intervals[old_first_bad_block].len;
                total_score += users_info[u].calc_score();
            }
        }

        while (old_first_bad_block < intervals.size() && intervals[old_first_bad_block].block != -1) {
            for (int u: intervals[old_first_bad_block].users) {
                total_score -= users_info[u].calc_score();
                users_info[u].sum_len += intervals[old_first_bad_block].len;
                total_score += users_info[u].calc_score();
            }

            old_first_bad_block++;
        }
    }

    void add_user_in_interval(int u, int interval) {
        ASSERT(intervals[interval].users.size() + 1 <= L, "failed add");
        ASSERT(!intervals[interval].users.contains(u), "user already contains");
        ASSERT(!((intervals[interval].beam_msk >> users_info[u].beam) & 1), "equal beams");

        intervals[interval].users.insert(u);
        intervals[interval].beam_msk ^= (uint32_t(1) << users_info[u].beam);

        if (intervals[interval].block != -1) {
            total_score -= users_info[u].calc_score();
            users_info[u].sum_len += intervals[interval].len;
            total_score += users_info[u].calc_score();
        }
    }

    void remove_user_in_interval(int u, int interval) {
        ASSERT(intervals[interval].users.contains(u), "user no contains");

        intervals[interval].users.erase(u);
        intervals[interval].beam_msk ^= (uint32_t(1) << users_info[u].beam);

        if (intervals[interval].block != -1) {
            total_score -= users_info[u].calc_score();
            users_info[u].sum_len -= intervals[interval].len;
            total_score += users_info[u].calc_score();
        }
    }

    /*void add_right_interval_in_user(int u) {
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
    }*/

    ///==========================
    ///===========RANDOM=========
    ///==========================

    ///TEST CASE: K=0 | tests: 666 | score: 94.3685% | 646577/685162 | time: 10949.5ms | max_time: 66.43ms | mean_time: 16.4407ms
    ///TEST CASE: K=1 | tests: 215 | score: 92.6091% | 209102/225790 | time: 2541.55ms | max_time: 20.171ms | mean_time: 11.8212ms
    ///TEST CASE: K=2 | tests: 80 | score: 91.374% | 75993/83167 | time: 907.977ms | max_time: 14.678ms | mean_time: 11.3497ms
    ///TEST CASE: K=3 | tests: 39 | score: 90.7939% | 41984/46241 | time: 428.105ms | max_time: 15.848ms | mean_time: 10.9771ms
    ///TEST CASE: K=4 | tests: 0 | score: -nan% | 0/0 | time: 0ms | max_time: 0ms | mean_time: 0ms
    ///TOTAL: tests: 1000 | score: 93.5884% | 973656/1040360 | time: 14827.1ms | max_time: 66.43ms | mean_time: 14.8271ms

    double temperature = 1;

    bool is_good(int old_score) {
        return total_score > old_score || rnd.get_d() < exp((total_score - old_score) / temperature);
    }

    bool flow_over(int interval) {
        if (interval + 1 < intervals.size() && rnd.get_d() < 0.5) {
            int change = rnd.get(-intervals[interval].len, intervals[interval + 1].len);

            int old_score = total_score;

            if (change > 0) {
                change_interval_len(interval + 1, -change);
                change_interval_len(interval, change);
            } else {
                change_interval_len(interval, change);
                change_interval_len(interval + 1, -change);
            }

            if (is_good(old_score)) {
            } else {
                if (change > 0) {
                    change_interval_len(interval, -change);
                    change_interval_len(interval + 1, change);
                } else {
                    change_interval_len(interval + 1, change);
                    change_interval_len(interval, -change);
                }
                ASSERT(old_score == total_score, "failed back score");
            }

            return true;
        } else {
            return false;
        }
    }

    bool change_len(int interval) {
        if (rnd.get_d() < 0.5) {
            int change = max(-intervals[interval].len, (int) rnd.get(-10, 10));
            if (change == 0) {
                change = 1;
            }

            int old_score = total_score;

            change_interval_len(interval, change);

            if (is_good(old_score)) {
            } else {
                change_interval_len(interval, -change);
                ASSERT(old_score == total_score, "failed back score");
            }
            return true;
        } else {
            return false;
        }
    }

    bool update_interval() {
        if (rnd.get_d() < 0.3) {
            int interval = rnd.get(0, intervals.size() - 1);
            return flow_over(interval) || change_len(interval);
        } else {
            return false;
        }
    }

    bool flip_user_interval(int u) {
        if (rnd.get_d() < 0.5) {
            int interval = rnd.get(0, intervals.size() - 1);

            // no place
            if (!intervals[interval].users.contains(u) && intervals[interval].users.size() + 1 > L) {
                return false;
            }

            // там нет этого юзера, но уже есть одинаковый beam
            if (!intervals[interval].users.contains(u) && ((intervals[interval].beam_msk >> users_info[u].beam) & 1)) {
                return false;
            }

            auto flip = [&]() {
                if (intervals[interval].users.contains(u)) {
                    remove_user_in_interval(u, interval);
                } else {
                    add_user_in_interval(u, interval);
                }
            };

            int old_score = total_score;

            flip();

            if (is_good(old_score)) {
            } else {
                flip();
                ASSERT(old_score == total_score, "failed back score");
            }
            return true;
        } else {
            return false;
        }
    }

    bool new_user_interval(int u) {
        if (rnd.get_d() < 0.9) {
            vector<bool> okay(intervals.size());
            for (int i = 0; i < intervals.size(); i++) {
                okay[i] = !intervals[i].users.contains(u) &&
                          intervals[i].users.size() + 1 <= L &&
                          ((intervals[i].beam_msk >> users_info[u].beam) & 1) == 0;
            }

            vector<pair<int, int>> bounds;

            for (int left = 0; left < intervals.size(); left++) {
                int sum_len = 0;
                for (int right = left; right < intervals.size() && okay[right] &&
                                       intervals[left].block == intervals[right].block; right++) {
                    bounds.emplace_back(left, right);
                }
            }

            if (bounds.empty()) {
                return false;
            }

            auto [left, right] = bounds[rnd.get(0, bounds.size() - 1)];

            int old_score = total_score;

            for(int i = left; i <= right; i++){
                add_user_in_interval(u, i);
            }

            if (is_good(old_score)) {
            } else {
                for(int i = left; i <= right; i++){
                    remove_user_in_interval(u, i);
                }
                ASSERT(old_score == total_score, "failed back score");
            }

            return true;
        } else {
            return false;
        }
    }

    bool user_truncate(int u){
        if(rnd.get_d() < 0){
            return true;
        }
        else{
            return false;
        }
    }

    int current_user = -1;

    vector<int> users_order;

    bool update_user() {
        if (rnd.get_d() < 0.6) {
            int u = current_user;//rnd.get(0, N - 1);
            current_user = (current_user + 1) % N;
            return flip_user_interval(u) || user_truncate(u) || new_user_interval(u);
        } else {
            return false;
        }
    }

    vector<Interval> annealing(const vector<Interval> &reservedRBs,
                               const vector<UserInfo> &userInfos) {
        temperature = 1;

        {
            current_user = 0;
            users_order.resize(N);
            iota(users_order.begin(), users_order.end(), 0);
            sort(users_order.begin(), users_order.end(), [&](int lhs, int rhs) {
                return users_info[lhs].rbNeed > users_info[rhs].rbNeed;
            });
        }

        vector <Interval> answer = get_total_answer();
        int answer_score = total_score;

        for (int step = 0; step < 100'000; step++) {
            temperature *= 0.99999;

            if (answer_score < total_score) {
                answer_score = total_score;
                answer = get_total_answer();
            }

            if (update_interval() || update_user()) {

            }
        }

        ASSERT(get_solution_score(N, M, K, J, L, reservedRBs, userInfos, answer) == answer_score,
               "failed answer score");

        return answer;
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
    auto answer = solver.annealing(reservedRBs, userInfos);
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
