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
#define ASSERT(condition, message) /*condition*/ /**THIS CONDITION VERY IMPORTANT */
#endif// MY_DEBUG_MODE
/*int CNT_CALL_GET_LEFT_USER = 0;
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
*/
int THEORY_MAX_SCORE = 1e9;
#include <climits>
class MyBitSet {
uint64_t word[2] = {0, 0};
void flip(uint8_t x) {
word[x / 64] ^= (static_cast<uint64_t>(1) << (x % 64));
}
public:
void insert(uint8_t x) {
ASSERT(0 <= x && x < 128, "invalid x");
ASSERT(!contains(x), "x already insert");
flip(x);
}
void erase(uint8_t x) {
ASSERT(0 <= x && x < 128, "invalid x");
ASSERT(contains(x), "x already erase");
flip(x);
}
void clear() {
word[0] = word[1] = 0;
}
[[nodiscard]] bool contains(uint8_t x) const {
ASSERT(0 <= x && x < 128, "invalid x");
return (word[x / 64] >> (x % 64)) & 1;
}
[[nodiscard]] bool empty() const {
return word[0] == 0 && word[1] == 0;
}
[[nodiscard]] int size() const {
return __builtin_popcountll(word[0]) + __builtin_popcountll(word[1]);
}
class Iterator {
const MyBitSet &object;
uint8_t bit;
public:
using difference_type = std::ptrdiff_t;
using value_type = int;
using pointer = int;
using reference = int;
using iterator_category = std::forward_iterator_tag;
Iterator(uint8_t BIT, const MyBitSet &OBJECT)
: bit(BIT), object(OBJECT) {
}
Iterator &operator++() {
if (bit == 128 || (++bit) == 128) {
return *this;
}
/*while (bit < 128) {
uint64_t val = object.word[bit / 64] >> (bit % 64);
if (val == 0) {
bit += 64 - bit % 64;
} else {
bit += __builtin_ctzll(val);
break;
}
}*/
uint64_t val = object.word[bit / 64] >> (bit % 64);
if (val == 0) {
if (bit < 64) {
bit = 64;
val = object.word[1];
if (val == 0) {
bit += 64 - bit % 64;
} else {
bit += __builtin_ctzll(val);
}
} else {
bit = 128;
}
} else {
bit += __builtin_ctzll(val);
}
return *this;
}
const int operator*() const {
return bit;
}
friend bool
operator==(const Iterator &lhs, const Iterator &rhs) {
return lhs.bit == rhs.bit && &lhs.object == &rhs.object;
}
friend bool
operator!=(const Iterator &lhs, const Iterator &rhs) {
return !(lhs == rhs);
}
};
[[nodiscard]] Iterator begin() const {
return ++Iterator(-1, *this);
}
[[nodiscard]] Iterator end() const {
return Iterator(128, *this);
}
friend bool operator==(const MyBitSet &lhs, const MyBitSet &rhs) {
return lhs.word[0] == rhs.word[0] && lhs.word[1] == rhs.word[1];
}
friend bool operator!=(const MyBitSet &lhs, const MyBitSet &rhs) {
return !(lhs == rhs);
}
friend MyBitSet operator|(MyBitSet lhs, const MyBitSet &rhs) {
lhs.word[0] |= rhs.word[0];
lhs.word[1] |= rhs.word[1];
return lhs;
}
friend MyBitSet operator&(MyBitSet lhs, const MyBitSet &rhs) {
lhs.word[0] &= rhs.word[0];
lhs.word[1] &= rhs.word[1];
return lhs;
}
friend MyBitSet operator^(MyBitSet lhs, const MyBitSet &rhs) {
lhs.word[0] ^= rhs.word[0];
lhs.word[1] ^= rhs.word[1];
return lhs;
}
};
/*class MyBitSet {
static constexpr std::size_t reserved_size = 128;
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
void clear() {
for (int i = 0; i < bits_size; i++) {
bits[i] = 0;
}
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
friend MyBitSet operator|(MyBitSet lhs, const MyBitSet &rhs) {
for (int i = 0; i < bits_size; i++) {
lhs.bits[i] |= rhs.bits[i];
}
return lhs;
}
friend MyBitSet operator&(MyBitSet lhs, const MyBitSet &rhs) {
for (int i = 0; i < bits_size; i++) {
lhs.bits[i] &= rhs.bits[i];
}
return lhs;
}
friend MyBitSet operator^(MyBitSet lhs, const MyBitSet &rhs) {
for (int i = 0; i < bits_size; i++) {
lhs.bits[i] ^= rhs.bits[i];
}
return lhs;
}
};*/
#include <random>
struct randomizer {
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
#include <vector>
#include <iostream>
struct SelectionRandomizer {
randomizer rnd;
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
SelectionRandomizer(std::vector<int> powers) : kit(powers.size()) {
ASSERT(kit.size() > 0, "invalid kit");
for (int i = 0; i < powers.size(); i++) {
kit[i] = {i, powers[i]};
ASSERT(powers[i] >= 0, "invalid power");
}
}
void reset_rnd() {
rnd = randomizer();
}
int select() {
//double p = rnd.get_d() * power_sum();
int p = rnd.get(0, power_sum());
int sum = 0;
for (auto [id, power]: kit) {
sum += power;
if (p <= sum) {
return id;
}
}
ASSERT(false, "select failed");
return -1;
}
friend std::ostream &operator<<(std::ostream &output, SelectionRandomizer &selection) {
output << "{ ";
for (int i = 0; i < selection.kit.size(); i++) {
output << selection.kit[i].second;
if (i + 1 < selection.kit.size()) {
output << ", ";
}
}
output << "}";
return output;
}
};
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
//ASSERT(QueryPerformanceFrequency(&perf), "call to QueryPerformanceFrequency fails");
QueryPerformanceFrequency(&perf) return perf.QuadPart;
}
uint64_t get_ticks() {
LARGE_INTEGER ticks;
//ASSERT(QueryPerformanceCounter(&ticks), "call to QueryPerformanceCounter fails");
QueryPerformanceCounter(&ticks) return ticks.QuadPart;
}
#elif defined(__linux__) || defined(__APPLE__)
#include <sys/time.h>
uint64_t calc_performance_frequency() {
return 1'000'000;// колво микросекунд в секунде
}
uint64_t get_ticks() {
timeval ticks;
// вернет -1 в случае ошибки
//ASSERT(gettimeofday(&ticks, NULL) == 0, "call to gettimeofday fails");
gettimeofday(&ticks, NULL);
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
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
namespace fs = std::filesystem;
using namespace std;
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
class Snapshoter {
public:
std::string name;
int theor_max;
std::string _write_directory;
int frame = 0;
bool init = false;
std::vector<string> frame_names;
Snapshoter() {}
std::vector<Interval> last_intervals;
std::vector<int> scores;
TestData test_data;
Snapshoter(int K, int test, int theor_max, TestData test_data, std::string name = "solution_snapshots") : name(name), theor_max(theor_max), test_data(test_data) {
fs::create_directory("movies_data");
fs::create_directory("movies_data/" + name);
_write_directory = "movies_data/" + name + "/" + to_string(K) + "_" + to_string(test) + "/";
fs::create_directory(_write_directory);
init = true;
}
void write(const std::vector<std::vector<Interval>> &intervals, const string &frame_name = "", bool combine_same = true) {
write(unblock_ans(intervals), frame_name, combine_same);
}
bool same_as_last(std::vector<Interval> intervals) {
sort(intervals.begin(), intervals.end(), [&](const auto &lhs, const auto &rhs) {
return lhs.start < rhs.start;
});
if (last_intervals.size() == 0) return false;
if (last_intervals.size() != intervals.size()) return false;
for (int i = 0; i < last_intervals.size(); i++) {
if (last_intervals[i].start != intervals[i].start || last_intervals[i].end != intervals[i].end) {
return false;
}
sort(intervals[i].users.begin(), intervals[i].users.end());
sort(last_intervals[i].users.begin(), last_intervals[i].users.end());
for (int g = 0; g < intervals[i].users.size(); g++) {
if (intervals[i].users[g] != last_intervals[i].users[g]) {
return false;
}
}
}
return true;
}
void write(const std::vector<Interval> &intervals, const string &frame_name = "", bool combine_same = true) {
if (same_as_last(intervals) && false) return;
last_intervals = intervals;
frame_names.push_back(frame_name);
int score = get_solution_score(test_data, intervals);
scores.push_back(score);
std::ofstream out(_write_directory + to_string(frame) + ".txt");
out << intervals.size() << endl;
for (int i = 0; i < intervals.size(); i++) {
out << intervals[i].start << " " << intervals[i].end << endl;
out << intervals[i].users.size() << endl;
for (auto user_id: intervals[i].users) {
out << user_id << " ";
}
out << endl;
}
out.close();
// cout << "UPDATING FRAME: " << frame << endl;
frame++;
}
~Snapshoter() {
if (!init) return;
std::ofstream out(_write_directory + "data" + ".txt");
out << "{" << endl;
out << "\"theor_max\": " << theor_max << "," << endl;
out << "\"frames\": " << frame << "," << endl;
out << "\"frame_names\": [";
for (int i = 0; i < frame_names.size(); i++) {
out << "\"" + frame_names[i] << "\"";
if (i + 1 != frame_names.size()) {
out << ",";
}
}
out << "]"
<< "," << endl;
out << "\"scores\": [";
for (int i = 0; i < scores.size(); i++) {
out << "\"" + to_string(scores[i]) << "\"";
if (i + 1 != scores.size()) {
out << ",";
}
}
out << "]" << endl;
out << "}" << endl;
out.close();
}
};
Snapshoter snapshoter;
//#define ENABLE_SNAPSHOT
#ifdef ENABLE_SNAPSHOT
#define SNAP(x) x
#else
#define SNAP(x)
#endif
int BEAM_MAX_AMOUNT = 32;
int get_blocks_amount(int M, vector<Interval>& reservedRBs) {
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
ASSERT(intervals.back().users.empty(), "users empty");
intervals.pop_back();
}
int start = 0;
if (intervals.size()) {
start = intervals.back().end;
}
ASSERT(intervals.size(), "intervals empty");
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
ASSERT(selected_index != -1, "invalid selected index");
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
std::vector<bool>is_user_used(N, false);
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
SNAP(snapshoter.write(pre_answer, "greedy"));
// СЋР·Р°РµРј OWNEDBY РґРѕ РєРѕРЅС†Р°!!!
// СЃС‡РёС‚Р°РµРј РєРѕР»-РІРѕ СЃРІРѕР±РѕРґРЅС‹С… СЃР»РѕС‚РѕРІ.
// РќР°Р±РёСЂР°РµРј
//-------------------------
std::vector<std::pair<int, int>> candidates;
std::set<int> to_delete;
//        if (true) {
//            for (auto user_id: activeUsers[pick_i]) {
//                float curr_len = pre_answer[pick_i][current_sub_interval[pick_i]].end -
//                                 pre_answer[pick_i][current_sub_interval[pick_i]].start;
//                float need_more = userInfos[user_id].rbNeed - rbSuplied[user_id];
//                if (need_more < curr_len * coef) {
//                    to_delete.insert(user_id);
//                }
//            }
//
//
//            int have_full = 0;
//            for (auto &user: userInfos) {
//                if (!is_user_used[user.id]) {
//                    if (beamOwnedBy[pick_i][user.beam] == -1 &&
//                        user.rbNeed - rbSuplied[user.id] > pre_answer[pick_i][current_sub_interval[pick_i]].end -
//                                                                   pre_answer[pick_i][current_sub_interval[pick_i]].start) {
//                        have_full++;
//                    }
//                    if (beamOwnedBy[pick_i][user.beam] != -1) {
//                        int owned_by_id = beamOwnedBy[pick_i][user.beam];
//                        if (user.rbNeed - rbSuplied[user.id] > userInfos[owned_by_id].rbNeed - rbSuplied[owned_by_id]) {
//                            have_full++;
//                        }
//                    }
//                }
//            }
//            std::vector<pair<int, int>>
//                    to_delete_sorted;
//            for (auto user_id: to_delete) {
//                to_delete_sorted.push_back({userInfos[user_id].rbNeed - rbSuplied[user_id], user_id});
//            }
//            sort(to_delete_sorted.begin(), to_delete_sorted.end());
//            if (have_full) {
//                for (auto [coeffi, user_id]: to_delete_sorted) {
//                    activeUsers[pick_i].erase(user_id);
//                    beamOwnedBy[pick_i][userInfos[user_id].beam] = -1;
//                    have_full--;
//                    if (have_full == 0) {
//                        break;
//                    }
//                }
//            }
//            candidates.clear();
//        }
for (auto &user: userInfos) {
if (!is_user_used[user.id] && beamOwnedBy[pick_i][user.beam] == -1) {
ASSERT(rbSuplied[user.id] == 0, "rb suplied is zero");
candidates.push_back({user.rbNeed - rbSuplied[user.id], user.id});
}
}
//-----------------------
sort(candidates.begin(), candidates.end(), greater<>());
int get_more = L - activeUsers[pick_i].size();
std::set<int> new_users;
for (int g = 0; g < (int) candidates.size(); g++) {
if (get_more == 0) break;
if (beamOwnedBy[pick_i][userInfos[candidates[g].second].beam] == -1) {
activeUsers[pick_i].insert(candidates[g].second);
new_users.insert(candidates[g].second);
is_user_used[candidates[g].second] = true;
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
ASSERT(need - now > 0, "invalid smth");
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
metric -= i;
} else {
int will_supplied_if_moved = rbSuplied[user_id] + i;
if (will_supplied_if_moved >= userInfos[user_id].rbNeed) {
metric += len - i;
metric -= will_supplied_if_moved - userInfos[user_id].rbNeed;
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
if (best_ind != 0) {// можно и убрать, просто для удобства и гарантии // даёт + 30. Можно убрать??
minus = best_ind;
//                cout << "UPD!" << " " << best_ind << endl;
pre_answer[pick_i][current_sub_interval[pick_i] - 1].end += best_ind;
pre_answer[pick_i][current_sub_interval[pick_i]].start += best_ind;
for (auto user_id: pre_answer[pick_i][current_sub_interval[pick_i] - 1].users) {
rbSuplied[user_id] += best_ind;
if (rbSuplied[user_id] >= userInfos[user_id].rbNeed) {
if (activeUsers[pick_i].count(user_id)) {
beamOwnedBy[pick_i][userInfos[user_id].beam] = -1;
activeUsers[pick_i].erase(user_id);
}
}
}
for (auto &user: userInfos) {
if (!is_user_used[user.id] && beamOwnedBy[pick_i][user.beam] == -1) {
ASSERT(rbSuplied[user.id] == 0, "rb suplied is zero");
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
is_user_used[candidates[g].second] = true;
beamOwnedBy[pick_i][userInfos[candidates[g].second].beam] = candidates[g].second;
get_more--;
}
}
SNAP(snapshoter.write(pre_answer, "moving border"));
}
}
for (auto user_id: activeUsers[pick_i]) {
rbSuplied[user_id] += pre_answer[pick_i][current_sub_interval[pick_i]].end -
pre_answer[pick_i][current_sub_interval[pick_i]].start;
pre_answer[pick_i][current_sub_interval[pick_i]].users.push_back(user_id);
SNAP(snapshoter.write(pre_answer, "adding user " + to_string(user_id)));
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
//    return;
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
// ÃƒÂ Ãƒâ€˜Ã¢â‚¬ÂºÃƒÂ Ãƒâ€šÃ‚Â±ÃƒÂÃ‚Â¡ÃƒÂÃ¢â‚¬Å¡ÃƒÂ Ãƒâ€šÃ‚ÂµÃƒÂ Ãƒâ€šÃ‚Â·ÃƒÂ Ãƒâ€šÃ‚Â°ÃƒÂ Ãƒâ€šÃ‚ÂµÃƒÂ Ãƒâ€˜Ã‹Å“ ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â¦ÃƒÂ ÃƒÂÃ¢â‚¬ ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂÃ‚Â¡ÃƒÂÃ†â€™ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡, ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡ÃƒÂ Ãƒâ€™Ã¢â‚¬ËœÃƒÂ Ãƒâ€šÃ‚Â°ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‹Å“ÃƒÂ Ãƒâ€˜Ã‹Å“ ÃƒÂ Ãƒâ€˜Ã¢â‚¬â€œÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€šÃ‚Â»ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ ÃƒÂÃ¢â‚¬ ÃƒÂ Ãƒâ€šÃ‚Âµ
for (int i = 0; i < N; i++) {
if (mi[i] != 10000) {// ÃƒÂ Ãƒâ€˜Ã‹Å“ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€šÃ‚Â¶ÃƒÂ ÃƒÂÃ¢â‚¬Â¦ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ ÃƒÂÃ‚Â¡ÃƒÂÃ†â€™ÃƒÂ Ãƒâ€™Ã¢â‚¬ËœÃƒÂ Ãƒâ€šÃ‚ÂµÃƒÂ Ãƒâ€šÃ‚Â»ÃƒÂ Ãƒâ€šÃ‚Â°ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡ÃƒÂÃ‚Â¡ÃƒÂÃ…  ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡ÃƒÂ Ãƒâ€šÃ‚Â°ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â¡ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ ÃƒÂ Ãƒâ€šÃ‚Â±ÃƒÂÃ‚Â¡ÃƒÂÃ…  mi == 0
bool is_start = (mi[i] == 0);
int best_score_gain = -1;
int best_receiver = -1;
int current_len = solution[mi[i]].end - solution[mi[i]].start;
if (suplied[i] == userInfos[i].rbNeed){
continue;
}
int minus = min(suplied[i], userInfos[i].rbNeed) - min(suplied[i] - current_len, userInfos[i].rbNeed);
bool is_empty_winner = false;
if (!is_start) {
for (auto &give_to: ma_set[mi[i] - 1]) {
if (userInfos[i].beam == userInfos[give_to].beam ||
beamOwnedBy[mi[i]][userInfos[give_to].beam] == -1) {
// ÃƒÂ Ãƒâ€˜Ã…Â¾ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€šÃ‚Â»ÃƒÂÃ‚Â¡ÃƒÂÃ… ÃƒÂ Ãƒâ€˜Ã¢â‚¬ÂÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€™Ã¢â‚¬ËœÃƒÂ Ãƒâ€˜Ã¢â‚¬ËœÃƒÂ ÃƒÂÃ¢â‚¬Â¦ÃƒÂ Ãƒâ€šÃ‚Â°ÃƒÂ Ãƒâ€˜Ã¢â‚¬ÂÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ ÃƒÂÃ¢â‚¬ ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â¹ÃƒÂ Ãƒâ€šÃ‚Âµ beam. ÃƒÂ Ãƒâ€˜Ã…Â¸ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€˜Ã‹Å“ ÃƒÂ Ãƒâ€šÃ‚Â»ÃƒÂÃ‚Â¡ÃƒÂÃ¢â‚¬Â¹ÃƒÂ Ãƒâ€šÃ‚Â±ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â¹ÃƒÂ Ãƒâ€šÃ‚Âµ
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
// ÃƒÂ Ãƒâ€˜Ã…Â¾ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€šÃ‚Â»ÃƒÂÃ‚Â¡ÃƒÂÃ… ÃƒÂ Ãƒâ€˜Ã¢â‚¬ÂÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€™Ã¢â‚¬ËœÃƒÂ Ãƒâ€˜Ã¢â‚¬ËœÃƒÂ ÃƒÂÃ¢â‚¬Â¦ÃƒÂ Ãƒâ€šÃ‚Â°ÃƒÂ Ãƒâ€˜Ã¢â‚¬ÂÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ ÃƒÂÃ¢â‚¬ ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â¹ÃƒÂ Ãƒâ€šÃ‚Âµ beam. ÃƒÂ Ãƒâ€˜Ã…Â¸ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€˜Ã‹Å“ ÃƒÂ Ãƒâ€šÃ‚Â»ÃƒÂÃ‚Â¡ÃƒÂÃ¢â‚¬Â¹ÃƒÂ Ãƒâ€šÃ‚Â±ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â¹ÃƒÂ Ãƒâ€šÃ‚Âµ
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
solution[mi[i]].users[iter-solution[mi[i]].users.begin()] = best_receiver;
//                    solution[mi[i]].users.erase(iter);// optimize_it
//                    solution[mi[i]].users.push_back(best_receiver);
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
// ÃƒÂ Ãƒâ€˜Ã¢â‚¬ÂºÃƒÂ Ãƒâ€šÃ‚Â±ÃƒÂÃ‚Â¡ÃƒÂÃ¢â‚¬Å¡ÃƒÂ Ãƒâ€šÃ‚ÂµÃƒÂ Ãƒâ€šÃ‚Â·ÃƒÂ Ãƒâ€šÃ‚Â°ÃƒÂ Ãƒâ€šÃ‚ÂµÃƒÂ Ãƒâ€˜Ã‹Å“ ÃƒÂ Ãƒâ€˜Ã¢â‚¬â€œÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€šÃ‚Â»ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ ÃƒÂÃ¢â‚¬ ÃƒÂÃ‚Â¡Ãƒâ€˜Ã¢â‚¬Å“, ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡ÃƒÂ Ãƒâ€™Ã¢â‚¬ËœÃƒÂ Ãƒâ€šÃ‚Â°ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‹Å“ÃƒÂ Ãƒâ€˜Ã‹Å“ ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â¦ÃƒÂ ÃƒÂÃ¢â‚¬ ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂÃ‚Â¡ÃƒÂÃ†â€™ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡ÃƒÂÃ‚Â¡Ãƒâ€˜Ã¢â‚¬Å“
for (int i = 0; i < N; i++) {
if (ma[i] != -10000) {// ÃƒÂ Ãƒâ€˜Ã‹Å“ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€šÃ‚Â¶ÃƒÂ ÃƒÂÃ¢â‚¬Â¦ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ ÃƒÂÃ‚Â¡ÃƒÂÃ†â€™ÃƒÂ Ãƒâ€™Ã¢â‚¬ËœÃƒÂ Ãƒâ€šÃ‚ÂµÃƒÂ Ãƒâ€šÃ‚Â»ÃƒÂ Ãƒâ€šÃ‚Â°ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡ÃƒÂÃ‚Â¡ÃƒÂÃ…  ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡ÃƒÂ Ãƒâ€šÃ‚Â°ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â¡ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ ÃƒÂ Ãƒâ€šÃ‚Â±ÃƒÂÃ‚Â¡ÃƒÂÃ…  mi == 0
bool is_last = ma[i] + 1 == solution.size();
int best_score_gain = -1;
int best_receiver = -1;
int current_len = solution[ma[i]].end - solution[ma[i]].start;
if (suplied[i] == userInfos[i].rbNeed){
continue;
}
int minus = min(suplied[i], userInfos[i].rbNeed) - min(suplied[i] - current_len, userInfos[i].rbNeed);
bool is_empty_winner = false;
if (!is_last) {
for (auto &give_to: mi_set[ma[i] + 1]) {
if (userInfos[i].beam == userInfos[give_to].beam ||
beamOwnedBy[ma[i]][userInfos[give_to].beam] == -1) {
// ÃƒÂ Ãƒâ€˜Ã…Â¾ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€šÃ‚Â»ÃƒÂÃ‚Â¡ÃƒÂÃ… ÃƒÂ Ãƒâ€˜Ã¢â‚¬ÂÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€™Ã¢â‚¬ËœÃƒÂ Ãƒâ€˜Ã¢â‚¬ËœÃƒÂ ÃƒÂÃ¢â‚¬Â¦ÃƒÂ Ãƒâ€šÃ‚Â°ÃƒÂ Ãƒâ€˜Ã¢â‚¬ÂÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ ÃƒÂÃ¢â‚¬ ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â¹ÃƒÂ Ãƒâ€šÃ‚Âµ beam. ÃƒÂ Ãƒâ€˜Ã…Â¸ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€˜Ã‹Å“ ÃƒÂ Ãƒâ€šÃ‚Â»ÃƒÂÃ‚Â¡ÃƒÂÃ¢â‚¬Â¹ÃƒÂ Ãƒâ€šÃ‚Â±ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â¹ÃƒÂ Ãƒâ€šÃ‚Âµ
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
// ÃƒÂ Ãƒâ€˜Ã…Â¾ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€šÃ‚Â»ÃƒÂÃ‚Â¡ÃƒÂÃ… ÃƒÂ Ãƒâ€˜Ã¢â‚¬ÂÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€™Ã¢â‚¬ËœÃƒÂ Ãƒâ€˜Ã¢â‚¬ËœÃƒÂ ÃƒÂÃ¢â‚¬Â¦ÃƒÂ Ãƒâ€šÃ‚Â°ÃƒÂ Ãƒâ€˜Ã¢â‚¬ÂÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ ÃƒÂÃ¢â‚¬ ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â¹ÃƒÂ Ãƒâ€šÃ‚Âµ beam. ÃƒÂ Ãƒâ€˜Ã…Â¸ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã…Â¡ÃƒÂ Ãƒâ€˜Ã¢â‚¬Â¢ÃƒÂ Ãƒâ€˜Ã‹Å“ ÃƒÂ Ãƒâ€šÃ‚Â»ÃƒÂÃ‚Â¡ÃƒÂÃ¢â‚¬Â¹ÃƒÂ Ãƒâ€šÃ‚Â±ÃƒÂÃ‚Â¡ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â¹ÃƒÂ Ãƒâ€šÃ‚Âµ
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
}
//                    cerr << "PUTTING" << best_receiver << "(" << userInfos[best_receiver].beam << ")" << " instead of " << i << "(" << userInfos[i].beam << ")" << " " << mi[i] << endl;
auto iter = find(solution[ma[i]].users.begin(), solution[ma[i]].users.end(), i);
solution[ma[i]].users[iter-solution[ma[i]].users.begin()] = best_receiver;
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
const vector<UserInfo> &userInfos, vector<vector<Interval>> &solution, bool do_it = true) {
//    std::vector<int> suplied(N, 0);
//    get_solution_score_light(N, solution, userInfos, suplied);
std::vector<bool> was(N, false);
std::vector<int> mi_ind(N, 10000);
std::vector<int> ma_ind(N, -10000);
for (int i = 0; i < solution.size(); i++) {
for (int g = 0; g < solution[i].size(); g++) {
for (auto user_id: solution[i][g].users) {
mi_ind[user_id] = min(mi_ind[user_id], g);
ma_ind[user_id] = max(ma_ind[user_id], g);
was[user_id] = true;
}
}
}
set<pair<int, int>, greater<>> empty;
for (int i = 0; i < N; i++) {
if (!was[i]) {
empty.insert({userInfos[i].rbNeed, i});
}
}
if (true) {
auto pre_reserved = reservedRBs;
sort(pre_reserved.begin(), pre_reserved.end(), [&](auto const &lhs, auto const &rhs) {
return lhs.start < rhs.start;
});
set<int> uniq_beams;
for (int i = 0; i < N; i++) {
uniq_beams.insert(userInfos[i].beam);
}
set<pair<int, int>, greater<>>
free_spaces;
vector<pair<int, int>> space_sizes;
std::vector<std::vector<int>>pre_placed(solution.size());
std::vector<::uint32_t>pre_placed_beams_bits(solution.size(), 0);
bool new_version = do_it;
std::vector<bool>was_empty_space(solution.size(), false);
std::vector<bool>was_full_interval(solution.size(), false); // we took whole interval to free space
vector<bool>ignore_those(128, false);
{
std::vector<pair<int,int>>start_ends;
if (!pre_reserved.empty()) {
if (pre_reserved[0].start != 0) {
start_ends.push_back({0,pre_reserved[0].start});
space_sizes.push_back({pre_reserved[0].start, 0});
}
for (int i = 1; i < pre_reserved.size(); i++) {
start_ends.push_back({pre_reserved[i-1].end, pre_reserved[i].start});
space_sizes.push_back({pre_reserved[i].start - pre_reserved[i - 1].end, pre_reserved[i-1].end});
}
if (pre_reserved.back().end != M) {
start_ends.push_back({pre_reserved.back().end, M});
space_sizes.push_back({M - pre_reserved.back().end, pre_reserved.back().end});
}
} else {
start_ends.push_back({0,M});
space_sizes.push_back({M, 0});
}
vector<vector<Interval>>new_solution(solution.size());
for (int i = 0; i < solution.size(); i++){
if (solution[i].empty()) continue ;
int picked_ind_to_place = -1;
for (int j = 0; j < space_sizes.size(); j++){
bool ok = true;
for (int g = 0; g < solution[i].size(); g++){
if (solution[i][g].start < start_ends[j].first || solution[i][g].end > start_ends[j].second){
ok = false;
break;
}
}
if (ok){
picked_ind_to_place = j;
break;
}
}
if (picked_ind_to_place == -1){
}
new_solution[picked_ind_to_place] = solution[i];
}
solution = new_solution;
if (new_version){
}
vector<int> pre_supplied(N, 0);
for (int i = 0; i < solution.size(); i++) {
int ma = 0;
for (int g = 0; g < solution[i].size(); g++) {
int cur_len = solution[i][g].end - solution[i][g].start;
for (auto user_id: solution[i][g].users) {
if (g + 1 == solution[i].size()) {
//ma = max(ma, userInfos[user_id].rbNeed - pre_supplied[user_id]);
//                            if (pre_supplied[user_id] + cur_len >= userInfos[user_id].rbNeed) { // ÃÅ“ÃÅ¾Ãâ€“Ãâ€¢ÃÅ“ ÃÂ£Ãâ€ºÃÂ£ÃÂ§ÃÂ¨ÃËœÃÂ¢ÃÂ¬
if (new_version){
if (userInfos[user_id].rbNeed - pre_supplied[user_id] >= cur_len){
pre_placed_beams_bits[i]|=(1u<<userInfos[user_id].beam);
ignore_those[user_id] = true;
pre_placed[i].push_back(user_id);
} else {
ma = max(ma, userInfos[user_id].rbNeed - pre_supplied[user_id]);
ma = min(ma, cur_len); // shouldn't be possible
}
} else {
ma = max(ma, userInfos[user_id].rbNeed - pre_supplied[user_id]);
ma = min(ma, cur_len);
}
//                            }
} else {
pre_supplied[user_id] += cur_len;
}
}
}
//ma = max(0,ma);
if (solution[i].size() != 0) {
if (ma > 0) { // can turn off?
ASSERT(start_ends[i].second - solution[i].back().start - ma >= 0, "W");
solution[i].back().end = solution[i].back().start + ma;
free_spaces.insert({start_ends[i].second - solution[i].back().start - ma, i});
} else {
// doesnt change score. Strange
//                        cout << "HERE" << endl;
ASSERT(start_ends[i].second - solution[i].back().start - ma >= 0, "STRANGE");
free_spaces.insert({start_ends[i].second - solution[i].back().start - ma, i});
was_full_interval[i] = true;
solution[i].pop_back();
}
} else {
ASSERT(space_sizes[i].first > 0, "kek");
was_empty_space[i] = true;
was_full_interval[i] = true;
free_spaces.insert({space_sizes[i].first, i});
}
}
}
map<int, set<pair<int, int>>> possible_pre_free_spaces;
for (int i = 0; i < solution.size(); i++) {
int ma = 0;
for (int g = 0; g < solution[i].size(); g++) {
std::set<int> beam_was;
if (solution[i][g].users.size() == L) continue;
for (auto user_id: solution[i][g].users) {
beam_was.insert(userInfos[user_id].beam);
}
}
}
std::set<pair<int, int>, greater<>>
space_left_q;
for (int i = 0; i < solution.size(); i++) {
if (solution[i].size()) {
space_left_q.insert({space_sizes[i].first, i});
}
}
std::vector<std::set<int>> activeUsers(solution.size());
std::vector<int> current_sub_interval(solution.size(), 0);
std::vector<std::set<int>> activeBeams(solution.size());
std::vector<std::vector<bool>> keep_or_not(solution.size());
std::vector<int> start_sizes(solution.size());
for (int i = 0; i < solution.size(); i++) {
start_sizes[i] = solution[i].size();
keep_or_not[i] = std::vector<bool>(solution[i].size(), true);
}
int iter = 0;
std::vector<bool>ever_picked(solution.size(), false);
//#ifdef MY_DEBUG_MODE
//        auto honest_cnt = [&](){
//            int j = 0;
//            for (int i = 0; i < solution.size(); i++){
//                j+=solution[i].size();
//            }
//            for (int i = 0; i < keep_or_not.size(); i++){
//                for(int g = 0; g < keep_or_not[i].size(); g++){
//                     if (!keep_or_not[i][g]){
//                         j--;
//                     }
//                }
//            }
//            return j;
//        };
//#undef MY_DEBUG_MODE
while (!space_left_q.empty()) {
//            int honest = honest_cnt();
//            if (honest > J){
//                ::abort();
//            }
iter++;
int space_left = space_left_q.begin()->first;
int pick_i = space_left_q.begin()->second;
space_left_q.erase(space_left_q.begin());
int curr = current_sub_interval[pick_i];
if (curr >= start_sizes[pick_i]) { // +1
continue;
}
int curr_len = solution[pick_i][curr].end - solution[pick_i][curr].start;
std::vector<int> need_to_find_new;
MyBitSet need_to_find_new_set_bits;
bool force_hold = false;
int already = 0;
if (curr != 0) {
already = solution[pick_i][curr - 1].users.size();
}
for (auto user_id: solution[pick_i][curr].users) {
//            if (!activeUsers[pick_i].count(user_id) && ma_ind[user_id]-mi_ind[user_id] > 0) {
//                force_hold = true;
//                break;
//            }
if (!activeUsers[pick_i].count(user_id)) {
if (activeBeams[pick_i].count(userInfos[user_id].beam) || already >= L) {// BETTER SOLUTION POSSIBLE
if (ma_ind[user_id] - mi_ind[user_id] > 0) {
force_hold = true;
break;
}
need_to_find_new.push_back(user_id);
need_to_find_new_set_bits.insert(user_id);
} else {
already++;
}
}
}
sort(need_to_find_new.begin(), need_to_find_new.end(), [&](const auto &lhs, const auto &rhs) {
if (userInfos[lhs].rbNeed == userInfos[rhs].rbNeed) {
return lhs < rhs;
}
return userInfos[lhs].rbNeed > userInfos[rhs].rbNeed;
});
map<pair<int, int>, int> place_get_amount;
bool can_place_all = true;
int picked = -1;
std::vector<int>candidates_placed_in_theor_interval(solution.size(), 0);
for (int user_id: need_to_find_new) {
if (ignore_those[user_id]){
can_place_all = false;
}
bool placed = false;
for (auto &place_to_get: possible_pre_free_spaces[userInfos[user_id].beam]) {
int place_to_get_len = solution[place_to_get.first][place_to_get.second].end - solution[place_to_get.first][place_to_get.second].start;
if (solution[place_to_get.first][place_to_get.second].users.size() +
place_get_amount[place_to_get] <
L &&
place_to_get_len >= userInfos[user_id].rbNeed) {
place_get_amount[place_to_get]++;
placed = true;
break;
}
}
if (!placed) {
if (free_spaces.empty()) {
can_place_all = false;
} else {
if (new_version){
int can_place_this = false;
if (picked == -1){
can_place_all = false;
} else {
for (auto &free_space: free_spaces) {
if (picked != -1 and free_space.second != picked) continue;
if (userInfos[user_id].rbNeed <= free_space.first) {
if ((pre_placed_beams_bits[free_space.second] & (1u << userInfos[user_id].beam)) == 0 && pre_placed[free_space.second].size() + candidates_placed_in_theor_interval[free_space.second] < L) {
can_place_this = true;
candidates_placed_in_theor_interval[free_space.second]++;
picked = free_space.second;
break;
}
} else {
break;// CORRECT?
}
}
}
if (!can_place_this){
can_place_all = false;
}
} else {
int sz = free_spaces.begin()->first;
if (sz < userInfos[user_id].rbNeed) {
can_place_all = false;
} else {
can_place_all = true;
}
break;
}
}
}
}
if (can_place_all && !force_hold) {
for (int user_id: need_to_find_new) {
bool placed = false;
for (auto place_to_get: possible_pre_free_spaces[userInfos[user_id].beam]) {
int place_to_get_len = solution[place_to_get.first][place_to_get.second].end - solution[place_to_get.first][place_to_get.second].start;
if (solution[place_to_get.first][place_to_get.second].users.size() < L && place_to_get_len >= userInfos[user_id].rbNeed) {
solution[place_to_get.first][place_to_get.second].users.push_back(user_id);
possible_pre_free_spaces[userInfos[user_id].beam].erase(place_to_get);
if (solution[place_to_get.first][place_to_get.second].users.size() == L) {
for (auto beam: uniq_beams) {
possible_pre_free_spaces[beam].erase(place_to_get);
}
}
placed = true;
break;
}
}
if (!placed) {
if (free_spaces.empty()) {
ASSERT(false, "false");
} else {
if (new_version){
{
int sz = free_spaces.begin()->first;
ASSERT(userInfos[user_id].rbNeed <= free_spaces.begin()->first, "STRANGE");
}
int user_need = userInfos[user_id].rbNeed;
int picked_free_place_donor = -1;
pair<int,int>free_space_picked;
for (auto& free_space: free_spaces){
if (userInfos[user_id].rbNeed <= free_space.first) {
if ((pre_placed_beams_bits[free_space.second]&(1u<<userInfos[user_id].beam)) == 0 && pre_placed[free_space.second].size() < L) {
picked_free_place_donor = free_space.second;
free_space_picked = free_space;
break;
}
}
}
ASSERT(picked_free_place_donor >= 0, "NOT PICKED? WTF!!");
if (solution[picked_free_place_donor].empty()) {
solution[picked_free_place_donor].push_back({space_sizes[picked_free_place_donor].second, space_sizes[picked_free_place_donor].second + user_need, pre_placed[picked_free_place_donor]});
} else {
solution[picked_free_place_donor].push_back(
{solution[picked_free_place_donor].back().end,
solution[picked_free_place_donor].back().end + user_need,
pre_placed[picked_free_place_donor]});
}
free_spaces.erase(free_space_picked);
if (free_space_picked.first - user_need > 0) {
free_spaces.insert({free_space_picked.first - user_need, picked_free_place_donor});
}
ever_picked[picked_free_place_donor] = true;
solution[picked_free_place_donor].back().users.push_back(user_id);
for (int beam: uniq_beams) {// if L != 1
if (beam != userInfos[user_id].beam && (pre_placed_beams_bits[picked_free_place_donor]&(1u<<beam)) == 0) {
possible_pre_free_spaces[beam].insert({picked_free_place_donor,
solution[picked_free_place_donor].size() -
1});
}
}
if (solution[picked_free_place_donor].back().users.size() == L){
for (auto beam: uniq_beams) {
possible_pre_free_spaces[beam].erase({picked_free_place_donor, solution[picked_free_place_donor].size()-1});
}
}
} else {
int sz = free_spaces.begin()->first;
if (sz < userInfos[user_id].rbNeed) {
ASSERT(false, "false");
} else {
int user_need = userInfos[user_id].rbNeed;
int picked_free_place_donor = free_spaces.begin()->second;
if (solution[picked_free_place_donor].empty()) {
solution[picked_free_place_donor].push_back({space_sizes[picked_free_place_donor].second, space_sizes[picked_free_place_donor].second + user_need, {}});
} else {
solution[picked_free_place_donor].push_back(
{solution[picked_free_place_donor].back().end,
solution[picked_free_place_donor].back().end + user_need,
{}});
}
free_spaces.erase(free_spaces.begin());
free_spaces.insert({sz - user_need, picked_free_place_donor});
ASSERT(sz - user_need >= 0, "kek");
solution[picked_free_place_donor].back().users.push_back(user_id);
for (int beam: uniq_beams) {// if L != 1
if (beam != userInfos[user_id].beam) {
possible_pre_free_spaces[beam].insert({picked_free_place_donor,
solution[picked_free_place_donor].size() -
1});
}
}
}
}
}
}
}
// mering two intervals;
//for (auto user_id: activeUsers[pick_i]){
//   solution[pick_i][curr].users.push_back(user_id);
// }
std::set<int> okay_users;
for (auto user_id: solution[pick_i][curr].users) {
if (need_to_find_new_set_bits.contains(user_id) == 0) {
activeUsers[pick_i].insert(user_id);
activeBeams[pick_i].insert(userInfos[user_id].beam);
okay_users.insert(user_id);
}
}
if (curr != 0) {
for (auto user_id: solution[pick_i][curr - 1].users) {
okay_users.insert(user_id);
}
solution[pick_i][curr].start = solution[pick_i][curr - 1].start;
keep_or_not[pick_i][curr - 1] = false;
}
solution[pick_i][curr].users.clear();
for (auto user_id: okay_users) {
solution[pick_i][curr].users.push_back(user_id);
if (solution[pick_i][curr].users.size() == L) {
for (auto beam: uniq_beams) {
possible_pre_free_spaces[beam].erase({pick_i, curr});
}
}
ASSERT(solution[pick_i][curr].users.size() <= L, "kek");
}
} else {
activeUsers[pick_i].clear();
activeBeams[pick_i].clear();
for (auto user_id: solution[pick_i][curr].users) {
activeUsers[pick_i].insert(user_id);
activeBeams[pick_i].insert(userInfos[user_id].beam);
}
}
current_sub_interval[pick_i]++;
space_left_q.insert({space_left - curr_len, pick_i});
}
std::set<pair<int, int>, greater<>> new_empty;
for (auto [rbNeed, user_id]: empty) {
if (!possible_pre_free_spaces[userInfos[user_id].beam].empty()) {
auto place_to_get = *possible_pre_free_spaces[userInfos[user_id].beam].begin();
for (auto [ii, gg]: possible_pre_free_spaces[userInfos[user_id].beam]) {
if (solution[ii][gg].end - solution[ii][gg].start > solution[place_to_get.first][place_to_get.second].end - solution[place_to_get.first][place_to_get.second].start) {
place_to_get = {ii, gg};
}
}
solution[place_to_get.first][place_to_get.second].users.push_back(user_id);
if (solution[place_to_get.first][place_to_get.second].users.size() > L){
}
ASSERT(solution[place_to_get.first][place_to_get.second].users.size() <= L, "kek");
possible_pre_free_spaces[userInfos[user_id].beam].erase(place_to_get);
if (solution[place_to_get.first][place_to_get.second].users.size() == L) {
for (auto beam: uniq_beams) {
possible_pre_free_spaces[beam].erase(place_to_get);
}
}
} else {
new_empty.insert({userInfos[user_id].rbNeed, user_id});
}
}
empty = new_empty;
std::vector<std::vector<Interval>> new_intervals(solution.size());
if (new_version) {
for (auto free_space: free_spaces) {
if (was_empty_space[free_space.second]) continue;
if (free_space.first > 0) {
int picked_free_place_donor = free_space.second;
int minus = 0;
for (int g = 0; g < start_sizes[picked_free_place_donor]; g++) {
if (!keep_or_not[picked_free_place_donor][g]) {
minus++;
}
}
if (!ever_picked[picked_free_place_donor] && was_full_interval[picked_free_place_donor]){
if (solution[picked_free_place_donor].empty()) {
solution[picked_free_place_donor].push_back({space_sizes[picked_free_place_donor].second, space_sizes[picked_free_place_donor].second + free_space.first, pre_placed[picked_free_place_donor]});
} else {
solution[picked_free_place_donor].push_back(
{solution[picked_free_place_donor].back().end,
solution[picked_free_place_donor].back().end + free_space.first,
pre_placed[picked_free_place_donor]});
}
} else {
solution[picked_free_place_donor].back().end+=free_space.first;
}
}
}
}
for (int i = 0; i < solution.size(); i++) {
for (int g = 0; g < start_sizes[i]; g++) {
if (keep_or_not[i][g]) {
new_intervals[i].push_back(solution[i][g]);
}
}
for (int g = start_sizes[i]; g < solution[i].size(); g++) {
new_intervals[i].push_back(solution[i][g]);
}
}
//        new_intervals[0].back().end = M;
solution = new_intervals;
}
//    std::vector<int>supplied(N,0);
//    int f_score = get_solution_score_light(N, solution, userInfos,supplied);
std::vector<int> suplied(N, 0);
get_solution_score_light(N, solution, userInfos, suplied);
std::set<int> new_empty;
for (auto [rbNeed, user_id]: empty) {
new_empty.insert(user_id);
}
for (int i = 0; i < solution.size(); i++) {
optimize_one_gap(N, M, K, J, L, reservedRBs, userInfos, solution[i], suplied, new_empty);
}
//    int s_score = get_solution_score_light(N, solution, userInfos,supplied);
//    if (f_score > s_score){
//        cout << f_score-s_score << " "  <<  f_score << " " << s_score << " " << N << endl;
//    }
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
optimize(N, M, K, J, L, reservedRBs, userInfos, anses[i], false);
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
//TEST CASE: K=0 | tests: 666 | score: 99.219% | 647247/652342 | time: 11025.8ms | max_time: 29.784ms | mean_time: 16.5553ms
//TEST CASE: K=1 | tests: 215 | score: 97.7355% | 211224/216118 | time: 3998.73ms | max_time: 23.965ms | mean_time: 18.5988ms
//TEST CASE: K=2 | tests: 80 | score: 97.5734% | 77365/79289 | time: 1437.42ms | max_time: 15.822ms | mean_time: 17.9678ms
//TEST CASE: K=3 | tests: 39 | score: 96.4027% | 43601/45228 | time: 672.092ms | max_time: 14.211ms | mean_time: 17.2331ms
//TEST CASE: K=4 | tests: 0 | score: -nan% | 0/0 | time: 0ms | max_time: 0ms | mean_time: 0ms
//TOTAL: tests: 1000 | score: 98.6364% | 979437/992977 | time: 17134.1ms | max_time: 29.784ms | mean_time: 17.1341ms
//TEST CASE: K=0 | tests: 666 | score: 99.3019% | 647788/652342 | time: 32750.2ms | max_time: 54.266ms | mean_time: 49.1745ms
//TEST CASE: K=1 | tests: 215 | score: 97.8757% | 211527/216118 | time: 9768.15ms | max_time: 41.15ms | mean_time: 45.4333ms
//TEST CASE: K=2 | tests: 80 | score: 97.8181% | 77559/79289 | time: 4195.06ms | max_time: 41.157ms | mean_time: 52.4383ms
//TEST CASE: K=3 | tests: 39 | score: 96.4226% | 43610/45228 | time: 2137.59ms | max_time: 40.539ms | mean_time: 54.81ms
//TEST CASE: K=4 | tests: 0 | score: -nan% | 0/0 | time: 0ms | max_time: 0ms | mean_time: 0ms
//TOTAL: tests: 1000 | score: 98.7419% | 980484/992977 | time: 48851ms | max_time: 54.266ms | mean_time: 48.851ms
//TEST CASE: K=0 | tests: 666 | score: 99.3477% | 648087/652342 | time: 38397.7ms | max_time: 81.211ms | mean_time: 57.6542ms
//TEST CASE: K=1 | tests: 215 | score: 98.0307% | 211862/216118 | time: 11075.9ms | max_time: 46.232ms | mean_time: 51.5159ms
//TEST CASE: K=2 | tests: 80 | score: 98.0426% | 77737/79289 | time: 4805.1ms | max_time: 93.13ms | mean_time: 60.0637ms
//TEST CASE: K=3 | tests: 39 | score: 96.6149% | 43697/45228 | time: 2171.13ms | max_time: 41.476ms | mean_time: 55.6699ms
//TEST CASE: K=4 | tests: 0 | score: -nan% | 0/0 | time: 0ms | max_time: 0ms | mean_time: 0ms
//TOTAL: tests: 1000 | score: 98.8324% | 981383/992977 | time: 56449.8ms | max_time: 93.13ms | mean_time: 56.4498ms
const int SELECTION_SIZE = 10;
const int METRIC_COEFFS_SIZE = 4;
//SelectionRandomizer SELECTION_ACTION(SELECTION_SIZE);
// = std::vector<int>{10, 1, 1, 1, 1, 16, 9, 6, 22, 12, 11, 30};
int STEPS = 400;
const int METRIC_CNT = 1;
int METRIC_TYPE = 0;
int CNT_ACCEPTED_EQ = 0;
static int frame_id = 0;
#define SNAP_ACTION(action_foo)                                                                              \
SNAP(snapshoter.write(get_total_answer(), "annealing:"                                                   \
"\\naccepted: " +                                              \
to_string(metric.accepted) +                           \
"\\nfree_space: " + to_string(metric.free_space) +     \
"\\noverflow: " + to_string(metric.overflow) +         \
"\\nunused_space: " + to_string(metric.unused_space) + \
"\\nframe_id: " + to_string(frame_id++) +              \
"\\n" #action_foo))
struct EgorTaskSolver {
///============================
/// task data
///============================
//                                                    0  1  2  3  4   5  6  7   8   9  10
SelectionRandomizer SELECTION_ACTION;// = vector<int>{0, 3, 1, 7, 0, 89, 0, 0, 90, 40, 23};
vector<int> METRICS_COEF;
long long TRAIN_SCORE = 0;
int N;
int M;
int K;
int J;
int L;
bool was_accepted = false;
int B;// колво блоков
vector<MyInterval> free_intervals;
///============================
/// internal data
///============================
randomizer rnd;
struct SetInterval {
int len = 0;
uint32_t beam_msk = 0;
MyBitSet users;
friend bool operator==(const SetInterval &lhs, const SetInterval &rhs) {
return lhs.len == rhs.len &&
lhs.users == rhs.users &&
lhs.beam_msk == rhs.beam_msk;
}
};
vector<vector<SetInterval>> intervals;
struct MyUserInfo {
// user info
int id = -1;
int rbNeed = -1;
int beam = -1;
int pos = -1;
// score
int sum_len = 0;
};
MyBitSet unused_users;
vector<MyUserInfo> users_info;
vector<int> users_beam[32];
vector<int> user_id_to_u;
struct Action {
enum Type {
ADD_USER_IN_INTERVAL,
REMOVE_USER_IN_INTERVAL,
CHANGE_INTERVAL_LEN,
REMOVE_INTERVAL,
ADD_INTERVAL,
} type;
int b = -1;
int i = -1;
int u = -1;
int c = 0;
};
vector<Action> actions;
struct Metric {
// сумма min(rbNeed, len) по всем пользователям
// то есть обычный score
long long accepted = 0;
// размер пустого пространства,
// которое не заполнено пользователем
// чем больше, тем лучше, так как можно поставить пользователей
// TODO: чем больше, тем не совсем лучше, надо смотреть
long long free_space = 0;
// сумма которую мы перевыполнили для пользователей
// чем она больше, тем хуже,
// так как мы могли бы отдать это место пользователем
long long overflow = 0;
// размер пространства, которое вообще не занято интервалами
// но не является зарезервированным
// изначально=M
// затем выкидываем длины зарезервированных
long long unused_space = 0;
friend bool operator==(Metric lhs, Metric rhs) {
return lhs.accepted == rhs.accepted &&
lhs.free_space == rhs.free_space &&
lhs.overflow == rhs.overflow &&
lhs.unused_space == rhs.unused_space;
}
} metric;
void rollback();
void rollback(int size);
EgorTaskSolver(int NN, int MM, int KK, int JJ, int LL,
const vector<Interval> &reservedRBs,
const vector<UserInfo> &userInfos,
vector<Interval> start_intervals, int random_seed, vector<int> powers, vector<int> metric_coefs);
///===========================
///===========ACTIONS=========
///===========================
void user_do_swap_eq_beam(int u, int u2);
void change_user_len(int u, int c);
void IMPL_change_interval_len(int b, int i, int c);
void change_interval_len(int b, int i, int c);
void IMPL_add_user_in_interval(int u, int b, int i);
void add_user_in_interval(int u, int b, int i);
void IMPL_remove_user_in_interval(int u, int b, int i);
void remove_user_in_interval(int u, int b, int i);
void remove_interval(int b, int i);
void insert_interval(int b, int i);
///==========================
///===========GETTERS========
///==========================
[[nodiscard]] vector<Interval> get_total_answer() const;
[[nodiscard]] tuple<int, int, int> get_user_position(int u) const;
[[nodiscard]] int get_user_score(int u) const;
[[nodiscard]] int get_block_len(int b) const;
[[nodiscard]] int get_intervals_size() const;
[[nodiscard]] Metric get_metric() const;
[[nodiscard]] long long get_vertical_free_space(int b, int i) const;
///==========================
///===========RANDOM=========
///==========================
double temperature = 1;
bool is_good(Metric old_metric) {
ASSERT(get_metric() == metric, "invalid metric");
return metric.accepted >= old_metric.accepted;
// 979437 -> 980155 -> 980332 -> 980459
/*auto calc_f = [&](Metric m) {
return m.accepted;
//return METRICS_COEF[0] * m.accepted +
//       METRICS_COEF[1] * m.free_space +
//       METRICS_COEF[2] * m.overflow +
//       METRICS_COEF[3] * m.unused_space;
//STEPS: 1000
//TEST CASE: K=0 | tests: 666 | score: 99.2499% | 647449/652342 | time: 2418.2ms | max_time: 15.081ms | mean_time: 3.63093ms
//TEST CASE: K=1 | tests: 215 | score: 98.0825% | 211974/216118 | time: 1502.41ms | max_time: 15.594ms | mean_time: 6.98797ms
//TEST CASE: K=2 | tests: 80 | score: 97.7954% | 77541/79289 | time: 803.128ms | max_time: 19.448ms | mean_time: 10.0391ms
//TEST CASE: K=3 | tests: 39 | score: 96.7343% | 43751/45228 | time: 375.988ms | max_time: 14.503ms | mean_time: 9.64072ms
//TEST CASE: K=4 | tests: 0 | score: -nan% | 0/0 | time: 0ms | max_time: 0ms | mean_time: 0ms
//TOTAL: tests: 1000 | score: 98.7651% | 980715/992977 | time: 5099.73ms | max_time: 19.448ms | mean_time: 5.09973ms
//return 100 * m.accepted;
//980680
//return 10'000 * m.accepted - m.unused_space;
//1682
//return 10'000 * m.accepted + m.vertical_free_space;
//return 100 * m.accepted - 10 * m.unused_space - m.overflow - m.free_space;
//add score: 0 897 417
if (METRIC_TYPE == 0) {
return m.accepted;
} else if (METRIC_TYPE == 1) {
// +712
//return 100 * m.accepted - m.unused_space - m.overflow - m.free_space;
// 720
//return 100 * m.accepted - 2 * m.unused_space - m.overflow - m.free_space;
// 769
//return 100 * m.accepted - 5 * m.unused_space - m.overflow - m.free_space;
//779
return 100 * m.accepted - 10 * m.unused_space - m.overflow - m.free_space;
//772
//return 1000 * m.accepted - 99 * m.unused_space - 10 * m.overflow - 10 * m.free_space;
} else if (METRIC_TYPE == 2) {
return m.accepted * 5 - m.unused_space;
}
else if(METRIC_TYPE == 3){
//980520
//return m.accepted * 100 + m.free_space;
//980526
//return m.accepted * 10 + m.free_space;
return m.accepted * 10 - m.overflow;
}
else if(METRIC_TYPE == 1){
// add score: 0 834 199 51
// 980488
return m.accepted * 5 - m.unused_space;
}
else if(METRIC_TYPE == 2){
return m.accepted + m.unused_space;
}
else if(METRIC_TYPE == 3){
return 100 * m.accepted - m.unused_space - m.overflow;
}
else if(METRIC_TYPE == 3){
return m.accepted * 10000 + m.overflow;
}
else if(METRIC_TYPE == 4){
return m.accepted * 10 + m.free_space;
}
ASSERT(false, "invalid METRIC_TYPE");
/// !!!
//return m.accepted;// 979437
///return m.accepted * m.accepted; // 979474
///return pow(m.accepted, 3); // 979474
//return sqrt(m.accepted); // 932116
//return -m.overflow; // 25356
//return m.accepted + m.free_space; // 50823
//return m.accepted * 10 + m.free_space; // 973878
///return m.accepted * 100 + m.free_space; // 977389
///return m.accepted * 1000 + m.free_space; // 977434
///return m.accepted * 10000 + m.free_space; // 977434
///return m.accepted * 100000 + m.free_space; // 977434
//return m.accepted * 100 - m.free_space; // 975114
//return m.accepted * 1000 - m.free_space; // 975116
//return m.accepted * 10 - m.overflow; // 975273
//return m.accepted * 100 - m.overflow; // 976539
//return m.accepted * 1000 - m.overflow; //976555
//return m.accepted * 10000 - m.overflow; //976555
//return m.accepted - m.unused_space; // 979322
//return m.accepted - m.unused_space * L; // 979341
//return m.accepted *2 - m.unused_space; // 979435
//return m.accepted *3 - m.unused_space; // 979452
//return m.accepted *4 - m.unused_space; // 979484
//return m.accepted * 5 - m.unused_space;// 979385
//return m.accepted * 10 - m.unused_space; // 979359
//return m.accepted * m.accepted - m.unused_space; // 979374
//return m.accepted + m.unused_space; // 970043
//return m.accepted * 10 + m.unused_space; // 978390
//return m.accepted * 100 + m.unused_space;// 978419
};
//double f = calc_f(metric);
//double old_f = calc_f(old_metric);
//return f >= old_f;// || rnd.get_d() < exp((f - old_f)  / temperature);
//ASSERT(f > 0, "invalid f");
//ASSERT(old_f > 0, "invalid old_f");
double rnd_ddd = rnd.get_d();
double up = (f - old_f) / (double) THEORY_MAX_SCORE;
double down = temperature;
//cout << rnd_ddd << " " << up << " " << down << " | " <<  up/down << " " << exp(up/down) << " | " << f << " " << old_f << " " << THEORY_MAX_SCORE << endl;
if (f > old_f) {
//            cout << old_metric.overflow << " " << old_metric.free_space << " " << old_metric.vertical_free_space << endl;
//            cout << "---------------" << endl;
//            cout << metric.overflow << " " << metric.free_space << " " << metric.vertical_free_space << endl;
//            return true;
} else if (f == old_f) {
CNT_ACCEPTED_EQ++;
if (metric.overflow <= old_metric.overflow) {
return true;
} else {
return true;
}
return true;
return true;
} else {
return false;
}
return f > old_f || (temp && (rnd.get_d() < exp((5 * (f - old_f) / (double) THEORY_MAX_SCORE) / temperature)));*/
}
///==========================
///===========INTERVAL=======
///==========================
void interval_flow_over();
bool merge_verify(int b, int i);
void interval_do_merge(int b, int i);
void interval_do_split(int b, int i);
void interval_merge();
bool interval_split_IMPL();
void interval_split();
void interval_do_free();
///======================
///===========USER=======
///======================
void user_do_new_interval(int u);
//void user_new_interval();
void user_remove_and_add();
void user_RobinHood();
///======================
///======ANNEALING=======
///======================
vector<Interval> annealing(vector<Interval> reservedRBs,
vector<UserInfo> userInfos);
};
///==========================
///===========GETTERS========
///==========================
vector<Interval> EgorTaskSolver::get_total_answer() const {
vector<Interval> answer;
for (int block = 0; block < B; block++) {
int start = free_intervals[block].start;
for (auto [len, beam_msk, users]: intervals[block]) {
if (len != 0 && !users.empty()) {
answer.push_back(Interval{start, start + len, {}});
start += len;
for (int u: users) {
answer.back().users.push_back(users_info[u].id);
}
}
}
ASSERT(start <= free_intervals[block].end, "invalid segment");
}
return answer;
}
tuple<int, int, int> EgorTaskSolver::get_user_position(int u) const {
ASSERT(0 <= u && u < N, "invalid u");
for (int block = 0; block < B; block++) {
int left = -1;
int right = -1;
for (int i = 0; i < intervals[block].size(); i++) {
if (intervals[block][i].users.contains(u)) {
right = i;
if (left == -1) {
left = i;
}
}
}
if (left != -1) {
return {block, left, right};
}
}
return {-1, -1, -1};
}
int EgorTaskSolver::get_user_score(int u) const {
ASSERT(0 <= u && u < N, "invalid u");
int len = users_info[u].sum_len;
int rbNeed = users_info[u].rbNeed;
return min(len, rbNeed);
}
int EgorTaskSolver::get_block_len(int b) const {
ASSERT(0 <= b && b < B, "invalid b");
int len = 0;
for (auto &interval: intervals[b]) {
len += interval.len;
}
return len;
}
int EgorTaskSolver::get_intervals_size() const {
int size = 0;
for (int b = 0; b < B; b++) {
size += intervals[b].size();
}
return size;
}
[[nodiscard]] EgorTaskSolver::Metric EgorTaskSolver::get_metric() const {
EgorTaskSolver::Metric m;
for (auto interval: free_intervals) {
m.unused_space += interval.len();
}
for (int u = 0; u < N; u++) {
m.accepted += min(users_info[u].rbNeed, users_info[u].sum_len);
m.overflow += max(0, users_info[u].sum_len - users_info[u].rbNeed);
}
for (int b = 0; b < B; b++) {
for (auto interval: intervals[b]) {
m.unused_space -= interval.len;
m.free_space += interval.len * (L - interval.users.size());
}
}
return m;
}
/*[[nodiscard]] long long EgorTaskSolver::get_vertical_free_space(int b, int i) const {
auto end_users = intervals[b][i].users;
if (i + 1 < intervals[b].size()) {
end_users = end_users ^ (end_users & intervals[b][i + 1].users);
}
int min_overflow = 1e9;
int cnt_overflow = 0;
for (int u: end_users) {
if (users_info[u].rbNeed < users_info[u].sum_len) {
// overflow
cnt_overflow++;
min_overflow = min(min_overflow, min(intervals[b][i].len, users_info[u].sum_len - users_info[u].rbNeed));
}
}
ASSERT(min_overflow >= 0, "failed");
return static_cast<long long>(min_overflow) * cnt_overflow;
}*/
///===========================
///===========ROLLBACK========
///===========================
void EgorTaskSolver::rollback() {
ASSERT(!actions.empty(), "actions is empty");
auto [type, b, i, u, c] = actions.back();
actions.pop_back();
if (type == Action::Type::ADD_USER_IN_INTERVAL) {
IMPL_remove_user_in_interval(u, b, i);// O(1)
} else if (type == Action::Type::REMOVE_USER_IN_INTERVAL) {
IMPL_add_user_in_interval(u, b, i);// O(1)
} else if (type == Action::Type::CHANGE_INTERVAL_LEN) {
IMPL_change_interval_len(b, i, -c);// O(L)
} else if (type == Action::Type::REMOVE_INTERVAL) {
intervals[b].insert(intervals[b].begin() + i, SetInterval());// O(J)
} else if (type == Action::Type::ADD_INTERVAL) {
intervals[b].erase(intervals[b].begin() + i);// O(J)
} else {
ASSERT(false, "invalid type");
}
}
void EgorTaskSolver::rollback(int size) {
while (actions.size() > size) {
rollback();
}
}
///===========================
///===========ACTIONS=========
///===========================
void EgorTaskSolver::user_do_swap_eq_beam(int u, int u2) {
ASSERT(users_info[u].beam == users_info[u].beam, "no equals beams");
int urbNeed = users_info[u].rbNeed;
int u2rbNeed = users_info[u2].rbNeed;
int usum_len = users_info[u].sum_len;
int u2sum_len = users_info[u2].sum_len;
metric.accepted += min(urbNeed, u2sum_len) - min(urbNeed, usum_len) +
min(u2rbNeed, usum_len) - min(u2rbNeed, u2sum_len);
metric.overflow += max(0, usum_len - u2rbNeed) - max(0, usum_len - urbNeed) +
max(0, u2sum_len - urbNeed) - max(0, u2sum_len - u2rbNeed);
swap(user_id_to_u[users_info[u].id], user_id_to_u[users_info[u2].id]);
swap(users_info[u].rbNeed, users_info[u2].rbNeed);
swap(users_info[u].id, users_info[u2].id);
swap(users_info[u].pos, users_info[u2].pos);
}
void EgorTaskSolver::change_user_len(int u, int c) {
auto &users = users_beam[users_info[u].beam];
#ifdef MY_DEBUG_MODE
for (int i = 0; i + 1 < users.size(); i++) {
ASSERT(users_info[users[i]].sum_len >= users_info[users[i + 1]].sum_len, "WTF");
}
#endif
{
metric.accepted -= min(users_info[u].rbNeed, users_info[u].sum_len);
metric.overflow -= max(0, users_info[u].sum_len - users_info[u].rbNeed);
users_info[u].sum_len += c;
metric.accepted += min(users_info[u].rbNeed, users_info[u].sum_len);
metric.overflow += max(0, users_info[u].sum_len - users_info[u].rbNeed);
}
//for (int i = 0; i < users.size(); i++)
{
// find
//if (users[i] == u)
{
int i = users_info[u].pos;
ASSERT(users[i] == u, "failed");
ASSERT(i == users_info[u].pos, "failed");
// move left
while (i > 0 && users_info[users[i - 1]].sum_len < users_info[u].sum_len) {
user_do_swap_eq_beam(users[i - 1], u);
swap(users[i - 1], users[i]);
i--;
}
// move right
while (i + 1 < users.size() && users_info[u].sum_len < users_info[users[i + 1]].sum_len) {
user_do_swap_eq_beam(users[i + 1], u);
swap(users[i + 1], users[i]);
i++;
}
#ifdef MY_DEBUG_MODE
for (int i = 0; i + 1 < users.size(); i++) {
ASSERT(users_info[users[i]].sum_len >= users_info[users[i + 1]].sum_len, "WTF");
}
#endif
return;
}
}
ASSERT(false, "why we here?");
}
void EgorTaskSolver::IMPL_change_interval_len(int b, int i, int c) {
auto &interval = intervals[b][i];
for (int u: interval.users) {
change_user_len(u, c);
}
interval.len += c;
metric.free_space += c * (L - interval.users.size());
metric.unused_space -= c;
ASSERT(0 <= interval.len, "invalid interval");
}
void EgorTaskSolver::change_interval_len(int b, int i, int c) {
//CNT_CALL_CHANGE_INTERVAL_LEN++;
actions.push_back({Action::Type::CHANGE_INTERVAL_LEN, b, i, -1, c});
IMPL_change_interval_len(b, i, c);
}
void EgorTaskSolver::IMPL_add_user_in_interval(int u, int b, int i) {
auto &interval = intervals[b][i];
auto &user = users_info[u];
ASSERT(interval.users.size() + 1 <= L, "failed add");
ASSERT(!interval.users.contains(u), "user already contains");
ASSERT(((interval.beam_msk >> user.beam) & 1) == 0, "equal beams");
if(unused_users.contains(u)){
unused_users.erase(u);
}
interval.users.insert(u);
interval.beam_msk ^= (uint32_t(1) << user.beam);
metric.free_space -= interval.len;
change_user_len(u, interval.len);
}
void EgorTaskSolver::add_user_in_interval(int u, int b, int i) {
//CNT_CALL_ADD_USER_IN_INTERVAL++;
actions.push_back({Action::Type::ADD_USER_IN_INTERVAL, b, i, u, 0});
IMPL_add_user_in_interval(u, b, i);
}
void EgorTaskSolver::IMPL_remove_user_in_interval(int u, int b, int i) {
auto &interval = intervals[b][i];
auto &user = users_info[u];
ASSERT(interval.users.contains(u), "user no contains");
ASSERT(((interval.beam_msk >> users_info[u].beam) & 1) == 1, "user no contains");
if((i == 0 || !intervals[b][i-1].users.contains(u)) && (i + 1 == intervals[b].size() || !intervals[b][i+1].users.contains(u))){
unused_users.insert(u);
}
interval.users.erase(u);
interval.beam_msk ^= (uint32_t(1) << user.beam);
metric.free_space += interval.len;
change_user_len(u, -interval.len);
}
void EgorTaskSolver::remove_user_in_interval(int u, int b, int i) {
//CNT_CALL_REMOVE_USER_IN_INTERVAL++;
actions.push_back({Action::Type::REMOVE_USER_IN_INTERVAL, b, i, u, 0});
IMPL_remove_user_in_interval(u, b, i);
}
void EgorTaskSolver::remove_interval(int b, int i) {
if (intervals[b][i].len != 0) {
change_interval_len(b, i, -intervals[b][i].len);
}
for (int u: intervals[b][i].users) {
remove_user_in_interval(u, b, i);
}
actions.push_back({Action::Type::REMOVE_INTERVAL, b, i, -1, 0});
intervals[b].erase(intervals[b].begin() + i);
}
void EgorTaskSolver::insert_interval(int b, int i) {
actions.push_back({Action::Type::ADD_INTERVAL, b, i, -1, 0});
intervals[b].insert(intervals[b].begin() + i, SetInterval());
}
///==========================
///===========INTERVAL=======
///==========================
#include <bitset>
#define CHOOSE_INTERVAL(condition, return_value)            \
int b, i;                                               \
{                                                       \
vector<pair<int, int>> ips;                         \
for (int b = 0; b < B; b++) {                       \
for (int i = 0; i < intervals[b].size(); i++) { \
if (condition) {                            \
ips.emplace_back(b, i);                 \
}                                           \
}                                               \
}                                                   \
if (ips.empty()) {                                  \
return return_value;                            \
}                                                   \
int k = rnd.get(0, ips.size() - 1);                 \
b = ips[k].first;                                   \
i = ips[k].second;                                  \
}
void EgorTaskSolver::interval_flow_over() {
//981995
/*int b = rnd.get(0, B - 1);
for (int step = 0; step < 100; step++) {
int best_from = -1, best_to = -1, best_f = -1e9;
for (int from = 0; from < intervals[b].size(); from++) {
if (intervals[b][from].len == 0) {
continue;
}
for (int to = 0; to < intervals[b].size(); to++) {
if (from != to) {
auto and_users = intervals[b][from].users & intervals[b][to].users;
auto unique_i_users = intervals[b][from].users ^ and_users;
auto unique_j_users = intervals[b][to].users ^ and_users;
int accepted = 0;
int overflow = 0;
for (int u: unique_i_users) {
if (users_info[u].sum_len <= users_info[u].rbNeed) {
accepted--;
}
else{
overflow--;
}
}
for (int u: unique_j_users) {
if (users_info[u].sum_len < users_info[u].rbNeed) {
accepted++;
}
else{
overflow++;
}
}
int cur_f = accepted * 100 + overflow;
if (best_f <= cur_f) {
best_f = cur_f;
best_from = from;
best_to = to;
}
}
}
}
if (best_from == -1) {
break;
}
change_interval_len(b, best_from, -1);
change_interval_len(b, best_to, +1);
}
for (int i = (int) intervals[b].size() - 1; i >= 0; i--) {
if (intervals[b][i].len == 0) {
remove_interval(b, i);
}
}*/
/*int b = rnd.get(0, B - 1);
for (int step = 0; step < 100; step++) {
// найти интервал, у которого можно хорошо спиздить длину 1
// отдать ее другому интервалу
int from = -1;
{
int best_f = -1e9;
for (int i = 0; i < intervals[b].size(); i++) {
if(intervals[b][i].len == 0){
continue;
}
int accepted = 0;
for (int u: intervals[b][i].users) {
accepted -= users_info[u].sum_len <= users_info[u].rbNeed;
}
int cur_f = accepted;
if (best_f <= cur_f) {
best_f = cur_f;
from = i;
}
}
if(from == -1){
break;
}
}
int to = -1;
{
int best_f = -1e9;
for (int i = 0; i < intervals[b].size(); i++) {
int accepted = 0;
for (int u: intervals[b][i].users) {
accepted += users_info[u].sum_len < users_info[u].rbNeed;
}
int cur_f = accepted;
if (best_f < cur_f) {
best_f = cur_f;
to = i;
}
}
if(to == -1){
break;
}
}
if (from == to) {
break;
}
change_interval_len(b, from, -1);
change_interval_len(b, to, +1);
}
for (int i = (int) intervals[b].size() - 1; i >= 0; i--) {
if (intervals[b][i].len == 0) {
remove_interval(b, i);
}
}*/
/*sort(ips.begin(), ips.end());
int sum = 0;
for(auto [metric, block, i] : ips){
sum += metric;
}
int x = rnd.get(0, sum);
bool choose = false;
for(auto [metric, bbbblock, iiii] : ips){
x -= metric;
if(x <= 0){
choose = true;
block = bbbblock;
i = iiii;
}
}
ASSERT(choose, "failed choose");*/
// 982347
int block, i;
{
// (metric, block, i)
vector<tuple<int, int, int>> ips;
for (int block = 0; block < B; block++) {
if (intervals[block].size() > 1) {
for (int i = 0; i < intervals[block].size(); i++) {
int overflow = 0;
for (int u: intervals[block][i].users) {
overflow += max(0, users_info[u].sum_len - users_info[u].rbNeed);
}
ips.emplace_back(overflow, block, i);
}
}
}
if (ips.empty()) {
return;
}
int p = rnd.get(0, ips.size() - 1);
block = get<1>(ips[p]);
i = get<2>(ips[p]);
//int p = rnd.get(0, ips.size() - 1);
}
/*MyBitSet dp;
for (int i = 0; i < intervals[block].size(); i++) {
for (int u: intervals[block][i].users) {
if (users_info[u].sum_len < users_info[u].rbNeed && !dp.contains(u)) {
dp.insert(u);
}
}
}*/
while (intervals[block][i].len > 0) {
// выбрать кому лучше всего отдать длину +1
int best_j = -1, best_f = 0;
for (int j = 0; j < intervals[block].size(); j++) {
if (j != i) {
int accepted = 0;
auto and_users = intervals[block][i].users & intervals[block][j].users;
auto unique_i_users = intervals[block][i].users ^ and_users;
auto unique_j_users = intervals[block][j].users ^ and_users;
for (int u: unique_i_users) {
if (users_info[u].sum_len <= users_info[u].rbNeed) {
accepted--;
}
}
for (int u: unique_j_users) {
if (users_info[u].sum_len < users_info[u].rbNeed) {
accepted++;
}
}
int cur_f = accepted;
if (best_f <= cur_f) {
best_f = cur_f;
best_j = j;
}
}
}
if (best_j == -1) {
break;
}
change_interval_len(block, i, -1);
change_interval_len(block, best_j, +1);
}
if (intervals[block][i].len == 0) {
remove_interval(block, i);
}
/*int block, i, j, change;
{
// (f, block, i, j, change)
vector<tuple<int, int, int, int,int>> ips;
for (int block = 0; block < B; block++) {
for (int i = 0; i < intervals[block].size(); i++) {
for (int j = 0; j < intervals[block].size(); j++) {
if (i != j) {
auto and_users = intervals[block][i].users & intervals[block][j].users;
auto unique_i = intervals[block][i].users ^ and_users;
auto unique_j = intervals[block][j].users ^ and_users;
for (int change = 1; change <= intervals[block][i].len; change++) {
int accepted = metric.accepted;
int overflow = metric.overflow;
for (int u: unique_i) {
accepted -= min(users_info[u].rbNeed, users_info[u].sum_len);
accepted += min(users_info[u].rbNeed, users_info[u].sum_len - change);
}
for (int u: unique_j) {
accepted -= min(users_info[u].rbNeed, users_info[u].sum_len);
accepted += min(users_info[u].rbNeed, users_info[u].sum_len + change);
}
int cur_f = accepted;
ips.emplace_back(cur_f, block, i, j, change);
}
}
}
}
}
if (ips.empty()) {
return;
}
sort(ips.begin(), ips.end(), greater<>());
int p = rnd.get(0, min((int)ips.size() - 1, 3));
block = get<1>(ips[p]);
i = get<2>(ips[p]);
j = get<3>(ips[p]);
change = get<4>(ips[p]);
}*/
// заберу у интервала i длину change, отдам ее интервалу j
/*int block, i, j;
{
vector<tuple<int, int, int>> ips;
for (int block = 0; block < B; block++) {
for (int i = 0; i < intervals[block].size(); i++) {
for (int j = 0; j < intervals[block].size(); j++) {
if (i != j) {
ips.emplace_back(block, i, j);
}
}
}
}
if (ips.empty()) {
return;
}
int p = rnd.get(0, ips.size() - 1);
block = get<0>(ips[p]);
i = get<1>(ips[p]);
j = get<2>(ips[p]);
}
int best_change = rnd.get(-intervals[block][i].len, intervals[block][j].len);
auto old_metric = metric;
int old_actions_size = actions.size();
change_interval_len(block, i, best_change);
change_interval_len(block, j, -best_change);
for(int unused_u : unused_users){
for(int u : intervals[block][i].users){
if(users_info[u].beam != users_info[unused_u].beam && ((intervals[block][i].beam_msk >> users_info[unused_u].beam) & 1) == 1){
continue;
}
bool have_left = i > 0 && intervals[block][i-1].users.contains(u);
bool have_right = i + 1 < intervals[block].size() && intervals[block][i+1].users.contains(u);
if(have_left && have_right){
continue;
}
if(min(users_info[u].rbNeed, users_info[u].sum_len) < min(users_info[unused_u].rbNeed, intervals[block][i].len)){
remove_user_in_interval(u, block, i);
add_user_in_interval(unused_u, block, i);
break;
}
}
if(!unused_users.contains(unused_u)){
break;
}
for(int u : intervals[block][j].users){
if(users_info[u].beam != users_info[unused_u].beam && ((intervals[block][j].beam_msk >> users_info[unused_u].beam) & 1) == 1){
continue;
}
bool have_left = j > 0 && intervals[block][j-1].users.contains(u);
bool have_right = j + 1 < intervals[block].size() && intervals[block][j+1].users.contains(u);
if(have_left && have_right){
continue;
}
if(min(users_info[u].rbNeed, users_info[u].sum_len) < min(users_info[unused_u].rbNeed, intervals[block][j].len)){
remove_user_in_interval(u, block, j);
add_user_in_interval(unused_u, block, j);
break;
}
}
}
if (is_good(old_metric)) {
SNAP_ACTION("interval_flow_over " + to_string(block) + " " + to_string(i) + " " + to_string(j) + " " +
to_string(change) + " accepted");
if (i > j) {
swap(i, j);
}
// i < j
if (intervals[block][j].len == 0) {
remove_interval(block, j);
}
if (intervals[block][i].len == 0) {
remove_interval(block, i);
}
#ifdef MY_DEBUG_MODE
for (int block = 0; block < B; block++) {
for (int index = 0; index < intervals[block].size(); index++) {
if (intervals[block][index].len == 0) {
ASSERT(false, "zero interval");
}
}
}
#endif
} else {
rollback(old_actions_size);
ASSERT(old_metric == metric, "failed back score");
}*/
}
void EgorTaskSolver::interval_do_merge(int b, int i) {
ASSERT(false, "not used");
ASSERT(i + 1 < intervals[b].size(), "invalid merge");
for (int u: intervals[b][i + 1].users) {
if (intervals[b][i].users.size() + 1 <= L &&
!intervals[b][i].users.contains(u) &&
((intervals[b][i].beam_msk >> users_info[u].beam) & 1) == 0) {
add_user_in_interval(u, b, i);
}
}
int right_len = intervals[b][i + 1].len;
remove_interval(b, i + 1);
change_interval_len(b, i, right_len);
}
void EgorTaskSolver::interval_do_split(int b, int i) {
auto right_end_users = intervals[b][i].users;
auto left_end_users = intervals[b][i].users;
if (i + 1 < intervals[b].size()) {
right_end_users = (right_end_users & intervals[b][i + 1].users) ^ right_end_users;
}
if (i > 0) {
left_end_users = (left_end_users & intervals[b][i - 1].users) ^ left_end_users;
}
auto end_users = left_end_users & right_end_users;
left_end_users = left_end_users ^ end_users;
right_end_users = right_end_users ^ end_users;
int best_left_len = -1;
int best_right_len = -1;
int best_f = -1;
std::vector<int> need_to_check;
std::vector<int> plus_this(intervals[b][i].len, 0);
std::vector<int> minus_this_after(intervals[b][i].len, 0);
//    std::vector<int>need_to_check;
bitset<512> have;
int accumulated_lefts = 0;
int accumulated_rights = 0;
for (int u: right_end_users) {
int right = users_info[u].sum_len - users_info[u].rbNeed;
int left = intervals[b][i].len - right;
if (left <= 0) {
accumulated_rights++;
}
if (left >= 1 and left < intervals[b][i].len) {
plus_this[left]++;
if (!have[left]) {
need_to_check.push_back(left);
have[left] = true;
}
}
}
for (int u: left_end_users) {
int left = users_info[u].sum_len - users_info[u].rbNeed;
if (users_info[u].sum_len > users_info[u].rbNeed) {
accumulated_lefts++;
}
if (left >= 1 and left < intervals[b][i].len) {
minus_this_after[left]++;
if (!have[left]) {
need_to_check.push_back(left);
have[left] = true;
}
}
}
if (!have[1]) {
need_to_check.push_back(1);
have[1] = true;
}
if (!have[intervals[b][i].len - 1]) {
need_to_check.push_back(intervals[b][i].len - 1);
have[intervals[b][i].len - 1] = true;
}
for (int u: end_users) {
{
int right = users_info[u].sum_len - users_info[u].rbNeed;
int left = intervals[b][i].len - right;
if (left >= 1 and left < intervals[b][i].len && !have[left]) {
need_to_check.push_back(left);
have[left] = true;
}
}
{
int left = users_info[u].sum_len - users_info[u].rbNeed;
if (left >= 1 and left < intervals[b][i].len && !have[left]) {
need_to_check.push_back(left);
have[left] = true;
}
}
}
sort(need_to_check.begin(), need_to_check.end());
if (false) {
for (int left_len = 1; left_len < intervals[b][i].len; left_len++) {
int right_len = intervals[b][i].len - left_len;
int free_space = 0;
for (int u: end_users) {
int x = 0;
if (users_info[u].sum_len - right_len >= users_info[u].rbNeed) {
x = max(x, right_len);
}
if (users_info[u].sum_len - left_len >= users_info[u].rbNeed) {
x = max(x, left_len);
}
free_space += x;
}
for (int u: right_end_users) {
if (users_info[u].sum_len - right_len >= users_info[u].rbNeed) {
free_space += right_len;
}
}
for (int u: left_end_users) {
if (users_info[u].sum_len - left_len >= users_info[u].rbNeed) {
free_space += left_len;
}
}
int cur_f = free_space;
if (cur_f > best_f) {
best_f = cur_f;
best_right_len = right_len;
best_left_len = left_len;
}
}
} else {
for (int left_len: need_to_check) {
int right_len = intervals[b][i].len - left_len;
int free_space = 0;
accumulated_rights += plus_this[left_len];
for (int u: end_users) {
int x = 0;
if (users_info[u].sum_len - right_len >= users_info[u].rbNeed) {
x = max(x, right_len);
}
if (users_info[u].sum_len - left_len >= users_info[u].rbNeed) {
x = max(x, left_len);
}
free_space += x;
}
int cur_f = free_space + accumulated_lefts * left_len + accumulated_rights * right_len;
if (cur_f > best_f) {
best_f = cur_f;
best_right_len = right_len;
best_left_len = left_len;
}
accumulated_lefts -= minus_this_after[left_len];
}
}
ASSERT(best_right_len != -1, "failed");
insert_interval(b, i + 1);
change_interval_len(b, i, -best_right_len);
change_interval_len(b, i + 1, best_right_len);
for (int u: intervals[b][i].users) {
add_user_in_interval(u, b, i + 1);
}
for (int u: right_end_users) {
if (users_info[u].sum_len - best_right_len >= users_info[u].rbNeed) {
remove_user_in_interval(u, b, i + 1);
}
}
for (int u: left_end_users) {
if (users_info[u].sum_len - best_left_len >= users_info[u].rbNeed) {
remove_user_in_interval(u, b, i);
}
}
for (int u: end_users) {
int x = 0;
if (users_info[u].sum_len - best_left_len >= users_info[u].rbNeed) {
x = max(x, best_left_len);
}
if (users_info[u].sum_len - best_right_len >= users_info[u].rbNeed) {
x = max(x, best_right_len);
}
if (x == best_left_len) {
remove_user_in_interval(u, b, i);
} else if (x == best_right_len) {
remove_user_in_interval(u, b, i + 1);
} else {
//ASSERT(false, "kek");
}
}
// попытаемся добавить юзеров, которых нет в освободившиеся места
#ifdef MY_DEBUG_MODE
MyBitSet set;
for (int user = 0; user < N; user++) {
if (users_info[user].sum_len == 0) {
set.insert(user);
}
}
ASSERT(unused_users == set, "invalid unused users");
#endif
for (int u: unused_users) {
bool may_left = intervals[b][i].users.size() < L && ((intervals[b][i].beam_msk >> users_info[u].beam) & 1) == 0;
bool may_right = intervals[b][i + 1].users.size() < L && ((intervals[b][i + 1].beam_msk >> users_info[u].beam) & 1) == 0;
if (may_left) {
add_user_in_interval(u, b, i);
}
if (may_right) {
add_user_in_interval(u, b, i + 1);
}
//user_do_new_interval(u);
}
}
/*void EgorTaskSolver::interval_do_split(int b, int i) {
auto right_end_users = intervals[b][i].users;
auto left_end_users = intervals[b][i].users;
if (i + 1 < intervals[b].size()) {
right_end_users = (right_end_users & intervals[b][i + 1].users) ^ right_end_users;
}
if (i > 0) {
left_end_users = (left_end_users & intervals[b][i - 1].users) ^ left_end_users;
}
auto end_users = left_end_users & right_end_users;
left_end_users = left_end_users ^ end_users;
right_end_users = right_end_users ^ end_users;
int best_left_len = -1;
int best_right_len = -1;
int best_f = -1;
for (int left_len = 1; left_len < intervals[b][i].len; left_len++) {
int right_len = intervals[b][i].len - left_len;
int free_space = 0;
for (int u: end_users) {
int x = 0;
if (users_info[u].sum_len - right_len >= users_info[u].rbNeed) {
x = max(x, right_len);
}
if (users_info[u].sum_len - left_len >= users_info[u].rbNeed) {
x = max(x, left_len);
}
free_space += x;
}
for (int u: right_end_users) {
if (users_info[u].sum_len - right_len >= users_info[u].rbNeed) {
free_space += right_len;
}
}
for (int u: left_end_users) {
if (users_info[u].sum_len - left_len >= users_info[u].rbNeed) {
free_space += left_len;
}
}
int cur_f = free_space;
if (cur_f > best_f) {
best_f = cur_f;
best_right_len = right_len;
best_left_len = left_len;
}
}
ASSERT(best_right_len != -1, "failed");
insert_interval(b, i + 1);
change_interval_len(b, i, -best_right_len);
change_interval_len(b, i + 1, best_right_len);
for (int u: intervals[b][i].users) {
add_user_in_interval(u, b, i + 1);
}
for (int u: right_end_users) {
if (users_info[u].sum_len - best_right_len >= users_info[u].rbNeed) {
remove_user_in_interval(u, b, i + 1);
}
}
for (int u: left_end_users) {
if (users_info[u].sum_len - best_left_len >= users_info[u].rbNeed) {
remove_user_in_interval(u, b, i);
}
}
for (int u: end_users) {
int x = 0;
if (users_info[u].sum_len - best_left_len >= users_info[u].rbNeed) {
x = max(x, best_left_len);
}
if (users_info[u].sum_len - best_right_len >= users_info[u].rbNeed) {
x = max(x, best_right_len);
}
if (x == best_left_len) {
remove_user_in_interval(u, b, i);
} else if (x == best_right_len) {
remove_user_in_interval(u, b, i + 1);
} else {
//ASSERT(false, "kek");
}
}
// попытаемся добавить юзеров, которых нет в освободившиеся места
#ifdef MY_DEBUG_MODE
MyBitSet set;
for (int user = 0; user < N; user++) {
if (users_info[user].sum_len == 0) {
set.insert(user);
}
}
ASSERT(unused_users == set, "invalid unused users");
#endif
// add unused users
for (int u: unused_users) {
bool may_left = intervals[b][i].users.size() < L && ((intervals[b][i].beam_msk >> users_info[u].beam) & 1) == 0;
bool may_right = intervals[b][i + 1].users.size() < L && ((intervals[b][i + 1].beam_msk >> users_info[u].beam) & 1) == 0;
if (may_left) {
add_user_in_interval(u, b, i);
}
if (may_right) {
add_user_in_interval(u, b, i + 1);
}
}
}*/
// replace
/*for (int unused_u: unused_users) {
for (int u: intervals[b][i].users) {
bool have_left = (i == 0 || intervals[b][i - 1].users.contains(u));
bool have_right = (i + 1 == intervals[b].size() || intervals[b][i + 1].users.contains(u));
if (have_left && have_right) {
continue;
}
int old_accepted = min(users_info[u].sum_len, users_info[u].rbNeed);
int new_accepted = min(users_info[u].sum_len - intervals[b][i].len, users_info[u].rbNeed) +
min(users_info[unused_u].rbNeed, intervals[b][i].len);
if (old_accepted < new_accepted && ((intervals[b][i].beam_msk >> users_info[unused_u].beam) & 1) == 0) {
remove_user_in_interval(u, b, i);
add_user_in_interval(unused_u, b, i);
break;
}
}
if (!unused_users.contains(unused_u)) {
continue;
}
for (int u: intervals[b][i + 1].users) {
bool have_left = intervals[b][i].users.contains(u);
bool have_right = (i + 2 == intervals[b].size() || intervals[b][i + 2].users.contains(u));
if (have_left && have_right) {
continue;
}
int old_accepted = min(users_info[u].sum_len, users_info[u].rbNeed);
int new_accepted = min(users_info[u].sum_len - intervals[b][i + 1].len, users_info[u].rbNeed) +
min(users_info[unused_u].rbNeed, intervals[b][i + 1].len);
if (old_accepted < new_accepted && ((intervals[b][i + 1].beam_msk >> users_info[unused_u].beam) & 1) == 0) {
remove_user_in_interval(u, b, i + 1);
add_user_in_interval(unused_u, b, i + 1);
break;
}
}
}*/
/*right_end_users = intervals[b][i].users;
left_end_users = intervals[b][i].users;
if (i + 1 < intervals[b].size()) {
right_end_users = (right_end_users & intervals[b][i + 1].users) ^ right_end_users;
}
if (i > 0) {
left_end_users = (left_end_users & intervals[b][i - 1].users) ^ left_end_users;
}
end_users = left_end_users & right_end_users;
left_end_users = left_end_users ^ end_users;
right_end_users = right_end_users ^ end_users;
for (int u: right_end_users) {
for (int unused_u: unused_users) {
int old_accepted = min(users_info[u].sum_len, users_info[u].rbNeed);
int new_accepted = min(users_info[u].sum_len - intervals[b][i + 1].len, users_info[u].rbNeed) +
min(users_info[unused_u].rbNeed, intervals[b][i + 1].len);
if (old_accepted < new_accepted) {
remove_user_in_interval(u, b, i + 1);
add_user_in_interval(unused_u, b, i + 1);
break;
}
}
}*/
/*for (int u = 0; u < N; u++) {
bool may_left = intervals[b][i].users.size() < L && ((intervals[b][i].beam_msk >> users_info[u].beam) & 1) == 0;
bool may_right = intervals[b][i + 1].users.size() < L && ((intervals[b][i + 1].beam_msk >> users_info[u].beam) & 1) == 0;
int len = (may_left ? intervals[b][i].len : 0) +
(may_right ? intervals[b][i + 1].len : 0);
if(min(users_info[u].rbNeed, users_info[u].sum_len) <
min(users_info[u].rbNeed, len)){
for(int b = 0; b < B; b++){
for(int i = 0; i < intervals[b].size(); i++){
if(intervals[b][i].users.contains(u)){
remove_user_in_interval(u, b, i);
}
}
}
if(may_left){
add_user_in_interval(u, b, i);
}
if(may_right){
add_user_in_interval(u, b, i + 1);
}
}
}*/
void EgorTaskSolver::interval_merge() {
ASSERT(false, "not used");
CHOOSE_INTERVAL(i + 1 < intervals[b].size(), );
auto old_metric = metric;
int old_actions_size = actions.size();
interval_do_merge(b, i);
if (is_good(old_metric)) {
SNAP_ACTION("interval_merge " + to_string(b) + " " + to_string(i) + " accepted");
} else {
rollback(old_actions_size);
ASSERT(old_metric == metric, "failed back score");
}
}
bool EgorTaskSolver::interval_split_IMPL() {
if (get_intervals_size() == J) {
// нужно добыть интервал для split
interval_do_free();
if (get_intervals_size() == J) {
return false;
}
}
CHOOSE_INTERVAL(intervals[b][i].len > 1, false);
interval_do_split(b, i);
return true;
}
void EgorTaskSolver::interval_split() {
ASSERT(get_intervals_size() <= J, "failed intervals size");
int old_actions_size = actions.size();
auto old_metric = metric;
if (interval_split_IMPL() && is_good(old_metric)) {
SNAP_ACTION("interval_split accepted");
#ifdef MY_DEBUG_MODE
for (int block = 0; block < B; block++) {
for (int index = 0; index < intervals[block].size(); index++) {
if (intervals[block][index].len == 0) {
ASSERT(false, "zero interval");
}
}
}
#endif
} else {
rollback(old_actions_size);
ASSERT(old_metric == metric, "failed back score");
}
}
void EgorTaskSolver::interval_do_free() {
// попытаемся смержить два одинаковых интервала
for (int block = 0; block < B; block++) {
for (int index = 0; index + 1 < intervals[block].size(); index++) {
if (intervals[block][index].users == intervals[block][index + 1].users) {
change_interval_len(block, index, intervals[block][index + 1].len);
remove_interval(block, index + 1);
return;
}
}
}
// TODO: рандомно выбирать интервал почему-то дает больше баллов
// (metric, block, index)
vector<tuple<int, int, int>> ips;
for (int block = 0; block < B; block++) {
for (int index = 0; index < intervals[block].size(); index++) {
// если мы удалим этот интервал, то сколько accepted будет?
int accepted = 0;//metric.accepted;
int overflow = 0;//metric.overflow;
for (int user: intervals[block][index].users) {
//accepted -= min(users_info[user].sum_len, users_info[user].rbNeed);
accepted += min(users_info[user].sum_len - intervals[block][index].len, users_info[user].rbNeed);
//overflow -= max(0, users_info[user].sum_len - users_info[user].rbNeed);
overflow += max(0, users_info[user].sum_len - intervals[block][index].len - users_info[user].rbNeed);
}
ips.emplace_back(accepted, block, index);
}
}
//sort(ips.begin(), ips.end(), greater<>());
if (ips.empty()) {
return;
}
auto [_, block, index] = ips[rnd.get(0, ips.size() - 1)];
int len = intervals[block][index].len;
ASSERT(len <= free_intervals[block].len(), "kek");
//ASSERT(get_block_len(block) == free_intervals[block].len(), "kek");
if (intervals[block].size() == 1) {
//ASSERT(false, "don't touch him");
return;
}
remove_interval(block, index);
ASSERT(!intervals[block].empty(), "kek");
// распределить наиболее оптимально len
change_interval_len(block, rnd.get(0, (int) intervals[block].size() - 1), len);
/*int best_index = -1, best_f = -1e9;
for (int index = 0; index < intervals[block].size(); index++) {
int accepted = 0;
for (int u: intervals[block][index].users) {
accepted += min(len, users_info[u].rbNeed - users_info[u].sum_len);
}
int cur_f = accepted;
if (best_f < cur_f) {
best_f = cur_f;
best_index = index;
}
}
ASSERT(best_index != -1, "invalid best_index");
change_interval_len(block, best_index, len);*/
// TODO: медленно, но хорошо дает скор
/*for (; len > 0; len--) {
int best_index = -1, best_f = -1e9;
for (int index = 0; index < intervals[block].size(); index++) {
int accepted = 0;
for (int u: intervals[block][index].users) {
if (users_info[u].sum_len < users_info[u].rbNeed) {
accepted++;
}
}
int cur_f = accepted;
if (best_f < cur_f) {
best_f = cur_f;
best_index = index;
}
}
ASSERT(best_index != -1, "invalid best_index");
change_interval_len(block, best_index, +1);
}*/
/*while(len > 0){
// (accepted, min_add_len, index)
vector<tuple<int, int, int>> ips;
for (int index = 0; index < intervals[block].size(); index++) {
int cnt = 0;
int min_add_len = 10000;
for (int u: intervals[block][index].users) {
if (users_info[u].sum_len < users_info[u].rbNeed) {
min_add_len = min(min_add_len, users_info[u].rbNeed - users_info[u].sum_len);
cnt++;
}
}
ips.emplace_back(cnt * min_add_len, min_add_len, index);
}
sort(ips.begin(), ips.end(), greater<>());
ASSERT(!ips.empty(), "ips empty");
auto [accepted, min_add_len, index] = ips[0];
int x = min(min_add_len, len);
ASSERT(x != 0, "invalid x");
len -= x;
change_interval_len(block, index, x);
//ASSERT(best_index != -1, "invalid best_index");
//change_interval_len(block, best_index, +1);
}*/
/*if (intervals[block].size() == 1) {
//ASSERT(false, "don't touch him");
return;
remove_interval(block, 0);
} else {
remove_interval(block, index);
change_interval_len(block, rnd.get(0, intervals[block].size() - 1), len);
}*/
/*if (index > 0 && index + 1 < intervals[block].size()) {
int best_left_len = 0;
ASSERT(best_left_len != -1, "failed");
int right_len = len - best_left_len;
remove_interval(block, index);
change_interval_len(block, index - 1, best_left_len);
change_interval_len(block, index, right_len);
} else if (index > 0) {
remove_interval(block, index);
change_interval_len(block, index - 1, len);
} else if (index + 1 < intervals[block].size()) {
remove_interval(block, index);
change_interval_len(block, index, len);
} else {
remove_interval(block, index);
}*/
}
///======================
///===========USER=======
///======================
/*#define CHOOSE_USER(condition)                     \
int u;                                         \
{                                              \
vector<int> ips;                           \
for (int u = 0; u < N; u++) {              \
auto [b, l, r] = get_user_position(u); \
if (condition) {                       \
ips.push_back(u);                  \
}                                      \
}                                          \
if (ips.empty()) {                         \
return;                                \
}                                          \
u = ips[rnd.get(0, ips.size() - 1)];       \
}
#define USER_FOR_BEGIN(condition)              \
for (int index = 0; index < 1; index++) {  \
int u = rnd.get(0, N - 1);             \
auto [b, l, r] = get_user_position(u); \
if (condition) {
#define USER_FOR_END \
}                \
}*/
void EgorTaskSolver::user_do_new_interval(int user) {
{
auto [old_b, old_l, old_r] = get_user_position(user);
ASSERT(old_b == -1, "failed");
}
auto f = [&](int len) {
return len;
//abs(len - users_info[u].rbNeed);
};
int beam = users_info[user].beam;
auto &users = users_beam[beam];
int best_b = -1, best_l = -1, best_r = -1, best_f = -1e9;
for (int b = 0; b < B; b++) {
for (int l = 0; l < intervals[b].size(); l++) {
int sum_len = 0;
int pos = users.size() - 1;
ASSERT(pos >= 0, "invalid pos");
for (int r = l; r < intervals[b].size() &&
intervals[b][r].users.size() < L &&
((intervals[b][r].beam_msk >> beam) & 1) == 0;
r++) {
sum_len += intervals[b][r].len;
/*int cur_f = min(users_info[0].rbNeed, sum_len);
if (best_f < cur_f) {
best_f = cur_f;
best_b = b;
best_l = l;
best_r = r;
}*/
while (pos > 0 && users_info[users[pos]].rbNeed < sum_len) {
pos--;
}
int cur_f = min(users_info[users[pos]].rbNeed, sum_len);// - min(users_info[users[pos]].rbNeed, users_info[users[pos]].sum_len);// * 5 - max(0, sum_len - users_info[users[pos]].rbNeed);
if (best_f < cur_f) {
best_f = cur_f;
best_b = b;
best_l = l;
best_r = r;
}
}
}
}
if (best_b == -1) {
return;
}
for (int i = best_l; i <= best_r; i++) {
add_user_in_interval(user, best_b, i);
}
}
/*void EgorTaskSolver::user_new_interval() {
int u = rnd.get(0, N - 1);
// TODO: только чуток херит score
//int u;
//{
//    vector<pair<int, int>> ips;
//    for(int user = 0; user < N; user++){
//        ips.emplace_back(abs(users_info[user].sum_len - users_info[user].rbNeed),user);
//    }
//    if(ips.empty()){
//        return;
//    }
//    sort(ips.begin(), ips.end(), greater<>());
//    u = ips[rnd.get(0, min(10, (int)ips.size() - 1))].second;
//}
auto old_metric = metric;
int old_actions_size = actions.size();
user_do_new_interval(u);
if (is_good(old_metric)) {
SNAP_ACTION("user_new_interval " + to_string(u) + " accepted");
} else {
rollback(old_actions_size);
ASSERT(old_metric == metric, "failed back score");
}
}*/
void EgorTaskSolver::user_remove_and_add() {
//user_RobinHood();
//982593
for (int step = 0; step < 3; step++) {
int u = rnd.get(0, N - 1);
int u2 = rnd.get(0, N - 1);
if (u == u2 || users_info[u].beam == users_info[u2].beam) {
continue;
}
auto old_metric = metric;
auto [old_b, old_l, old_r] = get_user_position(u);
auto [old_b2, old_l2, old_r2] = get_user_position(u2);
int best_b2 = -1, best_l2 = -1, best_r2 = -1;
int best_f2 = -1e9;
for (int b2 = 0; b2 < B; b2++) {
for (int l2 = 0; l2 < intervals[b2].size(); l2++) {
int len = 0;
for (int r2 = l2; r2 < intervals[b2].size() &&
(
// наш юзер u2 там уже лежит
(intervals[b2][r2].users.contains(u2)) ||
(
// размера хватает
intervals[b2][r2].users.size() - int(intervals[b2][r2].users.contains(u)) < L &&
// beam подходит
((intervals[b2][r2].beam_msk >> users_info[u2].beam) & 1) == 0));
r2++) {
len += intervals[b2][r2].len;
if (best_f2 < len) {
best_f2 = len;
best_b2 = b2;
best_l2 = l2;
best_r2 = r2;
}
}
}
}
int best_b = -1, best_l = -1, best_r = -1;
int best_f = -1e9;
for (int b = 0; b < B; b++) {
for (int l = 0; l < intervals[b].size(); l++) {
int len = 0;
for (int r = l; r < intervals[b].size() &&
(intervals[b][r].users.contains(u) || ((intervals[b][r].beam_msk >> users_info[u].beam) & 1) == 0) &&
intervals[b][r].users.size() - int(intervals[b][r].users.contains(u)) - int(intervals[b][r].users.contains(u2)) + int(best_b2 == b && best_l2 <= r && r <= best_r2) < L;
r++) {
len += intervals[b][r].len;
if (best_f < len) {
best_f = len;
best_b = b;
best_l = l;
best_r = r;
}
}
}
}
int accepted = max(0, min(best_f, users_info[u].rbNeed))
+ max(0, min(best_f2, users_info[u2].rbNeed))
- min(users_info[u].rbNeed, users_info[u].sum_len)
- min(users_info[u2].rbNeed, users_info[u2].sum_len);
/*int old_actions_size = actions.size();
auto [old_b, old_l, old_r] = get_user_position(u);
if (old_b != -1) {
for (int i = old_l; i <= old_r; i++) {
remove_user_in_interval(u, old_b, i);
}
}
auto [old_b2, old_l2, old_r2] = get_user_position(u2);
if (old_b2 != -1) {
for (int i = old_l2; i <= old_r2; i++) {
remove_user_in_interval(u2, old_b2, i);
}
}
// add
user_do_new_interval(u2);
user_do_new_interval(u);*/
if (accepted >= 0) {
SNAP_ACTION("user_remove_and_add " + to_string(u) + " " + to_string(u2) + " accepted");
if (old_b != -1) {
for (int i = old_l; i <= old_r; i++) {
remove_user_in_interval(u, old_b, i);
}
}
if (old_b2 != -1) {
for (int i = old_l2; i <= old_r2; i++) {
remove_user_in_interval(u2, old_b2, i);
}
}
if (best_b2 != -1) {
for (int i = best_l2; i <= best_r2; i++) {
add_user_in_interval(u2, best_b2, i);
}
}
if (best_b != -1) {
for (int i = best_l; i <= best_r; i++) {
add_user_in_interval(u, best_b, i);
}
}
} else {
//rollback(old_actions_size);
ASSERT(old_metric == metric, "failed back score");
}
}
/*if(metric.accepted > old_metric.accepted) {
auto [new_b, new_l, new_r] = get_user_position(u);
auto [new_b2, new_l2, new_r2] = get_user_position(u2);
cout << users_info[u].beam << ' ' << old_b << ' ' << old_l << ' ' << old_r << '\n';
cout << users_info[u2].beam << ' ' << old_b2 << ' ' << old_l2 << ' ' << old_r2 << '\n';
cout << users_info[u].beam << ' ' << new_b << ' ' << new_l << ' ' << new_r << '\n';
cout << users_info[u2].beam << ' ' << new_b2 << ' ' << new_l2 << ' ' << new_r2 << '\n';
cout << "============\n";
}*/
}
void EgorTaskSolver::user_RobinHood() {
if (rnd.get_d() < 0.2) {
vector<int> ips;
for (int u: unused_users) {
ips.push_back(u);
}
if (!ips.empty()) {
int u = ips[rnd.get(0, ips.size() - 1)];
user_do_new_interval(u);
}
}
if (rnd.get_d() < 0.1) {
int old_actions_size = actions.size();
auto old_metric = metric;
int u = rnd.get(0, N - 1);
auto [b, l, r] = get_user_position(u);
if (b != -1) {
for (int i = l; i <= r; i++) {
remove_user_in_interval(u, b, i);
}
}
user_do_new_interval(u);
if (is_good(old_metric)) {
} else {
rollback(old_actions_size);
ASSERT(old_metric == metric, "failed back score");
}
}
if (rnd.get_d() < 0.1) {
// (block, index, user)
vector<tuple<int, int, int>> ips;
for (int block = 0; block < B; block++) {
for (int index = 0; index + 1 < intervals[block].size(); index++) {
auto unique_users = intervals[block][index + 1].users;
unique_users = unique_users ^ (unique_users & intervals[block][index].users);
for (int u: unique_users) {
if (users_info[u].sum_len < users_info[u].rbNeed &&
intervals[block][index].users.size() < L &&
((intervals[block][index].beam_msk >> users_info[u].beam) & 1) == 0) {
ips.emplace_back(block, index, u);
}
}
}
for (int index = 1; index < intervals[block].size(); index++) {
auto unique_users = intervals[block][index - 1].users;
unique_users = unique_users ^ (unique_users & intervals[block][index].users);
for (int u: unique_users) {
if (users_info[u].sum_len < users_info[u].rbNeed &&
intervals[block][index].users.size() < L &&
((intervals[block][index].beam_msk >> users_info[u].beam) & 1) == 0) {
ips.emplace_back(block, index, u);
}
}
}
}
if (!ips.empty()) {
auto [b, i, u] = ips[rnd.get(0, ips.size() - 1)];
add_user_in_interval(u, b, i);
}
}
int block, index, u_left, u_right;
block = -1;
bool type;
bool mode;
{
// (f, block, index, u_left, u_right)
vector<tuple<int, int, int, int, int, bool>> ips;
for (int block = 0; block < B; block++) {
for (int index = 0; index < intervals[block].size(); index++) {
auto unique_right_users = intervals[block][index].users;// Отдаём правый край.
if (index + 1 < intervals[block].size()) {
auto and_users = intervals[block][index].users & intervals[block][index + 1].users;
unique_right_users = unique_right_users ^ and_users;
}
for (int u_right: unique_right_users) {
for (auto empty: unused_users) {
if (users_info[empty].beam == users_info[u_right].beam ||
((intervals[block][index].beam_msk >> users_info[empty].beam) & 1) == 0) {
int accepted = min(users_info[u_right].rbNeed, users_info[u_right].sum_len - intervals[block][index].len) +
min(users_info[empty].rbNeed, /*users_info[empty].sum_len*/ +intervals[block][index].len);
ips.emplace_back(accepted, block, index, u_right, empty, true);
}
}
}
}
}
for (int block = 0; block < B; block++) {
for (int index = 0; index < intervals[block].size(); index++) {
auto unique_left_users = intervals[block][index].users;
if (index != 0) {
auto and_users = intervals[block][index].users & intervals[block][index + 1].users;
unique_left_users = unique_left_users ^ and_users;
}
for (int u_left: unique_left_users) {
for (auto empty: unused_users) {
if (users_info[empty].beam == users_info[u_left].beam ||
((intervals[block][index].beam_msk >> users_info[empty].beam) & 1) == 0) {
int accepted = min(users_info[u_left].rbNeed, users_info[u_left].sum_len - intervals[block][index].len) +
min(users_info[empty].rbNeed, /*users_info[empty].sum_len*/ +intervals[block][index].len);
ips.emplace_back(accepted, block, index, u_left, empty, true);
}
}
}
}
}
if (ips.empty()) {
return;
}
//sort(ips.begin(), ips.end(), greater<>());
//int p = 0;
int p = rnd.get(0, ips.size() - 1);
block = get<1>(ips[p]);
index = get<2>(ips[p]);
u_left = get<3>(ips[p]);
u_right = get<4>(ips[p]);
type = get<5>(ips[p]);
}
ASSERT(block != -1, "kek");
int old_actions_size = actions.size();
auto old_metric = metric;
if (type) {
remove_user_in_interval(u_left, block, index);
add_user_in_interval(u_right, block, index);
} else {
ASSERT(false, "kek");
remove_user_in_interval(u_left, block, index);
add_user_in_interval(u_right, block, index);
}
if (is_good(old_metric)) {
if (metric.accepted > old_metric.accepted) {
//cout << "GOOOOOOD" << endl;
}
} else {
rollback(old_actions_size);
ASSERT(old_metric == metric, "failed back score");
}
}
EgorTaskSolver::EgorTaskSolver(int NN, int MM, int KK, int JJ, int LL,
const vector<Interval> &reservedRBs,
const vector<UserInfo> &userInfos,
vector<Interval> start_intervals, int random_seed, vector<int> powers,
vector<int> metric_coefs) : N(NN), M(MM),
K(KK), J(JJ),
L(LL),
SELECTION_ACTION(powers),
METRICS_COEF(metric_coefs) {
rnd.generator = mt19937_64(random_seed);
ASSERT(METRICS_COEF.size() == METRIC_COEFFS_SIZE, "invalid size");
ASSERT(2 <= L && L <= 16, "invalid L");
ASSERT(0 < J && J <= 16, "invalid J");
users_info.resize(N);
user_id_to_u.resize(N);
{
for (int u = 0; u < N; u++) {
ASSERT(u == userInfos[u].id, "are you stupid or something?");
ASSERT(0 <= userInfos[u].beam && userInfos[u].beam < 32, "invalid beam");
users_info[u].id = userInfos[u].id;
users_info[u].rbNeed = userInfos[u].rbNeed;
users_info[u].beam = userInfos[u].beam;
unused_users.insert(u);
}
for (int u = 0; u < N; u++) {
user_id_to_u[users_info[u].id] = u;
users_beam[userInfos[u].beam].push_back(u);
}
// TODO: можно еще для каждого beam оставить только J первых
for (int beam = 0; beam < 32; beam++) {
sort(users_beam[beam].begin(), users_beam[beam].end(), [&](int lhs, int rhs) {
return users_info[lhs].rbNeed > users_info[rhs].rbNeed;
});
for (int i = 0; i < users_beam[beam].size(); i++) {
users_info[users_beam[beam][i]].pos = i;
}
}
}
// build free_intervals
{
vector<bool> is_free(M + 1, true);
is_free[M] = false;
for (const auto &[start, end, users]: reservedRBs) {
for (int i = start; i < end; i++) {
is_free[i] = false;
}
}
int start = -1;
for (int i = 0; i <= M; i++) {
if (!is_free[i]) {
if (start != i - 1) {
free_intervals.push_back({start + 1, i});
metric.unused_space += i - start - 1;
}
start = i;
}
}
B = free_intervals.size();
}
// build from start_intervals
{
sort(start_intervals.begin(), start_intervals.end(), [&](const auto &lhs, const auto &rhs) {
return lhs.start < rhs.start;
});
intervals.resize(B);
for (auto [start, end, users]: start_intervals) {
ASSERT(start < end, "invalid interval");
bool already_push = false;
for (int block = 0; block < B; block++) {
if (free_intervals[block].start <= start && end <= free_intervals[block].end) {
ASSERT(!already_push, "double push");
already_push = true;
intervals[block].push_back({0, 0, {}});
//intervals[block].push_back({0, {}, 0});
change_interval_len(block, intervals[block].size() - 1, end - start);
for (int u: users) {
add_user_in_interval(u, block, intervals[block].size() - 1);
}
}
}
ASSERT(already_push, "no push");
}
{
vector<pair<int, int>> ips;
for (int block = 0; block < B; block++) {
int len = get_block_len(block);
if (len < free_intervals[block].len()) {
if (!intervals[block].empty()) {
change_interval_len(block, intervals[block].size() - 1, free_intervals[block].len() - len);
} else {
ips.emplace_back(free_intervals[block].len(), block);
}
}
}
sort(ips.begin(), ips.end(), greater<>());
for (auto [len, block]: ips) {
if (get_intervals_size() < J) {
intervals[block].push_back(SetInterval());
change_interval_len(block, 0, free_intervals[block].len());
}
}
}
}
}
int accepted_inc[10]{};
int accepted_more[10]{};
#define ACTION_WRAPPER(action_foo, action_id)                       \
int old_accepted = metric.accepted;                             \
action_foo();                                                   \
if (old_accepted < metric.accepted) {                           \
accepted_inc[action_id]++;                                  \
accepted_more[action_id] += metric.accepted - old_accepted; \
}
int itt = 0;
vector<Interval> EgorTaskSolver::annealing(vector<Interval> reservedRBs,
vector<UserInfo> userInfos) {
itt++;
temperature = 1;
//#define SAVE_BEST_ANS
#ifdef SAVE_BEST_ANS
int best_score = metric.accepted;
auto best_users_info = users_info;
auto best_intervals = intervals;
#endif
//int best_f = metric.accepted;
//int there_has_been_no_improvement_for_x_steps = 0;
for (int step = 0; step < STEPS; step++) {
temperature = ((STEPS - step) * 0.01 / STEPS);
//cout << "TEMPER: " << temperature << endl;
was_accepted = false;
//temperature *= 0.9999;
//cout << "step: " << step << endl;
//if(step == 4999){
//    cout << "HERE" << endl;
//}
//TRAIN_SCORE += best_score;
/*if (best_f == metric.accepted) {
there_has_been_no_improvement_for_x_steps++;
if (there_has_been_no_improvement_for_x_steps > 20) {
break;
}
} else {
there_has_been_no_improvement_for_x_steps = 0;
}*/
#ifdef MY_DEBUG_MODE
/*for(int u = 0; u + 1 < N; u++){
ASSERT(users_info[u].rbNeed >= users_info[u+1].rbNeed, "failed");
}*/
for (int block = 0; block < B; block++) {
for (int index = 0; index < intervals[block].size(); index++) {
if (intervals[block][index].len == 0) {
ASSERT(false, "zero interval");
}
if (index + 1 < intervals[block].size()) {
if (intervals[block][index].users == intervals[block][index + 1].users &&
intervals[block][index].users.size() == L) {
//change_interval_len(block, index, intervals[block][index + 1].len);
//remove_interval(block, index + 1);
}
}
}
}
#endif
ASSERT(get_solution_score(N, M, K, J, L, reservedRBs, userInfos, get_total_answer()) == metric.accepted,
"invalid total_score");
if (THEORY_MAX_SCORE <= metric.accepted) {
break;
}
auto old_metric = metric;
int s = SELECTION_ACTION.select();
if (s == 0) {
ACTION_WRAPPER(user_remove_and_add, s);
} else if (s == 1) {
ACTION_WRAPPER(interval_flow_over, s);
} else if (s == 2) {
ACTION_WRAPPER(interval_split, s);
} else {
ASSERT(false, "kek");
}
if (metric.accepted > old_metric.accepted) {
accepted_inc[s]++;
accepted_more[s] += metric.accepted - old_metric.accepted;
}
#ifdef SAVE_BEST_ANS
if (best_score < metric.accepted) {
best_score = metric.accepted;
best_users_info = users_info;
best_intervals = intervals;
}
#endif
actions.clear();
#ifdef VERIFY_ARTEM_ANS
actions.clear();
auto ans = get_total_answer();
auto get_egor_blocked = ans_to_blocked_ans(M, K, reservedRBs, ans);
optimize(N, M, K, J, L, reservedRBs, userInfos, get_egor_blocked, true);
auto egor_answer = unblock_ans(get_egor_blocked);
if (egor_answer.size() > J) {
ans = get_total_answer();
::abort();
}
#endif
//SNAP(snapshoter.write(get_total_answer(), "annealing"));
}
//    cout << itt << endl;
/*if (itt == 610){
for (int i = 0; i < 10; i++){
cout << accepted_inc[i] << " ";
}
cout << endl;
for (int i = 0; i < 10; i++){
cout << accepted_more[i] << " ";
}
cout << endl;
}*/
#ifdef SAVE_BEST_ANS
users_info = best_users_info;
intervals = best_intervals;
return get_total_answer();
#else
return get_total_answer();
#endif
}
vector<Interval> Solver_egor(int N, int M, int K, int J, int L,
const vector<Interval> &reservedRBs,
const vector<UserInfo> &userInfos, const std::vector<Interval> &solution, int random_seed,
vector<int> powers, vector<int> metric_coefs) {
EgorTaskSolver solver(N, M, K, J, L, reservedRBs, userInfos, solution, random_seed, powers, metric_coefs);
auto answer = solver.annealing(reservedRBs, userInfos);
//ASSERT(solver.metric.accepted == get_solution_score(N, M, K, J, L, reservedRBs, userInfos, answer), "invalid total_score");
return answer;
}
vector<Interval> Solver_egor(int N, int M, int K, int J, int L,
const vector<Interval> &reservedRBs,
const vector<UserInfo> &userInfos, const std::vector<Interval> &solution, int random_seed,
vector<int> powers, vector<int> metric_coefs);
vector<Interval> Solver(int N, int M, int K, int J, int L,
vector<Interval> reservedRBs,
vector<UserInfo> userInfos);
vector<Interval> Solver(const TestData &testdata) {
return Solver(testdata.N, testdata.M, testdata.K, testdata.J, testdata.L, testdata.reservedRBs, testdata.userInfos);
}
vector<Interval> Solver_IMPL(int N, int M, int K, int J, int L,
vector<Interval> reservedRBs,
vector<UserInfo> userInfos) {
auto artem_answer = Solver_Artem_grad(N, M, K, J, L, reservedRBs, userInfos);
auto artem_score = get_solution_score(N, M, K, J, L, reservedRBs, userInfos, artem_answer);
ASSERT(THEORY_MAX_SCORE >= artem_score, "WA THEORMAX");
if (THEORY_MAX_SCORE <= artem_score || get_time_ms() >   950) {
return artem_answer;
}
auto egor_answer = Solver_egor(N, M, K, J, L, reservedRBs, userInfos, artem_answer, 42,
//vector<int>{0, 3, 1, 7, 0, 89, 0, 0, 90, 40, 23} // OK
//          0  1  2  3  4   5  6  7   8   9  10
//vector<int>{5, 0, 0, 0, 0, 5, 5, 5, 5, 5},
vector<int>{5, 5, 5},
vector<int>{100, 0, 0, 0});
//vector<int>{5, 0, 0, 0, 0, 5, 5, 0,  5,  5,  5});
//auto egor_score = get_solution_score(N, M, K, J, L, reservedRBs, userInfos, egor_answer);
//return egor_answer;
//cout << "J: " << J << endl;
//cout << "egor_answer size: " << egor_answer.size() << endl;
auto get_egor_blocked = ans_to_blocked_ans(M, K, reservedRBs, egor_answer);
optimize(N, M, K, J, L, reservedRBs, userInfos, get_egor_blocked, false);
egor_answer = unblock_ans(get_egor_blocked);
//cout << "egor_answer size after optimize: " << egor_answer.size() << endl;
auto egor_score = get_solution_score(N, M, K, J, L, reservedRBs, userInfos, egor_answer);
if (egor_score > artem_score) {
return egor_answer;
} else {
return artem_answer;
}
}
vector<Interval> Solver_BUFF(int N, int M, int K, int J, int L,
vector<Interval> reservedRBs,
vector<UserInfo> userInfos) {
auto back_mapping = reduce_users(N, J, userInfos);
THEORY_MAX_SCORE = get_theory_max_score(N, M, K, J, L, reservedRBs, userInfos);
auto answer = Solver_IMPL(N, M, K, J, L, reservedRBs, userInfos);
normalize_id(answer, back_mapping);
return answer;
}
vector<Interval> Solver(int N, int M, int K, int J, int L,
vector<Interval> reservedRBs,
vector<UserInfo> userInfos) {
auto answer = Solver_BUFF(N, M, K, J, L, reservedRBs, userInfos);
#ifdef MY_DEBUG_MODE
get_solution_score(N, M, K, J, L, reservedRBs, userInfos, answer);
#endif
return answer;
}
