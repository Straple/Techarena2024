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
int CNT_CALL_INTERVAL_SPLIT = 0;
int CNT_ACCEPTED_INTERVAL_SPLIT = 0;

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

    [[nodiscard]] bool contains(int x) const {
        ASSERT(0 <= x && x < reserved_size, "invalid x");
        return (bits[x / bits_cnt] >> (x % bits_cnt)) & 1;
    }

    [[nodiscard]] bool empty() const {
        for (int i = 0; i < bits_size; i++) {
            if (bits[i] != 0) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] int size() const {
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

            x++;

            while (x / bits_cnt < bits_size) {
                uint64_t val = object.bits[x / bits_cnt] >> (x % bits_cnt);
                if (val == 0) {
                    x += bits_cnt - x % bits_cnt;
                } else {
                    x += __builtin_ctzll(val);
                    break;
                }
            }

            /*uint64_t val = object.bits[x / bits_cnt] >> (x % bits_cnt);
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
            }*/

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

    [[nodiscard]] Iterator begin() const {
        return ++Iterator(-1, *this);
        /*int x = 0;
        while (x < bits_size && bits[x] == 0) {
            x++;
        }
        x = x * bits_cnt + (x != bits_size ? __builtin_ctzll(bits[x]) : 0);
        return Iterator(x, *this);*/
    }

    [[nodiscard]] Iterator end() const {
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
    static inline int counter = 0;
    Timer timer;

    TimeAccumWrapper() : timer() {
        counter++;
    }

    ~TimeAccumWrapper() {
        time_accum += timer.get();
    }
};

// ===================================================================================================================
// =========================SOLUTION==================================================================================
// ===================================================================================================================

struct SelectionRandomizer {
    randomizer rnd;
    static inline randomizer SELECTION_RANDOMIZER_LEARN_RANDOM;

    // (id, power)
    std::vector<std::pair<int, int>> kit;

    int power_sum() const {
        int sum = 0;
        for (auto [id, power]: kit) {
            sum += power;
        }
        return sum;
    }

public:
    SelectionRandomizer(int n) : kit(n) {
        ASSERT(n > 0, "invalid n");
        for (int i = 0; i < n; i++) {
            kit[i] = {i, 10};
        }
    }

    SelectionRandomizer(std::vector<std::pair<int, int>> init_kit) : kit(std::move(init_kit)) {
        ASSERT(kit.size() > 0, "invalid kit");
#ifdef MY_DEBUG_MODE
        std::vector<int> cnt_id(kit.size());
        for (auto [id, power]: kit) {
            ASSERT(0 <= id && id < kit.size(), "invalid id");
            ASSERT(power >= 0, "invalid power");
            cnt_id[id]++;
        }
        for (int i = 0; i < kit.size(); i++) {
            ASSERT(cnt_id[i] == 1, "have equal id");
        }
#endif
    }

    void reset_rnd() {
        rnd = randomizer();
    }

    int select() {
        double p = rnd.get_d() * power_sum();
        int sum = 0;
        for (auto [id, power]: kit) {
            sum += power;
            if (p <= sum) {
                return id;
            }
        }
        ASSERT(false, "select failed");
    }

    void random_kit() {
        if (SELECTION_RANDOMIZER_LEARN_RANDOM.get_d() < 0.5) {
            // swap kits
            int a = SELECTION_RANDOMIZER_LEARN_RANDOM.get(0, kit.size() - 1);
            int b = SELECTION_RANDOMIZER_LEARN_RANDOM.get(0, kit.size() - 1);
            swap(kit[a], kit[b]);
        } else {
            int a = SELECTION_RANDOMIZER_LEARN_RANDOM.get(0, kit.size() - 1);
            int change = std::max(-kit[a].second, (int) SELECTION_RANDOMIZER_LEARN_RANDOM.get(-2, 2));
            if (change == 0) {
                change = 3;
            }
            kit[a].second += change;
        }
    }

    friend std::ostream &operator<<(std::ostream &output, SelectionRandomizer &selection) {
        output << "[\n";
        for (auto [id, power]: selection.kit) {
            output << "  " << id << ' ' << power << '\n';
        }
        output << "]\n";
        return output;
    }
};

SelectionRandomizer EGOR_TASK_SOLVER_SELECTION_USER_ACTION = std::vector<std::pair<int, int>>{
        {0, 27},
        {1, 0},
        {2, 1},
        {3, 1},
        {4, 0},
        {5, 9},
        {6, 3},
};
SelectionRandomizer EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION = std::vector<std::pair<int, int>>{
        {0, 12},
        {1, 6},
        {2, 18},
        {3, 8},
        {4, 6},
};
SelectionRandomizer EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION = std::vector<std::pair<int, int>>{
        {0, 9},
        {1, 11},
};
int STEPS = 100'000;

// TIME: 2336s
//score: 94.21
//93.93% 970937/1033704
//94.06% 972277/1033704
//94.22% 973974/1033704
//94.34% 975218/1033704
//
//USER:
//[
//  0 27
//  1 0
//  2 1
//  3 1
//  4 0
//  5 9
//  6 3
//]
//INTERVAL:
//[
//  0 12
//  1 6
//  2 18
//  3 8
//  4 6
//]
//USER OR INTERVAL:
//[
//  0 9
//  1 11
//]

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
        std::set<int>new_users;
        for (int g = 0; g < (int) candidates.size(); g++) {
            if (get_more == 0) break;
            if (beamOwnedBy[pick_i][userInfos[candidates[g].second].beam] == -1) {
                activeUsers[pick_i].insert(candidates[g].second);
                new_users.insert(candidates[g].second);
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
                    if (need - now <= int_len) {
                        ma_not_empty = max(ma_not_empty, need - now);
                    }
                }
//                cout << ma_not_empty << " " << int_len << " " << current_sub_interval[pick_i] << " " << pre_answer[pick_i].size() << endl;

                if (ma_not_empty == int_len) {
                    break;
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
                } else {
                    break;
                }
//                break;
//                break;
            }
        }
        int minus = 0;
        if (current_sub_interval[pick_i] != 0) {
            int len = pre_answer[pick_i][current_sub_interval[pick_i]].end -
                      pre_answer[pick_i][current_sub_interval[pick_i]].start;
            int best_ind = 0;
            int best_metric = 0;
            for (int i = 0; i <= len; i++) {
                int metric = 0;
                for (auto user_id: activeUsers[pick_i]) {
                    if (new_users.count(user_id)) {
                        metric-=i;
                    } else {
                        int will_supplied_if_moved = rbSuplied[user_id] + i;
                        if (will_supplied_if_moved >= userInfos[user_id].rbNeed) {
                            metric += len - i;
                            metric -= will_supplied_if_moved-userInfos[user_id].rbNeed;
                        }
                    }
                }
                //                cout << i << " " << metric << "|";
                if (metric > best_metric) {
                    best_metric = metric;
                    best_ind = i;
                }
            }
            //            cout << endl;
            if (best_ind != 0) { // Р СР С•Р В¶Р Р…Р С• Р С‘ РЎС“Р В±РЎР‚Р В°РЎвЂљРЎРЉ, Р С—РЎР‚Р С•РЎРѓРЎвЂљР С• Р Т‘Р В»РЎРЏ РЎС“Р Т‘Р С•Р В±РЎРѓРЎвЂљР Р†Р В° Р С‘ Р С–Р В°РЎР‚Р В°Р Р…РЎвЂљР С‘Р С‘
                minus = best_ind;
                //                cout << "UPD!" << " " << best_ind << endl;
                pre_answer[pick_i][current_sub_interval[pick_i] - 1].end += best_ind;
                pre_answer[pick_i][current_sub_interval[pick_i]].start += best_ind;

                for (auto user_id: pre_answer[pick_i][current_sub_interval[pick_i] - 1].users) {
                    rbSuplied[user_id]+=best_ind;
                    if (rbSuplied[user_id] >= userInfos[user_id].rbNeed) {
                        if (activeUsers[pick_i].count(user_id)) {
                            beamOwnedBy[pick_i][userInfos[user_id].beam] = -1;
                            activeUsers[pick_i].erase(user_id);
                        }
                    }
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
                std::set<int>new_users;
                for (int g = 0; g < (int) candidates.size(); g++) {
                    if (get_more == 0) break;
                    if (beamOwnedBy[pick_i][userInfos[candidates[g].second].beam] == -1) {
                        activeUsers[pick_i].insert(candidates[g].second);
                        used_users.insert(candidates[g].second);
                        beamOwnedBy[pick_i][userInfos[candidates[g].second].beam] = candidates[g].second;
                        get_more--;
                    }
                }
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
                      pre_answer[pick_i][current_sub_interval[pick_i]].start + minus;
        current_sub_interval[pick_i]++;
        if (space_left != 0) {
            space_left_q.insert({space_left, pick_i});
        }
    }

    return pre_answer;
}

int
get_solution_score_light(int N, vector<vector<Interval>> &ans, const vector<UserInfo> &userInfos, std::vector<int> &suplied) {
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


    for (int iter = 0; iter < 1; iter++) {
        // Обрезаем хвост, отдаём голове
        for (int i = 0; i < N; i++) {

            if (mi[i] != 10000) {// можно сделать так что бь mi == 0
                bool is_start = (mi[i] == 0);
                int best_score_gain = -1;
                int best_receiver = -1;
                int current_len = solution[mi[i]].end - solution[mi[i]].start;
                int minus = min(suplied[i], userInfos[i].rbNeed) - min(suplied[i] - current_len, userInfos[i].rbNeed);
                bool is_empty_winner = false;
                if (!is_start) {
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
            if (ma[i] != -10000 ) {// можно сделать так что бь mi == 0
                bool is_last =  ma[i] + 1 == solution.size();
                int best_score_gain = -1;
                int best_receiver = -1;
                int current_len = solution[ma[i]].end - solution[ma[i]].start;
                int minus = min(suplied[i], userInfos[i].rbNeed) - min(suplied[i] - current_len, userInfos[i].rbNeed);
                bool is_empty_winner = false;
                if (!is_last) {
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


vector<Interval> Solver_Artem_grad(int N, int M, int K, int J, int L,
                                   vector<Interval> reservedRBs,
                                   vector<UserInfo> userInfos) {

    vector<int> suplied(N, 0);
    std::vector<std::vector<std::vector<Interval>>>anses;
    anses.push_back(Solver_artem(N, M, K, J, L, reservedRBs, userInfos, -2.0));
    //    anses.push_back(Solver_artem(N, M, K, J, L, reservedRBs, userInfos, -2.0, 0.75));
    //    anses.push_back(Solver_artem(N, M, K, J, L, reservedRBs, userInfos, -2.0, 0.5));
    //    anses.push_back(Solver_artem(N, M, K, J, L, reservedRBs, userInfos, -2.0, 0.3));


    //    anses.push_back(Solver_artem(N, M, K, J, L, reservedRBs, userInfos, -2.0, 0.7));
    //    anses.push_back(Solver_artem(N, M, K, J, L, reservedRBs, userInfos, -2.0, 0.6));
    int biggest_score = -1;
    int biggest_index = -1;
    for (int i = 0; i < anses.size(); i++){
        optimize(N, M, K, J, L, reservedRBs, userInfos, anses[i]);
        int score = get_solution_score_light(N, anses[i], userInfos, suplied);
        if (score > biggest_score){
            biggest_score = score;
            biggest_index = i;
        }
    }
    std::vector<vector<Interval>>pre_answer = anses[biggest_index];
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

int cnt_edges[12][12];
int cnt_good_edges[12][12];
int prev_action = -1;

struct EgorTaskSolver {
    ///============================
    /// task data
    ///============================

    int N;
    int M;
    int K;
    int J;
    int L;

    int free_intervals_size = 0;
    MyInterval free_intervals[5];

    ///============================
    /// internal data
    ///============================

    randomizer rnd;
    mt19937 marsene_twist;

    int total_score = 0;

    int current_user = -1;

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

    SetInterval intervals[16];

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

    MyUserInfo users_info[128];
    int theor_max;

    int user_id_to_u[128];

    int users_order[128];

    int users_with_equal_beam_data[128];
    int users_with_equal_beam_size[32];
    int *users_with_equal_beam[32];

    // проходит по intervals
    // инициализирует .block так, чтобы длины не превышали free_interval
    // если .block = -1, то этот интервал никуда не влез
    void build_blocks() {
        int cur_block = 0;
        int cur_len = 0;

        int i;
        for (i = 0; i < J; i++) {
            if (cur_len + intervals[i].len > free_intervals[cur_block].len()) {
                // не помещаемся
                cur_block++;
                if (cur_block == free_intervals_size) {
                    break;
                }
                cur_len = 0;
                i--;
            } else {
                cur_len += intervals[i].len;
                intervals[i].block = cur_block;
            }
        }
        for (; i < J; i++) {
            intervals[i].block = -1;
        }
    }

    EgorTaskSolver(int NN, int MM, int KK, int JJ, int LL,
                   const vector<Interval> &reservedRBs,
                   const vector<UserInfo> &userInfos) : N(NN), M(MM), K(KK), J(JJ), L(LL) {

        theor_max = get_theory_max_score(N,M,K,J,L,reservedRBs, userInfos);

        ASSERT(2 <= L && L <= 16, "kek");
        ASSERT(0 < J && J <= 16, "hoho");

        {
            for (int beam = 0; beam < 32; beam++) {
                users_with_equal_beam_size[beam] = 0;
            }
            for (int u = 0; u < N; u++) {
                ASSERT(u == userInfos[u].id, "are you stupid or something?");
                ASSERT(0 <= userInfos[u].beam && userInfos[u].beam < 32, "invalid beam");
                users_info[u].id = userInfos[u].id;
                users_info[u].rbNeed = userInfos[u].rbNeed;
                users_info[u].beam = userInfos[u].beam;

                users_with_equal_beam_size[userInfos[u].beam]++;
                user_id_to_u[users_info[u].id] = u;
            }

            // reserved data
            users_with_equal_beam[0] = users_with_equal_beam_data;
            for (int beam = 1; beam < 32; beam++) {
                users_with_equal_beam[beam] = users_with_equal_beam[beam - 1] + users_with_equal_beam_size[beam - 1];
            }

            for (int beam = 0; beam < 32; beam++) {
                users_with_equal_beam_size[beam] = 0;
            }
            // push_back
            for (int u = 0; u < N; u++) {
                int beam = users_info[u].beam;
                users_with_equal_beam[beam][users_with_equal_beam_size[beam]] = users_info[u].id;
                users_with_equal_beam_size[beam]++;
            }
        }

        // build free_intervals
        {
            bool is_free[513];
            for (int i = 0; i < 513; i++) {
                is_free[i] = true;
            }
            is_free[M] = false;

            for (const auto &[start, end, users]: reservedRBs) {
                for (int i = start; i < end; i++) {
                    is_free[i] = false;
                }
            }

            int start = -1;
            for (int i = 0; i < 513; i++) {
                if (!is_free[i]) {
                    if (start != i - 1) {
                        free_intervals[free_intervals_size] = {start + 1, i};
                        free_intervals_size++;
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
        for (int i = 0; i < J; i++) {
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
        //#ifndef MY_DEBUG_MODE
        return users_info[u].left;
        //#endif
        CNT_CALL_GET_LEFT_USER++;
        for (int i = 0; i < J; i++) {
            if (intervals[i].users.contains(u)) {
                ASSERT(i == users_info[u].left, "failed calc left");
                return i;
            }
        }
        ASSERT(-1 == users_info[u].left, "failed calc left");
        return -1;
    }

    int get_right_user(int u) {
        //#ifndef MY_DEBUG_MODE
        return users_info[u].right;
        //#endif
        CNT_CALL_GET_RIGHT_USER++;
        for (int i = J - 1; i >= 0; i--) {
            if (intervals[i].users.contains(u)) {
                ASSERT(i == users_info[u].right, "failed calc right");
                return i;
            }
        }
        ASSERT(-1 == users_info[u].right, "failed calc right");
        return -1;
    }

    void update_block_after_change_interval() {
        int old_first_bad_block = J;
        for (int i = 0; i < J; i++) {
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

        while (old_first_bad_block < J && intervals[old_first_bad_block].block != -1) {
            for (int u: intervals[old_first_bad_block].users) {
                total_score -= users_info[u].calc_score();
                users_info[u].sum_len += intervals[old_first_bad_block].len;
                total_score += users_info[u].calc_score();
            }

            old_first_bad_block++;
        }
    }

    void change_interval_len_IMPL(int interval, int change) {
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
    }

    void change_interval_len(int interval, int change) {
        CNT_CALL_CHANGE_INTERVAL_LEN++;
        change_interval_len_IMPL(interval, change);
        update_block_after_change_interval();
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

    double temperature = 1;

    bool is_good(int old_score) {
        return total_score > old_score || rnd.get_d() < exp((total_score - old_score) / temperature);
    }

    ///==========================
    ///===========INTERVAL=======
    ///==========================

#define CHOOSE_INTERVAL(condition)                         \
    int interval;                                          \
    {                                                      \
        vector<int> ips;                                   \
        for (int interval = 0; interval < J; interval++) { \
            if (condition) {                               \
                ips.push_back(interval);                   \
            }                                              \
        }                                                  \
        if (ips.empty()) {                                 \
            return;                                        \
        }                                                  \
        interval = ips[rnd.get(0, ips.size() - 1)];        \
    }

    void interval_flow_over() {
        CNT_CALL_INTERVAL_FLOW_OVER++;

        CHOOSE_INTERVAL(interval + 1 < J);

        int change = rnd.get(-intervals[interval].len, intervals[interval + 1].len);

        int old_score = total_score;

        change_interval_len_IMPL(interval, change);
        change_interval_len_IMPL(interval + 1, -change);
        update_block_after_change_interval();

        /*if (change > 0) {
            change_interval_len(interval + 1, -change);
            change_interval_len(interval, change);
        } else {
            change_interval_len(interval, change);
            change_interval_len(interval + 1, -change);
        }*/

        if (is_good(old_score)) {
            CNT_ACCEPTED_INTERVAL_FLOW_OVER++;
        } else {
            change_interval_len_IMPL(interval, -change);
            change_interval_len_IMPL(interval + 1, change);
            update_block_after_change_interval();
            /*if (change > 0) {
                change_interval_len(interval, -change);
                change_interval_len(interval + 1, change);
            } else {
                change_interval_len(interval + 1, change);
                change_interval_len(interval, -change);
            }*/
            ASSERT(old_score == total_score, "failed back score");
        }
    }

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
        for (int i = 0; i < J; i++) {
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
    void interval_do_merge_equal(int interval) {
        CNT_CALL_INTERVAL_DO_MERGE_EQUAL++;

        ASSERT(intervals[interval].users == intervals[interval + 1].users, "kek");
        ASSERT(intervals[interval].beam_msk == intervals[interval + 1].beam_msk, "kek");

        int old_score = total_score;

        int right_len = intervals[interval + 1].len;
        change_interval_len(interval + 1, -right_len);
        change_interval_len(interval, right_len);

        // erase interval + 1
        for (int i = interval + 1; i + 1 < J; i++) {
            swap(intervals[i], intervals[i + 1]);
        }
        intervals[J - 1] = SetInterval();

        // update users left and right
        for (int u = 0; u < N; u++) {
            int &left = users_info[u].left;
            if (interval < left) {
                left--;
            }

            int &right = users_info[u].right;
            if (interval < right) {
                right--;
            }

#ifdef MY_DEBUG_MODE
            get_left_user(u);
            get_right_user(u);
#endif
        }
    }

    void interval_do_split(int interval, int right_len) {
        CNT_CALL_INTERVAL_DO_SPLIT++;
        // ожидается, что intervals[J-1] не используется
        // или можно || intervals[J-1].block == -1
        ASSERT(intervals[J - 1].users.empty(), "kek");
        for (int i = J - 1; interval < i - 1; i--) {
            swap(intervals[i - 1], intervals[i]);
        }

        intervals[interval + 1] = intervals[interval];
        intervals[interval + 1].len = 0;

        change_interval_len(interval, -right_len);
        change_interval_len(interval + 1, right_len);

        // update users left and right
        for (int u = 0; u < N; u++) {
            int &left = users_info[u].left;
            if (interval < left) {
                left++;
            }

            int &right = users_info[u].right;
            if (interval <= right) {
                right++;
            }

#ifdef MY_DEBUG_MODE
            get_left_user(u);
            get_right_user(u);
#endif
        }
    }

    void interval_merge_equal() {
        CNT_CALL_INTERVAL_MERGE_EQUAL++;

        CHOOSE_INTERVAL(interval + 1 < J && intervals[interval].users == intervals[interval + 1].users);

        int old_score = total_score;
        int right_len = intervals[interval + 1].len;

        interval_do_merge_equal(interval);

        if (is_good(old_score)) {
            CNT_ACCEPTED_INTERVAL_MERGE_EQUAL++;
        } else {
            interval_do_split(interval, right_len);

            ASSERT(old_score == total_score, "failed back score");
        }
    }

    void interval_split() {
        CNT_CALL_INTERVAL_SPLIT++;
        CHOOSE_INTERVAL(interval + 1 < J);

        int old_score = total_score;

        SetInterval save_back = intervals[J - 1];
        for (int u: intervals[J - 1].users) {
            remove_user_in_interval(u, J - 1);
        }

        int right_len = rnd.get(0, intervals[interval].len);

        interval_do_split(interval, right_len);

        if (is_good(old_score)) {
            CNT_ACCEPTED_INTERVAL_SPLIT++;
        } else {
            interval_do_merge_equal(interval);
            intervals[J - 1].len = save_back.len;
            build_blocks();
            for (int u: save_back.users) {
                add_user_in_interval(u, J - 1);
            }

            ASSERT(old_score == total_score, "failed back score");
        }
    }

    void interval_random_action() {
        int s = EGOR_TASK_SOLVER_SELECTION_INTERVAL_ACTION.select();
        if (s == 0) {
            int old_score = total_score;
            if (prev_action != -1) {
                cnt_edges[prev_action][7]++;
            }
            interval_increase_len();

            if (old_score < total_score && prev_action != -1) {
                cnt_good_edges[prev_action][7]++;
            }

            prev_action = 7;
        } else if (s == 1) {
            int old_score = total_score;
            if (prev_action != -1) {
                cnt_edges[prev_action][8]++;
            }
            interval_decrease_len();

            if (old_score < total_score && prev_action != -1) {
                cnt_good_edges[prev_action][8]++;
            }

            prev_action = 8;
        } else if (s == 2) {
            int old_score = total_score;
            if (prev_action != -1) {
                cnt_edges[prev_action][9]++;
            }
            interval_flow_over();

            if (old_score < total_score && prev_action != -1) {
                cnt_good_edges[prev_action][9]++;
            }

            prev_action = 9;
        } else if (s == 3) {
            int old_score = total_score;
            if (prev_action != -1) {
                cnt_edges[prev_action][10]++;
            }
            interval_merge_equal();

            if (old_score < total_score && prev_action != -1) {
                cnt_good_edges[prev_action][10]++;
            }

            prev_action = 10;
        } else if (s == 4) {
            int old_score = total_score;
            if (prev_action != -1) {
                cnt_edges[prev_action][11]++;
            }
            interval_split();

            if (old_score < total_score && prev_action != -1) {
                cnt_good_edges[prev_action][11]++;
            }

            prev_action = 11;
        } else {
            ASSERT(false, "kek");
        }

        /*double p = rnd.get_d();
        if (p < 0.2) {
            interval_increase_len();
        } else if (p < 0.4) {
            interval_decrease_len();
        } else if (p < 0.8) {
            interval_flow_over();
        } else if (p < 0.9) {
            interval_merge_equal();
        } else {
            interval_split();
        }*/
        /* else {
            interval_get_full_free_space(interval);
        }*/
    }

    ///======================
    ///===========USER=======
    ///======================

#define CHOOSE_USER(condition)               \
    int u;                                   \
    {                                        \
        vector<int> ips;                     \
        for (int u = 0; u < N; u++) {        \
            int left = get_left_user(u);     \
            int right = get_right_user(u);   \
            if (condition) {                 \
                ips.push_back(u);            \
            }                                \
        }                                    \
        if (ips.empty()) {                   \
            return;                          \
        }                                    \
        u = ips[rnd.get(0, ips.size() - 1)]; \
    }

#define USER_FOR_BEGIN(condition)                         \
    shuffle(users_order, users_order + N, marsene_twist); \
    for (int index = 0; index < N; index++) {             \
        int u = users_order[index];                       \
        int left = get_left_user(u);                      \
        int right = get_right_user(u);                    \
        if (condition) {

#define USER_FOR_END \
    }                \
    }

    // 0
    void user_new_interval() {
        CNT_CALL_USER_NEW_INTERVAL++;

        int u = users_order[current_user];//rnd.get(0, N - 1);
        current_user = (current_user + 1) % N;

        int user_left = get_left_user(u);
        int user_right = get_right_user(u);

        bool okay[16];
        for (int i = 0; i < J; i++) {
            okay[i] = intervals[i].block != -1 &&
                      (intervals[i].users.contains(u) ||// либо мы уже здесь стоим
                       // либо мы можем сюда поставить
                       (intervals[i].users.size() + 1 <= L && ((intervals[i].beam_msk >> users_info[u].beam) & 1) == 0));
        }

        auto f = [&](int len) {
            if (len > users_info[u].rbNeed) {
                return 2 * (len - users_info[u].rbNeed);
            } else {
                return users_info[u].rbNeed - len;
            }
        };

        int best_left = -1, best_right = -1, best_f = 1e9, best_sum_len = 0;
        for (int left = 0; left < J; left++) {
            int sum_len = 0;
            for (int right = left; right < J && okay[right] && intervals[left].block == intervals[right].block; right++) {
                sum_len += intervals[right].len;

                int cur_f = f(sum_len);
                if (cur_f < best_f && !(user_left == left && user_right == right)) {
                    best_f = cur_f;
                    best_sum_len = sum_len;
                    best_left = left;
                    best_right = right;
                }
            }
        }

        ASSERT(user_left == -1 || !(best_left == user_left && best_right == user_right), "failed get best");

        if (best_left == -1) {
            return;
        }

        int old_score = total_score;

        total_score -= users_info[u].calc_score();
        swap(users_info[u].sum_len, best_sum_len);
        total_score += users_info[u].calc_score();
        swap(users_info[u].sum_len, best_sum_len);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_NEW_INTERVAL++;

            int nice_score = total_score;
            total_score = old_score;

            if (user_left != -1) {
                for (int i = user_left; i <= user_right; i++) {
                    remove_user_in_interval(u, i);
                }
            }
            for (int i = best_left; i <= best_right; i++) {
                add_user_in_interval(u, i);
            }
            ASSERT(nice_score == total_score, "failed nice score");

        } else {
            total_score = old_score;
        }

        /// old version
        /// must too slow
        /*CNT_CALL_USER_NEW_INTERVAL++;
        vector<int> removed;

        int u = users_order[current_user];//rnd.get(0, N - 1);
        current_user = (current_user + 1) % N;

        int old_score = total_score;

        bool okay[16];
        for (int i = 0; i < J; i++) {
            if (intervals[i].users.contains(u)) {
                removed.push_back(i);
                remove_user_in_interval(u, i);
            }
            okay[i] = intervals[i].users.size() + 1 <= L &&
                      ((intervals[i].beam_msk >> users_info[u].beam) & 1) == 0;
        }

        ASSERT(get_left_user(u) == users_info[u].left, "failed left");
        ASSERT(get_right_user(u) == users_info[u].right, "failed right");

        auto f = [&](int len) {
            if (len > users_info[u].rbNeed) {
                return 2 * (len - users_info[u].rbNeed);
            } else {
                return users_info[u].rbNeed - len;
            }
        };

        int best_left = -1, best_right = -1, best_f = 1e9;
        for (int left = 0; left < J; left++) {
            int sum_len = 0;
            for (int right = left; right < J && okay[right] &&
                                   intervals[left].block == intervals[right].block &&
                                   intervals[left].block != -1;
                 right++) {
                sum_len += intervals[right].len;

                int cur_f = f(sum_len);
                if (cur_f < best_f) {
                    best_f = cur_f;
                    best_left = left;
                    best_right = right;
                }
            }
        }

        if (best_left == -1) {
            for (int i: removed) {
                add_user_in_interval(u, i);
            }
            ASSERT(old_score == total_score, "failed back score");
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
        }*/
    }

    // 1
    void user_add_left() {
        CNT_CALL_USER_ADD_LEFT++;

        USER_FOR_BEGIN(left > 0 &&
                       intervals[left - 1].users.size() < L &&
                       ((intervals[left - 1].beam_msk >> users_info[u].beam) & 1) == 0 &&
                       intervals[left - 1].block != -1)


        left--;

        int old_score = total_score;

        add_user_in_interval(u, left);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_ADD_LEFT++;
        } else {
            remove_user_in_interval(u, left);
            ASSERT(old_score == total_score, "failed back score");
        }
        USER_FOR_END
    }

    // 2
    void user_add_right() {
        CNT_CALL_USER_ADD_RIGHT++;

        USER_FOR_BEGIN(right != -1 && right + 1 < J &&
                       intervals[right + 1].users.size() < L &&
                       ((intervals[right + 1].beam_msk >> users_info[u].beam) & 1) == 0 &&
                       intervals[right + 1].block != -1)


        right++;
        int old_score = total_score;

        add_user_in_interval(u, right);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_ADD_RIGHT++;
        } else {
            remove_user_in_interval(u, right);
            ASSERT(old_score == total_score, "failed back score");
        }
        USER_FOR_END

        /*CHOOSE_USER(right != -1 && right + 1 < J &&
                    intervals[right + 1].users.size() < L &&
                    ((intervals[right + 1].beam_msk >> users_info[u].beam) & 1) == 0 &&
                    intervals[right + 1].block != -1);

        int right = get_right_user(u);
        right++;

        int old_score = total_score;

        add_user_in_interval(u, right);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_ADD_RIGHT++;
        } else {
            remove_user_in_interval(u, right);
            ASSERT(old_score == total_score, "failed back score");
        }*/
    }

    // 3
    void user_remove_left() {
        CNT_CALL_USER_REMOVE_LEFT++;

        USER_FOR_BEGIN(left != -1 && intervals[left].block != -1)

        int old_score = total_score;

        remove_user_in_interval(u, left);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_REMOVE_LEFT++;
        } else {
            add_user_in_interval(u, left);
            ASSERT(old_score == total_score, "failed back score");
        }

        USER_FOR_END

        /*CHOOSE_USER(left != -1 && intervals[left].block != -1);

        int left = get_left_user(u);

        int old_score = total_score;

        remove_user_in_interval(u, left);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_REMOVE_LEFT++;
        } else {
            add_user_in_interval(u, left);
            ASSERT(old_score == total_score, "failed back score");
        }*/
    }

    // 4
    void user_remove_right() {
        CNT_CALL_USER_REMOVE_RIGHT++;

        USER_FOR_BEGIN(right != -1)

        int old_score = total_score;

        remove_user_in_interval(u, right);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_REMOVE_RIGHT++;
        } else {
            add_user_in_interval(u, right);
            ASSERT(old_score == total_score, "failed back score");
        }
        USER_FOR_END

        /*CHOOSE_USER(right != -1);

        int right = get_right_user(u);

        int old_score = total_score;

        remove_user_in_interval(u, right);

        if (is_good(old_score)) {
            CNT_ACCEPTED_USER_REMOVE_RIGHT++;
        } else {
            add_user_in_interval(u, right);
            ASSERT(old_score == total_score, "failed back score");
        }*/
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

    // 5
    void user_swap() {
        for (int beam = 0; beam < 32; beam++) {
            // (sum_len, u)
            vector<pair<int, int>> ups;
            for (int i = 0; i < users_with_equal_beam_size[beam]; i++) {
                int u_id = users_with_equal_beam[beam][i];
                int u = user_id_to_u[u_id];
                ups.push_back({users_info[u].sum_len, u});
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

    vector<int> user_do_crop(int u) {
        CNT_CALL_USER_DO_CROP++;
        int best_left = -1, best_right = -1, best_len = -1e9;
        for (int left = 0; left < J; left++) {
            int len = 0;
            for (int right = left; right < J && intervals[right].users.contains(u) &&
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
    // 6
    void user_crop() {
        CNT_CALL_USER_CROP++;

        CHOOSE_USER(left != -1 && intervals[left].block != intervals[right].block);

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

    void user_random_action() {
        int s = EGOR_TASK_SOLVER_SELECTION_USER_ACTION.select();
        if (s == 0) {
            int old_score = total_score;
            if (prev_action != -1) {
                cnt_edges[prev_action][0]++;
            }
            user_new_interval();

            if (old_score < total_score && prev_action != -1) {
                cnt_good_edges[prev_action][0]++;
            }

            prev_action = 0;
        } else if (s == 1) {
            int old_score = total_score;
            if (prev_action != -1) {
                cnt_edges[prev_action][1]++;
            }
            user_add_left();

            if (old_score < total_score && prev_action != -1) {
                cnt_good_edges[prev_action][1]++;
            }

            prev_action = 1;
        } else if (s == 2) {
            int old_score = total_score;
            if (prev_action != -1) {
                cnt_edges[prev_action][2]++;
            }
            user_add_right();

            if (old_score < total_score && prev_action != -1) {
                cnt_good_edges[prev_action][2]++;
            }

            prev_action = 2;
        } else if (s == 3) {
            int old_score = total_score;
            if (prev_action != -1) {
                cnt_edges[prev_action][3]++;
            }
            user_remove_left();

            if (old_score < total_score && prev_action != -1) {
                cnt_good_edges[prev_action][3]++;
            }

            prev_action = 3;
        } else if (s == 4) {
            int old_score = total_score;
            if (prev_action != -1) {
                cnt_edges[prev_action][4]++;
            }
            user_remove_right();

            if (old_score < total_score && prev_action != -1) {
                cnt_good_edges[prev_action][4]++;
            }

            prev_action = 4;
        } else if (s == 5) {
            int old_score = total_score;
            if (prev_action != -1) {
                cnt_edges[prev_action][5]++;
            }
            user_swap();

            if (old_score < total_score && prev_action != -1) {
                cnt_good_edges[prev_action][5]++;
            }

            prev_action = 5;
        } else if (s == 6) {
            int old_score = total_score;
            if (prev_action != -1) {
                cnt_edges[prev_action][6]++;
            }
            user_crop();

            if (old_score < total_score && prev_action != -1) {
                cnt_good_edges[prev_action][6]++;
            }

            prev_action = 6;
        } else {
            ASSERT(false, "kek");
        }
    }

    vector<Interval> annealing(vector<Interval> start_intervals) {
        temperature = 1;
        prev_action = -1;

        {
            current_user = 0;
            for (int u = 0; u < N; u++) {
                users_order[u] = u;
            }
            sort(users_order, users_order + N, [&](int lhs, int rhs) {
                return users_info[lhs].rbNeed > users_info[rhs].rbNeed;
            });
        }

        // build from start_intervals
        {
            sort(start_intervals.begin(), start_intervals.end(), [&](const auto &lhs, const auto &rhs) {
                return lhs.start < rhs.start;
            });

            int interval = 0;
            for (int i = 0; i < start_intervals.size(); i++) {
                intervals[interval] = SetInterval{-1, start_intervals[i].end - start_intervals[i].start, {}};
                interval++;
            }
            while (interval < J) {
                intervals[interval] = SetInterval{-1, 0, {}};
                interval++;
            }
            build_blocks();

            for (int i = 0; i < start_intervals.size(); i++) {
                for (auto u: start_intervals[i].users) {
                    add_user_in_interval(u, i);
                }
            }
        }

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

        for (int step = 0; step < STEPS; step++) {
            temperature = (STEPS - step) * 1.0 / STEPS;
            //temperature *= 0.999999;
            if (total_score == theor_max) {
                break;
            }

            ASSERT(STEPS % 10 == 0 && STEPS != 0, "invalid STEPS");
            if (step != 0 && step % (STEPS / 10) == 0) {
                //94.5985% | 977868/1033704
                for (int u = 0; u < N; u++) {
                    user_do_crop(u);
                }

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

            int s = EGOR_TASK_SOLVER_SELECTION_USER_OR_INTERVAL_ACTION.select();
            if (s == 0) {
                interval_random_action();
            } else if (s == 1) {
                user_random_action();
            } else {
                ASSERT(false, "kek");
            }
        }

        //ASSERT(get_solution_score(N, M, K, J, L, reservedRBs, userInfos, answer) == answer_score,
        //       "failed answer score");

        // добить все интервалы до максимального свободного пространства
        for (int i = 0; i < J; i++) {
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
    ASSERT(solver.total_score == get_solution_score(N, M, K, J, L, reservedRBs, userInfos, answer), "invalid total_score");
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


    //    return Solver_egor(N, M, K, J, L, reservedRBs, userInfos, {});
    auto theor_max = get_theory_max_score(N,M,K,J,L,reservedRBs, userInfos);
    auto artem_answer = Solver_Artem_grad(N, M, K, J, L, reservedRBs, userInfos);
    auto artem_score = get_solution_score(N, M, K, J, L, reservedRBs, userInfos, artem_answer);
    //    cout << artem_score << " " << theor_max << endl;
    ASSERT(theor_max >= artem_score, "WA THEORMAX");
    if (theor_max <= artem_score){
        //        cerr << "MAX_SCORE" << endl;
        return artem_answer;
    }
    auto egor_answer = Solver_egor(N, M, K, J, L, reservedRBs, userInfos, artem_answer);
    auto get_egor_blocked = ans_to_blocked_ans(M,K,reservedRBs,egor_answer);
    optimize(N, M, K, J, L, reservedRBs, userInfos,get_egor_blocked);
    egor_answer.clear();
    vector<Interval> answer;
    for (int i = 0; i < get_egor_blocked.size(); i++) {
        for (int g = 0; g < get_egor_blocked[i].size(); g++) {
            if (get_egor_blocked[i][g].users.size()) {
                egor_answer.push_back(get_egor_blocked[i][g]);
            }
        }
    }
    //
    //    return egor_answer;
    auto egor_score = get_solution_score(N, M, K, J, L, reservedRBs, userInfos, egor_answer);

    if (egor_score > artem_score) {
        //        cout << "EGOR BETTER" << endl;
        //        if (egor_score == theor_max){
        //            cout << "EGOR IS THE BEST" << endl;
        //        }
        return egor_answer;
    } else {
        return artem_answer;
    }
    //    double artem_score = get_solution_score({N, M, K, J, L, reservedRBs, userInfos}, artem_answer);
    //
    //    auto egor_answer = Solver_egor(N, M, K, J, L, reservedRBs, userInfos);
    //    double egor_score = get_solution_score({N, M, K, J, L, reservedRBs, userInfos}, egor_answer);
    //
    //    if (artem_score > egor_score) {
    //        return artem_answer;
    //    } else {
    //        return egor_answer;
    //    }
}