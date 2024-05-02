// TIME OPTIMIZATION
// 3375.92ms ->

int CNT_CALL_GET_LEFT_USER = 0;
int CNT_CALL_GET_RIGHT_USER = 0;

int CNT_CALL_CHANGE_INTERVAL_LEN = 0;

int CNT_CALL_ADD_USER_IN_INTERVAL = 0;
int CNT_CALL_REMOVE_USER_IN_INTERVAL = 0;

int CNT_CALL_INTERVAL_FLOW_OVER = 0;
int CNT_ACCEPTED_INTERVAL_FLOW_OVER = 0;
int CNT_CALL_INTERVAL_INCREASE_LEN = 0;
int CNT_ACCEPTED_INTERVAL_INCREASE_LEN = 0;
int CNT_CALL_INTERVAL_DECREASE_LEN = 0;
int CNT_ACCEPTED_INTERVAL_DECREASE_LEN = 0;
int CNT_CALL_INTERVAL_GET_FULL_FREE_SPACE = 0;
int CNT_ACCEPTED_INTERVAL_GET_FULL_FREE_SPACE = 0;
int CNT_CALL_INTERVAL_DO_MERGE_EQUAL = 0;
int CNT_CALL_INTERVAL_DO_SPLIT = 0;
int CNT_CALL_INTERVAL_MERGE_EQUAL = 0;
int CNT_ACCEPTED_INTERVAL_MERGE_EQUAL = 0;

int CNT_CALL_USER_NEW_INTERVAL = 0;
int CNT_ACCEPTED_USER_NEW_INTERVAL = 0;
int CNT_CALL_USER_ADD_LEFT = 0;
int CNT_ACCEPTED_USER_ADD_LEFT = 0;
int CNT_CALL_USER_REMOVE_LEFT = 0;
int CNT_ACCEPTED_USER_REMOVE_LEFT = 0;
int CNT_CALL_USER_ADD_RIGHT = 0;
int CNT_ACCEPTED_USER_ADD_RIGHT = 0;
int CNT_CALL_USER_REMOVE_RIGHT = 0;
int CNT_ACCEPTED_USER_REMOVE_RIGHT = 0;
int CNT_CALL_USER_DO_CROP = 0;
int CNT_CALL_USER_CROP = 0;
int CNT_ACCEPTED_USER_CROP = 0;
int CNT_CALL_USER_SWAP = 0;
int CNT_ACCEPTED_USER_SWAP = 0;

// ===================================================================================================================
// =========================ASSERT====================================================================================
// ===================================================================================================================
#include <iostream>

///////// !!!
//#define MY_DEBUG_MODE
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


    friend bool operator==(const MyBitSet &lhs, const MyBitSet &rhs) {
        for (int i = 0; i < bits_size; i++) {
            if (lhs.bits[i] != rhs.bits[i]) {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const MyBitSet &lhs, const MyBitSet &rhs) {
        return !(lhs == rhs);
    }
};

// ===================================================================================================================
// =========================TIME_ACCUM_WRAPPER========================================================================
// ===================================================================================================================

struct TimeAccumWrapper {
    static inline double time_accum = 0;
    Timer timer;

    TimeAccumWrapper() {
    }

    ~TimeAccumWrapper() {
        time_accum += timer.get();
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

int get_theory_max_score(int N, int M, int K, int J, int L, const vector<Interval> &reservedRBs,
                         const vector<UserInfo> &userInfos) {
    int max_score = 0;
    std::vector<int> rbNeeded;
    std::vector<pair<int, int>>
            reserved;
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
        rbNeeded.push_back(min(user.rbNeed, max_res_len));
    }
    sort(rbNeeded.begin(), rbNeeded.end(), greater<>());
    for (int i = 0; i < min(N, J * L); i++) {
        max_score += rbNeeded[i];
    }

    int max_possible = M * L;
    for (const auto &reserved: reservedRBs) {
        max_possible -= (reserved.end - reserved.start) * L;
    }

    return min(max_score, max_possible);
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

void recut(std::vector<Interval> &intervals, int last_k, int to) {
    int s = 0;
    for (int i = max(0, (int) intervals.size() - last_k); i < intervals.size(); i++) {
        s += intervals[i].end - intervals[i].start;
    }
    for (int i = 0; i < last_k; i++) {
        assert(intervals.back().users.empty());
        intervals.pop_back();
    }
    int start = 0;
    if (intervals.size()) {
        start = intervals.back().end;
    }
    assert(intervals.size());
    intervals.pop_back();
    int more = s;
    int new_size = s / to;
    int pref = start;
    for (int i = 0; i < to; i++) {
        int put = new_size;
        if (i + 1 == to) {
            put = more;
        }
        intervals.push_back({pref, pref + put, {}});
        more -= put;
        pref += put;
    }
}

vector<vector<Interval>> Solver_artem(int N, int M, int K, int J, int L,
                                      vector<Interval> &reservedRBs,
                                      vector<UserInfo> &userInfos, float coef, float coef2 = 1.0) {
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
        int TARGET_LEN = total_free_space_size / J * coef2;
        std::vector<int> free_spaces_seperation_starts;
        for (auto free_space: free_spaces) {
            free_spaces_seperation_starts.push_back(free_space.start);
        }
        for (auto j = 0; j < J; j++) {
            // РІС‹Р±СЂР°С‚СЊ РЅР°РёР±РѕР»СЊС€РёР№ РІРѕР·РјРѕР¶РЅС‹Р№, РѕС‚СЂРµР·Р°С‚СЊ РѕС‚ РЅРµРіРѕ РєСѓСЃРѕРє TARGET_LEN;
            int selected_index = -1;
            int selected_size = -1;
            int selected_size_fake = -1;
            for (int i = 0; i < free_spaces.size(); i++) {
                int current_possible_len = free_spaces[i].end - free_spaces_seperation_starts[i];
                int current_size_fake = current_possible_len;
                if (free_spaces_seperation_starts[i] == free_spaces[i].start) {
                    current_size_fake *= 1;
                }
                if (current_size_fake > selected_size_fake) {
                    selected_size_fake = current_size_fake;
                    selected_size = current_possible_len;
                    selected_index = i;
                }
            }
            assert(selected_index != -1);
            int can_cut = selected_size;// РµСЃР»Рё СЌС‚Рѕ РЅРµ РїРѕСЃР»РµРґРЅРёР№ РѕС‚СЂРµР·Р°РµРјС‹Р№ РєСѓСЃРѕРє. РўРѕРіРґР° РѕС‚СЂРµР·Р°РµРј РІСЃС‘ С‡С‚Рѕ РµСЃС‚СЊ
            if (j + 1 != J) {
                can_cut = std::min(selected_size, TARGET_LEN);
            }
            int currect_start = free_spaces_seperation_starts[selected_index];
            pre_answer[selected_index].push_back({currect_start, currect_start + can_cut, {}});
            free_spaces_seperation_starts[selected_index] += can_cut;
        }
        for (int i = 0; i < pre_answer.size(); i++) {
            int s = 0;
            for (int g = 0; g < pre_answer[i].size(); g++) {
                s += pre_answer[i][g].end - pre_answer[i][g].start;
            }

            if (pre_answer[i].size()) {
                int more = free_spaces[i].end - free_spaces[i].start - s;
                int give_to_one = more / pre_answer[i].size();
                int nakop = 0;
                for (int g = 0; g < pre_answer[i].size(); g++) {
                    int give = give_to_one;
                    if (g + 1 == pre_answer[i].size()) {
                        give = more - give_to_one * (pre_answer[i].size() - 1);
                    }
                    pre_answer[i][g].start += nakop;
                    pre_answer[i][g].end += nakop + give;
                    nakop += give;
                }
                //pre_answer[i].back().end+=free_spaces[i].end-free_spaces[i].start-s;
            }
        }
    }
    std::vector<int> rbSuplied(N, 0);
    std::set<int> used_users;
    int current_interval_iter = 0;
    std::set<pair<int, int>, std::greater<>>
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
    //    cout << "-------------------" << endl;
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
            std::vector<pair<int, int>>
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
        if (coef == -2.0) {
            while (true) {
                int ma_not_empty = -1;
                int int_len = pre_answer[pick_i][current_sub_interval[pick_i]].end -
                              pre_answer[pick_i][current_sub_interval[pick_i]].start;
                for (auto user_id: activeUsers[pick_i]) {
                    int need = userInfos[user_id].rbNeed;
                    int now = rbSuplied[user_id];
                    assert(need - now > 0);
                    if (need - now < int_len) {
                        ma_not_empty = max(ma_not_empty, need - now);
                    }
                }
                if (activeUsers[pick_i].empty()) {
                    break;
                }
                if (ma_not_empty != -1 && current_sub_interval[pick_i] + 1 != pre_answer[pick_i].size()) {
                    ma_not_empty = max(ma_not_empty, 1);
                    int give = int_len - ma_not_empty;
                    pre_answer[pick_i][current_sub_interval[pick_i]].end -= give;
                    pre_answer[pick_i][current_sub_interval[pick_i] + 1].start -= give;
                } else if (ma_not_empty == -1 && current_sub_interval[pick_i] + 2 < pre_answer[pick_i].size()) {
                    int curr_index = current_sub_interval[pick_i];
                    int end = pre_answer[pick_i][curr_index + 1].end;
                    int more = pre_answer[pick_i].size() - current_sub_interval[pick_i] - 2;
                    if ((pre_answer[pick_i].back().end - pre_answer[pick_i][curr_index + 2].start) / (more + 1) == 0) {
                        break;
                    }
                    pre_answer[pick_i][curr_index].end = end;
                    recut(pre_answer[pick_i], more, more + 1);

                    continue;
                }
                break;
            }
        }

        for (auto user_id: activeUsers[pick_i]) {
            rbSuplied[user_id] += pre_answer[pick_i][current_sub_interval[pick_i]].end -
                                  pre_answer[pick_i][current_sub_interval[pick_i]].start;
            pre_answer[pick_i][current_sub_interval[pick_i]].users.push_back(user_id);
            if (rbSuplied[user_id] >= userInfos[user_id].rbNeed) {
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

int get_solution_score_light(int N, vector<vector<Interval>> &ans, const vector<UserInfo> &userInfos, std::vector<int> &suplied) {
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
                      const vector<UserInfo> &userInfos, vector<Interval> &solution, vector<int> &suplied, set<int> &empty) {

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
            if (mi[i] != 10000 && mi[i] != 0) {// можно сделать так что бь mi == 0
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
                    //                    cerr << "+" << best_score_gain - minus << endl;
                    if (is_empty_winner) {
                        empty.erase(best_receiver);
                        //cerr << "IS EMPTY" << endl;
                    }
                    //                    cerr << "OPTIMIZING1" << endl;
                    //                    cerr << "PUTTING" << best_receiver << "(" << userInfos[best_receiver].beam << ")" << " instead of " << i << "(" << userInfos[i].beam << ")" << " " << mi[i] << endl;
                    auto iter = find(solution[mi[i]].users.begin(), solution[mi[i]].users.end(), i);
                    solution[mi[i]].users.erase(iter);// optimize_it
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
            if (ma[i] != -10000 && ma[i] + 1 != solution.size()) {// можно сделать так что бь mi == 0
                int best_score_gain = -1;
                int best_receiver = -1;
                int current_len = solution[ma[i]].end - solution[ma[i]].start;
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
                    //                    cerr << "+" << best_score_gain << "-" <<  minus << endl;
                    //                    cerr << "OPTIMIZING2" << endl;
                    if (is_empty_winner) {
                        empty.erase(best_receiver);
                        //cerr << "EMPTY" << endl;
                    }
                    //                    cerr << "PUTTING" << best_receiver << "(" << userInfos[best_receiver].beam << ")" << " instead of " << i << "(" << userInfos[i].beam << ")" << " " << mi[i] << endl;
                    auto iter = find(solution[ma[i]].users.begin(), solution[ma[i]].users.end(), i);
                    solution[ma[i]].users.erase(iter);// optimize_it
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


vector<Interval> Solver_Artem_grad(int N, int M, int K, int J, int L,
                                   vector<Interval> reservedRBs,
                                   vector<UserInfo> userInfos) {

    vector<int> suplied(N, 0);
    std::vector<std::vector<std::vector<Interval>>> anses;
    anses.push_back(Solver_artem(N, M, K, J, L, reservedRBs, userInfos, -2.0));
    //    anses.push_back(Solver_artem(N, M, K, J, L, reservedRBs, userInfos, -2.0, 0.75));
    //    anses.push_back(Solver_artem(N, M, K, J, L, reservedRBs, userInfos, -2.0, 0.5));
    //    anses.push_back(Solver_artem(N, M, K, J, L, reservedRBs, userInfos, -2.0, 0.3));


    //    anses.push_back(Solver_artem(N, M, K, J, L, reservedRBs, userInfos, -2.0, 0.7));
    //    anses.push_back(Solver_artem(N, M, K, J, L, reservedRBs, userInfos, -2.0, 0.6));
    int biggest_score = -1;
    int biggest_index = -1;
    for (int i = 0; i < anses.size(); i++) {
        optimize(N, M, K, J, L, reservedRBs, userInfos, anses[i]);
        int score = get_solution_score_light(N, anses[i], userInfos, suplied);
        if (score > biggest_score) {
            biggest_score = score;
            biggest_index = i;
        }
    }
    std::vector<vector<Interval>> pre_answer = anses[biggest_index];
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

    int theor_max = 0;
    int total_score = 0;

    struct SetInterval {
        // position
        int block = -1;
        int len = 0;

        // info
        MyBitSet<128> users;
        uint32_t beam_msk = 0;

        friend bool operator==(const SetInterval &lhs, const SetInterval &rhs) {
            return lhs.block == rhs.block &&
                   lhs.len == rhs.len &&
                   lhs.users == rhs.users &&
                   lhs.beam_msk == rhs.beam_msk;
        }
    };

    vector<SetInterval> intervals;

    struct MyUserInfo {
        // position
        int left = -1, right = -1;

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

    vector<vector<int>> users_with_equal_beam;
    vector<int> user_id_to_u;

    // проходит по intervals
    // инициализирует .block так, чтобы длины не превышали free_interval
    // если .block = -1, то этот интервал никуда не влез

    // 271.944ms -> 250.176ms
    void build_blocks() {
        ASSERT(intervals.size() == J, "kek");
        int cur_block = 0;
        int cur_len = 0;

        int i = 0;
        while (i < J) {
            if (cur_len + intervals[i].len > free_intervals[cur_block].len()) {
                // не помещаемся
                cur_block++;
                if (cur_block == free_intervals.size()) {
                    break;
                }
                cur_len = 0;
                i--;
            } else {
                cur_len += intervals[i].len;
                intervals[i].block = cur_block;
            }
            i++;
        }
        while (i < J) {
            intervals[i].block = -1;
            i++;
        }

        /*for (int i = 0; i < intervals.size(); i++) {
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
        }*/
    }

    EgorTaskSolver(int NN, int MM, int KK, int JJ, int LL,
                   const vector<Interval> &reservedRBs,
                   const vector<UserInfo> &userInfos) : N(NN), M(MM), K(KK), J(JJ), L(LL) {

        ASSERT(2 <= L && L <= 16, "kek");

        theor_max = get_theory_max_score(N, M, K, J, L, reservedRBs, userInfos);

        users_info.resize(N);
        user_id_to_u.resize(N);
        users_with_equal_beam.resize(32);
        for (int u = 0; u < N; u++) {
            ASSERT(u == userInfos[u].id, "are you stupid or something?");
            ASSERT(0 <= userInfos[u].beam && userInfos[u].beam < 32, "invalid beam");
            users_info[u].id = userInfos[u].id;
            users_info[u].rbNeed = userInfos[u].rbNeed;
            users_info[u].beam = userInfos[u].beam;

            users_with_equal_beam[users_info[u].beam].push_back(users_info[u].id);
            user_id_to_u[users_info[u].id] = u;
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
        //        {
        //            int sum_free_len = 0;
        //            for (int block = 0; block < free_intervals.size(); block++) {
        //                sum_free_len += free_intervals[block].len();
        //            }
        //            int mean_len = sum_free_len / J;
        //
        //            for (int j = 0; j < J; j++) {
        //                intervals.push_back(SetInterval{0, mean_len, {}});
        //            }
        //            build_blocks();
        //        }
    }

    [[nodiscard]] vector<Interval> get_total_answer() {
        vector<Interval> answer;
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
                    answer.back().users.push_back(users_info[u].id);
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
        return users_info[u].left;
        CNT_CALL_GET_LEFT_USER++;
        for (int i = 0; i < J; i++) {
            if (intervals[i].users.contains(u)) {
                //if(i != users_info[u].left) {
                //    return i;
                //}
                ASSERT(i == users_info[u].left, "failed calc left");
                return i;
            }
        }
        ASSERT(-1 == users_info[u].left, "failed calc left");
        return -1;
    }

    int get_right_user(int u) {
        return users_info[u].right;
        CNT_CALL_GET_RIGHT_USER++;
        for (int i = J - 1; i >= 0; i--) {
            if (intervals[i].users.contains(u)) {
                //if(i != users_info[u].right) {
                //    return i;
                //}
                ASSERT(i == users_info[u].right, "failed calc right");
                return i;
            }
        }
        ASSERT(-1 == users_info[u].right, "failed calc right");
        return -1;
    }

    void change_interval_len(int interval, int change) {
        //TimeAccumWrapper _;
        CNT_CALL_CHANGE_INTERVAL_LEN++;
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

    void add_user_in_interval(int u, int i) {
        CNT_CALL_ADD_USER_IN_INTERVAL++;

        auto &interval = intervals[i];
        auto &user = users_info[u];

        ASSERT(interval.users.size() + 1 <= L, "failed add");
        ASSERT(!interval.users.contains(u), "user already contains");
        ASSERT(!((interval.beam_msk >> user.beam) & 1), "equal beams");

        interval.users.insert(u);
        interval.beam_msk ^= (uint32_t(1) << user.beam);

        if (user.left == -1) {
            user.left = user.right = i;
        } else {
            user.left = min(user.left, i);
            user.right = max(user.right, i);
        }

        ASSERT(user.left == get_left_user(u), "failed left");
        ASSERT(user.right == get_right_user(u), "failed right");

        if (interval.block != -1) {
            total_score -= user.calc_score();
            user.sum_len += interval.len;
            total_score += user.calc_score();
        }
    }

    void remove_user_in_interval(int u, int i) {
        CNT_CALL_REMOVE_USER_IN_INTERVAL++;

        auto &interval = intervals[i];
        auto &user = users_info[u];

        ASSERT(interval.users.contains(u), "user no contains");

        interval.users.erase(u);
        interval.beam_msk ^= (uint32_t(1) << user.beam);

        if (user.left == i) {
            user.left++;
        } else if (user.right == i) {
            user.right--;
        }
        if (user.left > user.right) {
            user.left = user.right = -1;
        }

        if(user.left != get_left_user(u)) {
            cout << "FUCK" << endl;
            return;
        }

        ASSERT(user.left == get_left_user(u), "failed left");
        ASSERT(user.right == get_right_user(u), "failed right");

        if (interval.block != -1) {
            total_score -= user.calc_score();
            user.sum_len -= interval.len;
            total_score += user.calc_score();
        }
    }

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

    ///==========================
    ///===========INTERVAL=======
    ///==========================

#define CHOOSE_INTERVAL(condition)                                        \
    int interval;                                                         \
    {                                                                     \
        vector<int> ips;                                                  \
        for (int interval = 0; interval < intervals.size(); interval++) { \
            if (condition) {                                              \
                ips.push_back(interval);                                  \
            }                                                             \
        }                                                                 \
        if (ips.empty()) {                                                \
            return;                                                       \
        }                                                                 \
        interval = ips[rnd.get(0, ips.size() - 1)];                       \
    }

    void interval_flow_over() {
        CNT_CALL_INTERVAL_FLOW_OVER++;

        CHOOSE_INTERVAL(interval + 1 < intervals.size());

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
            CNT_ACCEPTED_INTERVAL_FLOW_OVER++;
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
    }

    /*void interval_decrease_len() {
        CNT_CALL_INTERVAL_CHANGE_LEN++;

        CHOOSE_INTERVAL(true);

        int change = max(-intervals[interval].len, (int) rnd.get(-5, 10));
        if (change == 0) {
        change = 1;
        }

        int old_score = total_score;

        change_interval_len(interval, change);

        if (is_good(old_score)) {
        CNT_ACCEPTED_INTERVAL_CHANGE_LEN++;
        } else {
        change_interval_len(interval, -change);
        ASSERT(old_score == total_score, "failed back score");
        }
    }*/

    void interval_increase_len() {
        CNT_CALL_INTERVAL_INCREASE_LEN++;

        CHOOSE_INTERVAL(true);

        int change = rnd.get(1, 10);

        int old_score = total_score;

        change_interval_len(interval, change);

        if (is_good(old_score)) {
            CNT_ACCEPTED_INTERVAL_INCREASE_LEN++;
        } else {
            change_interval_len(interval, -change);
            ASSERT(old_score == total_score, "failed back score");
        }
    }

    void interval_decrease_len() {
        CNT_CALL_INTERVAL_DECREASE_LEN++;

        CHOOSE_INTERVAL(intervals[interval].len > 0);

        int change = rnd.get(-intervals[interval].len, -1);

        int old_score = total_score;

        change_interval_len(interval, change);

        if (is_good(old_score)) {
            CNT_ACCEPTED_INTERVAL_DECREASE_LEN++;
        } else {
            change_interval_len(interval, -change);
            ASSERT(old_score == total_score, "failed back score");
        }
    }

    void interval_get_full_free_space(int interval) {
        CNT_CALL_INTERVAL_GET_FULL_FREE_SPACE++;
        int block = intervals[interval].block;
        if (block == -1) {
            return;
        }

        int len = 0;
        for (int i = 0; i < intervals.size(); i++) {
            if (intervals[i].block == block) {
                len += intervals[i].len;
            }
        }

        int change = free_intervals[block].len() - len;
        ASSERT(change >= 0, "what?");
        if (change != 0) {
            int old_score = total_score;

            change_interval_len(interval, change);

            if (is_good(old_score)) {
                CNT_ACCEPTED_INTERVAL_GET_FULL_FREE_SPACE++;
            } else {
                change_interval_len(interval, -change);
                ASSERT(old_score == total_score, "failed back score");
            }
        }
    }

    // merge interval and interval+1
    /*void interval_do_merge_equal(int interval) {
        CNT_CALL_INTERVAL_DO_MERGE_EQUAL++;

        //if (interval + 1 >= intervals.size() && intervals[interval].users == intervals[interval + 1].users &&
        //    intervals[interval].block == intervals[interval + 1].block) {
        //    return false;
        //}

        ASSERT(intervals[interval].users == intervals[interval + 1].users, "kek");
        ASSERT(intervals[interval].beam_msk == intervals[interval + 1].beam_msk, "kek");

        int old_score = total_score;

        int right_len = intervals[interval + 1].len;
        change_interval_len(interval + 1, -right_len);
        change_interval_len(interval, right_len);

        intervals.erase(intervals.begin() + interval + 1);
    }

    void interval_do_split(int interval, int right_len) {
        CNT_CALL_INTERVAL_DO_SPLIT++;
        intervals.insert(intervals.begin() + interval + 1, intervals[interval]);
        intervals[interval + 1].len = 0;

        change_interval_len(interval, -right_len);
        change_interval_len(interval + 1, right_len);
    }

    void interval_merge_equal() {
        CNT_CALL_INTERVAL_MERGE_EQUAL++;

        CHOOSE_INTERVAL(interval + 1 < intervals.size() && intervals[interval].users == intervals[interval + 1].users);

        int old_score = total_score;
        int right_len = intervals[interval + 1].len;

        interval_do_merge_equal(interval);

        intervals.push_back(SetInterval());

        if (is_good(old_score)) {
            CNT_ACCEPTED_INTERVAL_MERGE_EQUAL++;
        } else {
            intervals.pop_back();
            interval_do_split(interval, right_len);

            ASSERT(old_score == total_score, "failed back score");
        }
    }*/

    // downgrade score
    // 92% -> 9%
    /*void interval_split(int interval) {
        if (interval + 1 >= intervals.size()) {
            return;
        }

        auto old_intervals = intervals;

        int old_score = total_score;

        auto save_back = move(intervals.back());
        intervals.pop_back();

        int right_len = rnd.get(0, intervals[interval].len);

        interval_do_split(interval, right_len);

        ASSERT(intervals.size() == J, "kek");

        if (is_good(old_score)) {
        } else {
            interval_do_merge_equal(interval);
            intervals.push_back(save_back);

            ASSERT(old_score == total_score, "failed back score");
            ASSERT(intervals.size() == J, "kek");

            ASSERT(intervals == old_intervals, "?");
        }
    }*/

    void interval_random_action() {
        double p = rnd.get_d();
        if (p < 0.2) {
            interval_increase_len();
        } else if (p < 0.4) {
            interval_decrease_len();
        } else if (p < 0.6) {
            interval_flow_over();
        } else {
            //interval_merge_equal();
        } /* else {
            interval_get_full_free_space(interval);
        }*/
    }

    ///======================
    ///===========USER=======
    ///======================

    //TEST CASE: K=0 | tests: 666 | score: 94.3582% | 643894/682393 | time: 15929.8ms | max_time: 79.75ms | mean_time: 23.9186ms
    //TEST CASE: K=1 | tests: 215 | score: 93.0797% | 207861/223315 | time: 4812.53ms | max_time: 50.081ms | mean_time: 22.3839ms
    //TEST CASE: K=2 | tests: 80 | score: 92.573% | 75921/82012 | time: 2049.64ms | max_time: 51.105ms | mean_time: 25.6205ms
    //TEST CASE: K=3 | tests: 39 | score: 90.9903% | 41841/45984 | time: 995.188ms | max_time: 54.005ms | mean_time: 25.5176ms
    //TEST CASE: K=4 | tests: 0 | score: -nan% | 0/0 | time: 0ms | max_time: 0ms | mean_time: 0ms
    //TOTAL: tests: 1000 | score: 93.7906% | 969517/1033704 | time: 23787.2ms | max_time: 79.75ms | mean_time: 23.7872ms
    //EGOR TASK SOLVER STATISTIC:
    //CNT_CALL_GET_LEFT_USER: 863313
    //CNT_CALL_GET_RIGHT_USER: 868586
    //=================
    //CNT_CALL_CHANGE_INTERVAL_LEN: 33315.8
    //=================
    //CNT_CALL_ADD_USER_IN_INTERVAL: 31956.7
    //CNT_CALL_REMOVE_USER_IN_INTERVAL: 31867.2
    //=================
    //INTERVAL_FLOW_OVER: 35.0816% 2789/7951
    //INTERVAL_CHANGE_LEN: 38.5651% 2304/5976
    //INTERVAL_GET_FULL_FREE_SPACE: 7.69328% 307/4000
    //CNT_CALL_INTERVAL_DO_MERGE_EQUAL: 2026.78
    //CNT_CALL_INTERVAL_DO_SPLIT: 0
    //INTERVAL_MERGE_EQUAL: 19.2738% 390/2026
    //=================
    //USER_NEW_INTERVAL: 61.2332% 4972/8121
    //USER_ADD_LEFT: 12.9177% 1020/7903
    //USER_REMOVE_LEFT: 3.22802% 259/8035
    //USER_ADD_RIGHT: 78.7074% 6263/7957
    //USER_REMOVE_RIGHT: 8.96607% 723/8066
    //CNT_CALL_USER_DO_CROP: 5378.05
    //USER_CROP: 26.2043% 4202/16037
    //USER_SWAP: 194.268% 46567/23970

#define FIND_USER(condition)           \
    int u;                             \
    bool find = false;                 \
    for (int x: user_brute_order) {    \
        u = x;                         \
        int left = get_left_user(u);   \
        int right = get_right_user(u); \
        if (condition) {               \
            find = true;               \
            break;                     \
        }                              \
    }                                  \
    if (!find) {                       \
        return;                        \
    }

    int current_user = -1;

    vector<int> users_order, user_brute_order;

    // удаляет старые интервалы,
    // добавляет лучший новый
    void user_new_interval() {
        CNT_CALL_USER_NEW_INTERVAL++;
        vector<int> removed;

        int u = user_brute_order[current_user];//rnd.get(0, N - 1);
        current_user = (current_user + 1) % N;

        int old_score = total_score;

        vector<bool> okay(intervals.size());
        for (int i = 0; i < intervals.size(); i++) {
            if (intervals[i].users.contains(u)) {
                removed.push_back(i);
                remove_user_in_interval(u, i);
            }
            okay[i] = intervals[i].users.size() + 1 <= L &&
                      ((intervals[i].beam_msk >> users_info[u].beam) & 1) == 0;
        }

        ASSERT(get_left_user(u) == users_info[u].left, "failed left");
        ASSERT(get_right_user(u) == users_info[u].right, "failed right");

        int best_left = -1, best_right = -1, best_f = 1e9;
        for (int left = 0; left < intervals.size(); left++) {
            int sum_len = 0;
            for (int right = left; right < intervals.size() && okay[right] &&
                                   intervals[left].block == intervals[right].block &&
                                   intervals[left].block != -1;
                 right++) {
                sum_len += intervals[right].len;

                int cur_f = abs(sum_len - users_info[u].rbNeed);
                if (cur_f < best_f) {
                    best_f = cur_f;
                    best_left = left;
                    best_right = right;
                }
            }
        }

        if (best_left == -1) {
            return;
        }

        ASSERT(get_left_user(u) == users_info[u].left, "failed left");
        ASSERT(get_right_user(u) == users_info[u].right, "failed right");
        for (int i = best_left; i <= best_right; i++) {
            add_user_in_interval(u, i);
        }
        ASSERT(get_left_user(u) == users_info[u].left, "failed left");
        ASSERT(get_right_user(u) == users_info[u].right, "failed right");

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_NEW_INTERVAL++;
        } else {
            for (int i = best_left; i <= best_right; i++) {
                remove_user_in_interval(u, i);
            }
            ASSERT(get_left_user(u) == users_info[u].left, "failed left");
            ASSERT(get_right_user(u) == users_info[u].right, "failed right");
            for (int i: removed) {
                add_user_in_interval(u, i);
            }
            ASSERT(get_left_user(u) == users_info[u].left, "failed left");
            ASSERT(get_right_user(u) == users_info[u].right, "failed right");
            ASSERT(old_score == total_score, "failed back score");
        }
    }

    void user_add_left() {
        CNT_CALL_USER_ADD_LEFT++;

        FIND_USER(left > 0 &&
                  intervals[left - 1].users.size() < L &&
                  ((intervals[left - 1].beam_msk >> users_info[u].beam) & 1) == 0);

        int left = get_left_user(u);
        left--;

        int old_score = total_score;

        add_user_in_interval(u, left);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_ADD_LEFT++;
        } else {
            remove_user_in_interval(u, left);
            ASSERT(old_score == total_score, "failed back score");
        }
    }

    void user_remove_left() {
        CNT_CALL_USER_REMOVE_LEFT++;

        FIND_USER(left != -1);

        int left = get_left_user(u);

        int old_score = total_score;

        remove_user_in_interval(u, left);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_REMOVE_LEFT++;
        } else {
            add_user_in_interval(u, left);
            ASSERT(old_score == total_score, "failed back score");
        }
    }

    void user_add_right() {
        CNT_CALL_USER_ADD_RIGHT++;

        FIND_USER(right != -1 && right + 1 < intervals.size() &&
                  intervals[right + 1].users.size() < L &&
                  ((intervals[right + 1].beam_msk >> users_info[u].beam) & 1) == 0);

        int right = get_right_user(u);
        right++;

        int old_score = total_score;

        add_user_in_interval(u, right);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_ADD_RIGHT++;
        } else {
            remove_user_in_interval(u, right);
            ASSERT(old_score == total_score, "failed back score");
        }
    }

    void user_remove_right() {
        CNT_CALL_USER_REMOVE_RIGHT++;

        FIND_USER(right != -1);

        int right = get_right_user(u);

        int old_score = total_score;

        remove_user_in_interval(u, right);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_REMOVE_RIGHT++;
        } else {
            add_user_in_interval(u, right);
            ASSERT(old_score == total_score, "failed back score");
        }
    }

    vector<int> user_do_crop(int u) {
        CNT_CALL_USER_DO_CROP++;
        int best_left = -1, best_right = -1, best_len = -1e9;
        for (int left = 0; left < intervals.size(); left++) {
            int len = 0;
            for (int right = left; right < intervals.size() && intervals[right].users.contains(u) &&
                                   intervals[right].block == intervals[left].block &&
                                   intervals[right].block != -1;
                 right++) {

                len += intervals[right].len;

                if (best_len < len) {
                    best_len = len;
                    best_left = left;
                    best_right = right;
                }
            }
        }

        vector<int> removed;
        if (best_left == -1) {
            return removed;
        }
        for (int i = 0; i < best_left; i++) {
            if (intervals[i].users.contains(u)) {
                removed.push_back(i);
                remove_user_in_interval(u, i);
            }
        }
        for (int i = J - 1; i > best_right; i--) {
            if (intervals[i].users.contains(u)) {
                removed.push_back(i);
                remove_user_in_interval(u, i);
            }
        }
        //for (int i = 0; i < intervals.size(); i++) {
        //    if ((i < best_left || best_right < i) && intervals[i].users.contains(u)) {
        //        removed.push_back(i);
        //        remove_user_in_interval(u, i);
        //    }
        //}
        reverse(removed.begin(), removed.end());
        return removed;
    }

    // обрезает отрезок юзера до содержащегося только в одном блоке
    void user_crop() {
        CNT_CALL_USER_CROP++;

        FIND_USER(left != -1 && intervals[left].block != intervals[right].block);

        int old_score = total_score;

        auto removed = user_do_crop(u);

        ASSERT(!removed.empty(), "kek");

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_CROP++;
        } else {
            for (int i: removed) {
                add_user_in_interval(u, i);
            }
            ASSERT(old_score == total_score, "failed back score");
        }
    }

    void user_do_swap(int u, int u2) {
        total_score -= users_info[u].calc_score() + users_info[u2].calc_score();

        swap(user_id_to_u[users_info[u].id], user_id_to_u[users_info[u2].id]);
        swap(users_info[u].sum_len, users_info[u2].sum_len);
        swap(users_info[u].left, users_info[u2].left);
        swap(users_info[u].right, users_info[u2].right);
        swap(users_info[u], users_info[u2]);

        total_score += users_info[u].calc_score() + users_info[u2].calc_score();
    }

    void user_swap() {
        for (int beam = 0; beam < 32; beam++) {
            if (users_with_equal_beam[beam].size() >= 2) {
                // (sum_len, u)
                vector<pair<int, int>> ups;
                for (int u_id: users_with_equal_beam[beam]) {
                    int u = user_id_to_u[u_id];
                    ups.push_back({users_info[u].sum_len, u});
                    //for (int u2_id: users_with_equal_beam[beam]) {
                     //   if(u_id == u2_id) {
                     //       continue;
                     //   }
                     //   CNT_CALL_USER_SWAP++;
//
                     //   int u = user_id_to_u[u_id];
                     //   int u2 = user_id_to_u[u2_id];
//
                     //   int old_score = total_score;
//
                     //   user_do_swap(u, u2);
////
                     //   if (is_good(old_score)) {
                     //      CNT_ACCEPTED_USER_SWAP++;
                     //   } else {
                     //       user_do_swap(u, u2);
                     //       ASSERT(old_score == total_score, "failed back score");
                     //   }
                    //}
                }
                sort(ups.begin(), ups.end());
                for (int i = 0; i + 1 < ups.size(); i++) {
                    CNT_CALL_USER_SWAP++;

                    int &u = ups[i].second;
                    int &u2 = ups[i + 1].second;

                    int old_score = total_score;

                    user_do_swap(u, u2);

                    if (is_good(old_score)) {
                        CNT_ACCEPTED_USER_SWAP++;
                    } else {
                        user_do_swap(u, u2);
                        ASSERT(old_score == total_score, "failed back score");
                    }
                }
            }
        }

        return;

        //int u = -1, u2 = -1;
        for (int beam = 0; beam < 32; beam++) {
            if (users_with_equal_beam[beam].size() >= 2) {
                for (int steps = 0; steps < 1; steps++) {
                    CNT_CALL_USER_SWAP++;
                    int u = user_id_to_u[users_with_equal_beam[beam][rnd.get(0, users_with_equal_beam[beam].size() - 1)]];
                    int u2 = user_id_to_u[users_with_equal_beam[beam][rnd.get(0, users_with_equal_beam[beam].size() - 1)]];
                    //for (int u_id: users_with_equal_beam[beam]) {
                    //for (int u2_id: users_with_equal_beam[beam]) {


                    if (u != u2) {

                        int old_score = total_score;

                        user_do_swap(u, u2);

                        if (is_good(old_score)) {
                            CNT_ACCEPTED_USER_SWAP++;
                        } else {
                            user_do_swap(u, u2);
                            ASSERT(old_score == total_score, "failed back score");
                        }
                    }
                }
            }
        }
    }

    void user_random_action() {
        //  94.519%
        double p = rnd.get_d();
        if (p < 0.6) {
            user_new_interval();
        } /*else if (p < 0.2) {
            user_add_left();
        } */
        else if (p < 0.8) {
            user_add_right();
        } /*else if (p < 0.4) {
            user_remove_left();
        } */
        else if (p < 0.8) {
            user_remove_right();
        } else if (p < 0.9) {
            user_swap();
        } else {
            user_crop();
        }
    }

    vector<Interval> annealing(vector<Interval> start) {
        temperature = 1;

        {
            current_user = 0;
            users_order.resize(N);
            iota(users_order.begin(), users_order.end(), 0);
            sort(users_order.begin(), users_order.end(), [&](int lhs, int rhs) {
                return users_info[lhs].rbNeed > users_info[rhs].rbNeed;
            });

            user_brute_order = users_order;
        }

        sort(start.begin(), start.end(), [&](const auto &lhs, const auto &rhs) {
            return lhs.start < rhs.start;
        });

        ASSERT(intervals.empty(), "NOT EMPTY INTERVALS BAD BAD");
        for (int i = 0; i < start.size(); i++) {
            intervals.push_back(SetInterval{-1, start[i].end - start[i].start, {}});
        }
        while (intervals.size() < J) {
            intervals.push_back(SetInterval{-1, 0, {}});
        }
        build_blocks();

        for (int i = 0; i < start.size(); i++) {
            for (auto user_id: start[i].users) {
                add_user_in_interval(user_id, i);
            }
        }
        //        for (int i = 0; i < N; i++) {
        //            user_new_interval();
        //        }

        //vector <Interval> answer = get_total_answer();
        //int answer_score = total_score;

        //TEST CASE: K=0 | tests: 666 | score: 94.7412% | 646507/682393 | time: 5647.33ms | max_time: 31.324ms | mean_time: 8.47947ms
        //TEST CASE: K=1 | tests: 215 | score: 94.1612% | 210276/223315 | time: 1644.51ms | max_time: 21.472ms | mean_time: 7.64887ms
        //TEST CASE: K=2 | tests: 80 | score: 93.5522% | 76724/82012 | time: 780.929ms | max_time: 25.218ms | mean_time: 9.76161ms
        //TEST CASE: K=3 | tests: 39 | score: 94.0436% | 43245/45984 | time: 405.57ms | max_time: 24.404ms | mean_time: 10.3992ms
        //TEST CASE: K=4 | tests: 0 | score: -nan% | 0/0 | time: 0ms | max_time: 0ms | mean_time: 0ms
        //TOTAL: tests: 1000 | score: 94.4905% | 976752/1033704 | time: 8478.33ms | max_time: 31.324ms | mean_time: 8.47833ms
        //EGOR TASK SOLVER STATISTIC:
        //CNT_CALL_GET_LEFT_USER: 35403.3
        //CNT_CALL_GET_RIGHT_USER: 36244.1
        //=================
        //CNT_CALL_CHANGE_INTERVAL_LEN: 6416.52
        //=================
        //CNT_CALL_ADD_USER_IN_INTERVAL: 5006.4
        //CNT_CALL_REMOVE_USER_IN_INTERVAL: 4825.14
        //=================
        //INTERVAL_FLOW_OVER: 40.0878% 316/789
        //INTERVAL_INCREASE_LEN: 34.1213% 274/805
        //INTERVAL_DECREASE_LEN: 21.1914% 166/785
        //INTERVAL_GET_FULL_FREE_SPACE: 3.39132% 0/10
        //CNT_CALL_INTERVAL_DO_MERGE_EQUAL: 556.631
        //CNT_CALL_INTERVAL_DO_SPLIT: 18.573
        //INTERVAL_MERGE_EQUAL: 33.2781% 538/1616
        //=================
        //USER_NEW_INTERVAL: 61.989% 2230/3598
        //USER_ADD_LEFT: -nan% 0/0
        //USER_REMOVE_LEFT: -nan% 0/0
        //USER_ADD_RIGHT: 68.9216% 840/1219
        //USER_REMOVE_RIGHT: -nan% 0/0
        //CNT_CALL_USER_DO_CROP: 260.483
        //USER_CROP: 32.6654% 193/591
        //USER_SWAP: 50.0409% 77172/154217

        constexpr int STEPS = 10'000;
        for (int step = 0; step < STEPS; step++) {
            temperature = (STEPS - step) * 1.0 / STEPS;
            //temperature *= 0.999999;

            if (step != 0 && step % 2000 == 0) {
                //94.5985% | 977868/1033704
                //for (int u = 0; u < N; u++) {
                //    user_do_crop(u);
                //}

                //ASSERT(intervals.size() == J, "invalid intervals");
                //for(int i = 0; i < J; i++) {
                //    interval_get_full_free_space(i);
                //}
                //static mt19937 marsene(42);
                //shuffle(user_brute_order.begin(), user_brute_order.end(), marsene);
            }

            /*if (answer_score < total_score) {
                answer_score = total_score;
                answer = get_total_answer();
            }*/

            double p = rnd.get_d();
            if (p < 0.6) {
                interval_random_action();
            } else {
                user_random_action();
            }
        }

        //ASSERT(get_solution_score(N, M, K, J, L, reservedRBs, userInfos, answer) == answer_score,
        //       "failed answer score");

        // добить все интервалы до максимального свободного пространства
        for (int i = 0; i < intervals.size(); i++) {
            interval_get_full_free_space(i);
        }

        // обрезать юзеров так, чтобы они были только в одном блоке
        for (int u = 0; u < N; u++) {
            user_do_crop(u);
        }

        return get_total_answer();
    }
};

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

//TEST CASE: K=0 | tests: 666 | score: 93.3172% | 639374/685162 | time: 612.187ms | max_time: 9.977ms | mean_time: 0.9192ms
//TEST CASE: K=1 | tests: 215 | score: 91.7999% | 207275/225790 | time: 235.996ms | max_time: 9.534ms | mean_time: 1.09766ms
//TEST CASE: K=2 | tests: 80 | score: 90.471% | 75242/83167 | time: 119.629ms | max_time: 8.073ms | mean_time: 1.49536ms
//TEST CASE: K=3 | tests: 39 | score: 90.6576% | 41921/46241 | time: 72.05ms | max_time: 6.908ms | mean_time: 1.84744ms
//TEST CASE: K=4 | tests: 0 | score: -nan% | 0/0 | time: 0ms | max_time: 0ms | mean_time: 0ms
//TOTAL: tests: 1000 | score: 92.6422% | 963812/1040360 | time: 1039.86ms | max_time: 9.977ms | mean_time: 1.03986ms
vector<Interval> Solver_egor(int N, int M, int K, int J, int L,
                             const vector<Interval> &reservedRBs,
                             const vector<UserInfo> &userInfos, std::vector<Interval> solution) {
    EgorTaskSolver solver(N, M, K, J, L, reservedRBs, userInfos);
    auto answer = solver.annealing(solution);
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

    /*std::ofstream out("ans_data/case_K="+to_string(K)+"/"+to_string(test)+".txt");
    out << egor_answer.size() << endl;
    for (int i = 0; i < egor_answer.size(); i++){
        out << egor_answer[i].start << " " << egor_answer[i].end << endl;
        out << egor_answer[i].users.size() << endl;
        for (auto user_id: egor_answer[i].users ){
            out << user_id << " ";
        }
        out << endl;
    }
    out.close();*/

    return Solver_egor(N, M, K, J, L, reservedRBs, userInfos, {});

    /*auto artem_answer = Solver_Artem_grad(N, M, K, J, L, reservedRBs, userInfos);
    auto egor_answer = Solver_egor(N, M, K, J, L, reservedRBs, userInfos, artem_answer);

    //return egor_answer;
    auto egor_score = get_solution_score(N, M, K, J, L, reservedRBs, userInfos, egor_answer);
    auto artem_score = get_solution_score(N, M, K, J, L, reservedRBs, userInfos, artem_answer);

    if (egor_score > artem_score) {
        return egor_answer;
    } else {
        return artem_answer;
    }*/

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
