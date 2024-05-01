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

#include <set>

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
int get_theory_max_score(int N, int M, int K, int J, int L, const vector<Interval>& reservedRBs, const  vector<UserInfo>& userInfos) {
    int max_score = 0;
    std::vector<int>rbNeeded;
    std::vector<pair<int,int>>reserved;
    for (int i = 0; i < K; i++){
        reserved.push_back({reservedRBs[i].start,reservedRBs[i].end});
    }
    sort(reserved.begin(), reserved.end());
    int max_res_len = M;
    if (K){
        max_res_len = max(reserved[0].first, M - reserved.back().second);
    }
    for (int i = 1; i < reserved.size(); i++){
        max_res_len = max(max_res_len, reserved[i].first - reserved[i-1].second);
    }
    for (auto user: userInfos) {
        rbNeeded.push_back(min(user.rbNeed,max_res_len));
    }
    sort(rbNeeded.begin(), rbNeeded.end(), greater<>());
    for (int i = 0; i < min(N, J*L); i++){
        max_score+=rbNeeded[i];
    }

    int max_possible = M * L;
    for (const auto &reserved: reservedRBs) {
        max_possible -= (reserved.end - reserved.start) * L;
    }

    return min(max_score, max_possible);
}

int get_theory_max_score(const TestData &data) {
    return get_theory_max_score(data.N,data.M,data.K,data.J,data.L,data.reservedRBs, data.userInfos );
}


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
    std::vector<set < int>>
    mi_set(M);
    std::vector<set < int>>
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
    int theor_max = 0;

    vector<MyInterval> free_intervals;

    ///============================
    /// internal data
    ///============================

    randomizer rnd;

    int total_score = 0;

    struct SetInterval {
        int start, end;
        MyBitSet<128> users;

        [[nodiscard]] int len() const {
            return end - start;
        }
    };

    vector<vector<SetInterval>> intervals;

    vector<int> sum_intervals_len;

    struct MyUserInfo {
        // user info
        int id = -1;
        int rbNeed = -1;
        int beam = -1;

        // interval position
        int block = -1;
        int left = -1;
        int right = -1;

        // score
        int sum_len = 0;

        int calc_score() const {
            if (sum_len > rbNeed) {
                return rbNeed * 100;
            } else {
                return sum_len * 100;
            }
        }
    };

    vector<MyUserInfo> users_info;

    vector<vector<uint32_t>> beams_msk;

    EgorTaskSolver(int NN, int MM, int KK, int JJ, int LL,
                   const vector<Interval> &reservedRBs,
                   const vector<UserInfo> &userInfos) : N(NN), M(MM), K(KK), J(JJ), L(LL) {

        theor_max = get_theory_max_score(N,M,K,J,L,reservedRBs, userInfos);
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
                        intervals[block].push_back(SetInterval{0, mean_len, {}});
                        break;
                    }
                }

                if (!find) {
                    intervals[p].push_back(SetInterval{0, 0, {}});
                    p = (p + 1) % free_intervals.size();
                }
            }

            beams_msk.resize(free_intervals.size());
            for (int block = 0; block < intervals.size(); block++) {
                beams_msk[block].resize(intervals[block].size());
            }
        }

        for (int u = 0; u < N; u++) {
            total_score += users_info[u].calc_score();
        }
    }

    [[nodiscard]] vector<Interval> get_total_answer() {
        vector <Interval> answer;
        for (int block = 0; block < intervals.size(); block++) {
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
        }
        return answer;
    }

    int calc_beams_score(int block, int interval) {
        int pops = __builtin_popcount(beams_msk[block][interval]);
        return pops;
    }

    bool have_equal_beam(int block, int interval, int beam) {
        ASSERT(0 <= block && block < intervals.size() && 0 <= interval && interval < intervals[block].size(),
               "invalid request");

        /*bool res = false;
        for (int u: intervals[block][interval].users) {
            if (users_info[u].beam == beam) {
                res = true;
            }
        }

        ASSERT(res == bool((beams_msk[block][interval] >> beam) & 1), "kek");*/
        return (beams_msk[block][interval] >> beam) & 1;
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
            sum_len += intervals[block][i].len();
        }
        ASSERT(sum_len == sum_intervals_len[block], "failed calculating sum_intervals_len");
        ASSERT(sum_len <= free_intervals[block].len(), "len more than free interval");
#endif

        for (int u: users) {
            total_score -= users_info[u].calc_score();
            users_info[u].sum_len += change;
            total_score += users_info[u].calc_score();
        }
    }

    void add_user_in_interval(int u, int block, int interval) {
        ASSERT(intervals[block][interval].users.size() + 1 <= L, "failed add");

        total_score -= users_info[u].calc_score() + calc_beams_score(block, interval);

        intervals[block][interval].users.insert(u);
        users_info[u].sum_len += intervals[block][interval].len();
        beams_msk[block][interval] ^= (uint32_t(1) << users_info[u].beam);
        ASSERT(0 <= users_info[u].beam && users_info[u].beam < 32, "invalid beam");

        total_score += users_info[u].calc_score() + calc_beams_score(block, interval);
    }

    void remove_user_in_interval(int u, int block, int interval) {
        total_score -= users_info[u].calc_score() + calc_beams_score(block, interval);

        auto &users = intervals[block][interval].users;
        users.erase(u);
        users_info[u].sum_len -= intervals[block][interval].len();
        beams_msk[block][interval] ^= (uint32_t(1) << users_info[u].beam);
        ASSERT(0 <= users_info[u].beam && users_info[u].beam < 32, "invalid beam");

        total_score += users_info[u].calc_score() + calc_beams_score(block, interval);
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

    ///TEST CASE: K=0 | tests: 666 | score: 94.3685% | 646577/685162 | time: 10949.5ms | max_time: 66.43ms | mean_time: 16.4407ms
    ///TEST CASE: K=1 | tests: 215 | score: 92.6091% | 209102/225790 | time: 2541.55ms | max_time: 20.171ms | mean_time: 11.8212ms
    ///TEST CASE: K=2 | tests: 80 | score: 91.374% | 75993/83167 | time: 907.977ms | max_time: 14.678ms | mean_time: 11.3497ms
    ///TEST CASE: K=3 | tests: 39 | score: 90.7939% | 41984/46241 | time: 428.105ms | max_time: 15.848ms | mean_time: 10.9771ms
    ///TEST CASE: K=4 | tests: 0 | score: -nan% | 0/0 | time: 0ms | max_time: 0ms | mean_time: 0ms
    ///TOTAL: tests: 1000 | score: 93.5884% | 973656/1040360 | time: 14827.1ms | max_time: 66.43ms | mean_time: 14.8271ms

    vector<Interval> annealing() {
        double temp = 1;
        auto is_good = [&](int old_score) {
            return total_score > old_score || rnd.get_d() < exp((total_score - old_score) / temp);
        };

        vector<int> users_order(N);
        iota(users_order.begin(), users_order.end(), 0);
        sort(users_order.begin(), users_order.end(), [&](int lhs, int rhs) {
            return users_info[lhs].rbNeed > users_info[rhs].rbNeed;
        });

        int current_user = 0;

        vector <Interval> answer = get_total_answer();
        int answer_score = total_score;
        int last_update = 0;
        int last_type = -1;
//        cout << "----------" << endl;
        for (int step = 0; step < 10'000; step++) {
            temp *= 0.999999;
//            if (answer_score/100.0 > 0.99*theor_max){ // step > 7000 &&
//                break;
//            }
//            if (step - last_update > 1000){
//                break;
//            }
            if (answer_score < total_score) {
//                if (theor_max > 3000) {
                   // cout << "UP: +" << total_score - answer_score << "| STEP: " << step << " " << last_update << " " << theor_max << " | " << last_type << endl;
                //}
                last_update = step;
                answer_score = total_score;
                answer = get_total_answer();
            }
            last_type = -1;

            if (rnd.get_d() < 0.1) {
                last_type = 1;
                // update interval

                int block = rnd.get(0, free_intervals.size() - 1);
                if (intervals[block].empty()) {
                    continue;
                }

                int interval = rnd.get(0, intervals[block].size() - 1);

                auto flow_over = [&]() {
                    if (interval + 1 < intervals[block].size() && rnd.get_d() < 0.3) {
                        int change = rnd.get(-intervals[block][interval].len(),
                                             intervals[block][interval + 1].len());

                        int old_score = total_score;

                        if (change > 0) {
                            change_interval_len(block, interval + 1, -change);
                            change_interval_len(block, interval, change);
                        } else {
                            change_interval_len(block, interval, change);
                            change_interval_len(block, interval + 1, -change);
                        }

                        if (is_good(old_score)) {
                        } else {
                            if (change > 0) {
                                change_interval_len(block, interval, -change);
                                change_interval_len(block, interval + 1, change);
                            } else {
                                change_interval_len(block, interval + 1, change);
                                change_interval_len(block, interval, -change);
                            }
                            ASSERT(old_score == total_score, "failed back score");
                        }

                        return true;
                    } else {
                        return false;
                    }
                };

                auto change_len = [&]() {
                    if (rnd.get_d() < 0.5) {
                        int change = min(max(-intervals[block][interval].len(), (int) rnd.get(-10, 10)),
                                         free_intervals[block].len() - sum_intervals_len[block] -
                                         intervals[block][interval].len());

                        ASSERT(0 <= intervals[block][interval].len() + change &&
                               intervals[block][interval].len() + change + sum_intervals_len[block] <=
                               free_intervals[block].len(), "kek");

                        if (change == 0) {
                            return false;
                        }

                        int old_score = total_score;

                        change_interval_len(block, interval, change);

                        if (is_good(old_score)) {
                        } else {
                            change_interval_len(block, interval, -change);
                            ASSERT(old_score == total_score, "failed back score");
                        }
                        return true;
                    } else {
                        return false;
                    }
                };

                auto move_interval = [&]() {
                    if (rnd.get_d() < 0.2) {

                        if (interval == 0) {
                            return false;
                        }

                        int old_score = total_score;

                        int len = intervals[block][interval].len();

                        for (int u: intervals[block][interval].users) {
                            total_score -= users_info[u].calc_score();
                            users_info[u].sum_len -= len;
                            //beams_msk[block][interval] ^= (uint32_t(1) << users_info[u].beam);
                            total_score += users_info[u].calc_score();
                        }
                        //ASSERT(beams_msk[block][interval] == 0, "failed beam clear");

                        intervals[block][interval].end = 0;

                        //cout << "abada" << endl;
                        sum_intervals_len[block] -= len;
                        //cout << "caabada" << endl;
                        change_interval_len(block, interval - 1, len);
                        //cout << "dabra" << endl;

                        if (is_good(old_score)) {

                            intervals[block].erase(intervals[block].begin() + interval);
                            beams_msk[block].erase(beams_msk[block].begin() + interval);

                            for (int u = 0; u < N; u++) {
                                if (users_info[u].block == block) {
                                    if (interval == users_info[u].left && interval == users_info[u].right) {
                                        users_info[u].block = users_info[u].left = users_info[u].right = -1;
                                        continue;
                                    }

                                    if (interval == users_info[u].left) {

                                    } else if (interval < users_info[u].left) {
                                        users_info[u].left--;
                                    }

                                    if (interval <= users_info[u].right) {
                                        users_info[u].right--;
                                    }

                                    int left = 1e9;
                                    int right = -1;
                                    for (int i = 0; i < intervals[block].size(); i++) {
                                        if (find(intervals[block][i].users.begin(), intervals[block][i].users.end(),
                                                 u) !=
                                            intervals[block][i].users.end()) {
                                            left = min(left, i);
                                            right = i;
                                        }
                                    }
                                    //cout << left << ' ' << users_info[u].left << endl;
                                    ASSERT(left == users_info[u].left, "are you stupid or something?");
                                    //cout << right << ' ' << users_info[u].right << endl;
                                    ASSERT(right == users_info[u].right, "are you stupid or something?");
                                }
                            }

                            int to_block = rnd.get(0, intervals.size() - 1);
                            intervals[to_block].push_back(SetInterval{0, 0, {}});
                            beams_msk[to_block].push_back(0);

                        } else {
                            intervals[block][interval].end = len;
                            //cout << "kek" << endl;
                            sum_intervals_len[block] += len;
                            //cout << "lol" << endl;
                            change_interval_len(block, interval - 1, -len);
                            //cout << "heh" << endl;
                            //sum_intervals_len[block] += len;

                            for (int u: intervals[block][interval].users) {
                                total_score -= users_info[u].calc_score();
                                users_info[u].sum_len += len;
                                total_score += users_info[u].calc_score();
                            }

                            ASSERT(old_score == total_score, "failed back score");
                        }

                        /*sum_intervals_len[block] -= len;
                        intervals[block].erase(intervals[block].begin() + interval);
                        beams_msk[block].erase(beams_msk[block].begin() + interval);

                        change_interval_len(block, interval - 1, len);*/

                        /*if (interval > 0 && interval + 1 < intervals[block].size()) {
                            if (rnd.get_d() < 0.5) {
                                change_interval_len(block, interval - 1, length(intervals[block][interval]));
                            } else {
                                change_interval_len(block, interval + 1, length(intervals[block][interval]));
                            }
                        } else if (interval > 0) {
                            change_interval_len(block, interval - 1, length(intervals[block][interval]));
                        } else if (interval + 1 < intervals[block].size()) {
                            change_interval_len(block, interval + 1, length(intervals[block][interval]));
                        }*/

                        return true;
                    } else {
                        return false;
                    }
                };

                if (flow_over() || change_len() || move_interval()) {

                }

            } else if (true) {
                last_type = 2;
                // update user

                int u = users_order[current_user];
                current_user = (current_user + 1) % N;
                //int u = rnd.get(0, N - 1);

                if (users_info[u].block == -1) {
                    // no interval

                    auto f = [&](int x, int y) {
                        if (x < y) {
                            return (y - x);
                        } else {
                            return x - y;
                        }
                    };

                    int best_block = -1, best_l = -1, best_r = -1;
                    double best_f = 0;

                    // выделим очень хороший
                    for (int block = 0; block < intervals.size(); block++) {
                        vector<bool> okay(intervals[block].size(), true);
                        for (int i = 0; i < okay.size(); i++) {
                            okay[i] = intervals[block][i].users.size() < L &&
                                      !have_equal_beam(block, i, users_info[u].beam);
                        }

                        for (int l = 0; l < okay.size(); l++) {
                            int len = 0;
                            for (int r = l; r < okay.size() && okay[r]; r++) {
                                // [l, r] okay is true
                                len += intervals[block][r].len();

                                if (best_block == -1 || f(len, users_info[u].rbNeed) <= best_f) {
                                    best_block = block;
                                    best_l = l;
                                    best_r = r;
                                    best_f = f(len, users_info[u].rbNeed);
                                }
                            }
                        }
                    }

                    if (best_block != -1) {

                        int old_score = total_score;

                        new_user_interval(u, best_block, best_l);
                        for (int i = best_l + 1; i <= best_r; i++) {
                            add_right_interval_in_user(u);
                        }

                        if (is_good(old_score)) {

                        } else {
                            remove_all_user_interval(u);
                            ASSERT(old_score == total_score, "failed back score");
                        }
                    }

                    // random choose
                    // some bad
                    /*int block = rnd.get(0, free_intervals.size() - 1);
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
                    }*/

                } else {

                    int block = users_info[u].block;
                    int right = users_info[u].right;
                    int left = users_info[u].left;
                    int beam = users_info[u].beam;

                    auto add_right = [&]() {
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

                            return true;
                        } else {
                            return false;
                        }
                    };

                    auto add_left = [&]() {
                        if (left > 0 &&
                            intervals[block][left - 1].users.size() + 1 <= L &&
                            !have_equal_beam(block, left - 1, beam) &&
                            rnd.get_d() < 0.2) {

                            int old_score = total_score;

                            add_left_interval_in_user(u);

                            if (is_good(old_score)) {

                            } else {
                                remove_left_interval_in_user(u);
                                ASSERT(old_score == total_score, "failed back score");
                            }
                            return true;
                        } else {
                            return false;
                        }
                    };

                    auto remove_right = [&]() {
                        if (left + 1 <= right &&
                            rnd.get_d() < 0.1) {

                            int old_score = total_score;

                            remove_right_interval_in_user(u);

                            if (is_good(old_score)) {

                            } else {
                                add_right_interval_in_user(u);
                                ASSERT(old_score == total_score, "failed back score");
                            }
                            return true;
                        } else {
                            return false;
                        }
                    };

                    auto remove_left = [&]() {
                        if (left + 1 <= right &&
                            rnd.get_d() < 0.1) {

                            int old_score = total_score;

                            remove_left_interval_in_user(u);

                            if (is_good(old_score)) {

                            } else {
                                add_left_interval_in_user(u);
                                ASSERT(old_score == total_score, "failed back score");
                            }
                            return true;
                        } else {
                            return false;
                        }
                    };

                    auto remove_all = [&]() {
                        if (rnd.get_d() < 0.5) {
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
                            return true;
                        } else {
                            return false;
                        }
                    };

                    auto choose_nice_new = [&]() {
                        if (rnd.get_d() < 0.2) {
                            int old_score = total_score;

                            remove_all_user_interval(u);

                            int best_block = -1, best_l = -1, best_r = -1;
                            // choose nice
                            {
                                auto f = [&](int x, int y) {
                                    if (x < y) {
                                        return (y - x);
                                    } else {
                                        return x - y;
                                    }
                                };

                                double best_f = 0;

                                // выделим очень хороший
                                //for (int block = 0; block < intervals.size(); block++)
                                {
                                    int block = rnd.get(0, intervals.size() - 1);
                                    vector<bool> okay(intervals[block].size(), true);
                                    for (int i = 0; i < okay.size(); i++) {
                                        okay[i] = intervals[block][i].users.size() < L &&
                                                  !have_equal_beam(block, i, users_info[u].beam);
                                    }

                                    for (int l = 0; l < okay.size(); l++) {
                                        int len = 0;
                                        for (int r = l; r < okay.size() && okay[r]; r++) {
                                            // [l, r] okay is true
                                            len += intervals[block][r].len();

                                            if (best_block == -1 || f(len, users_info[u].rbNeed) <= best_f) {
                                                best_block = block;
                                                best_l = l;
                                                best_r = r;
                                                best_f = f(len, users_info[u].rbNeed);
                                            }
                                        }
                                    }
                                }
                            }

                            if (best_block == -1) {
                                new_user_interval(u, block, left);
                                for (int i = left + 1; i <= right; i++) {
                                    add_right_interval_in_user(u);
                                }
                                ASSERT(old_score == total_score, "failed back score");
                                return false;
                            }

                            new_user_interval(u, best_block, best_l);
                            for (int i = best_l + 1; i <= best_r; i++) {
                                add_right_interval_in_user(u);
                            }

                            if (is_good(old_score)) {

                            } else {
                                remove_all_user_interval(u);
                                new_user_interval(u, block, left);
                                for (int i = left + 1; i <= right; i++) {
                                    add_right_interval_in_user(u);
                                }
                                ASSERT(old_score == total_score, "failed back score");
                            }
                            return true;
                        } else {
                            return false;
                        }
                    };

                    if (choose_nice_new() || add_right() || add_left() || remove_right() || remove_left() ||
                        remove_all()) {
                    }
                }
            }
        }

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

    return solver.annealing();
}

vector<Interval> Solver(int N, int M, int K, int J, int L,
                        vector<Interval> reservedRBs,
                        vector<UserInfo> userInfos, int test);

vector<Interval> Solver(const TestData &testdata, int test) {
    return Solver(testdata.N, testdata.M, testdata.K, testdata.J, testdata.L, testdata.reservedRBs, testdata.userInfos, test);
}

vector<Interval> Solver(int N, int M, int K, int J, int L,
                        vector<Interval> reservedRBs,
                        vector<UserInfo> userInfos, int test) {

    auto egor_answer = Solver_egor(N, M, K, J, L, reservedRBs, userInfos);

    std::ofstream out("ans_data/case_K="+to_string(K)+"/"+to_string(test)+".txt");
    out << egor_answer.size() << endl;
    for (int i = 0; i < egor_answer.size(); i++){
        out << egor_answer[i].start << " " << egor_answer[i].end << endl;
        out << egor_answer[i].users.size() << endl;
        for (auto user_id: egor_answer[i].users ){
            out << user_id << " ";
        }
        out << endl;
    }
    out.close();

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