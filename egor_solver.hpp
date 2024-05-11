#pragma once

#include "assert.hpp"
#include "my_bit_set.hpp"
#include "randomizer.hpp"
#include "selection_randomizer.hpp"
#include "tools.hpp"

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

const int SELECTION_SIZE = 11;
//SelectionRandomizer SELECTION_ACTION(SELECTION_SIZE);
// = std::vector<int>{10, 1, 1, 1, 1, 16, 9, 6, 22, 12, 11, 30};
int STEPS = 1200;

const int METRIC_CNT = 1;
int METRIC_TYPE = 0;

struct EgorTaskSolver {
    ///============================
    /// task data
    ///============================

    //                                                    0  1  2  3  4   5  6  7   8   9  10
    SelectionRandomizer SELECTION_ACTION;// = vector<int>{0, 3, 1, 7, 0, 89, 0, 0, 90, 40, 23};

    long long TRAIN_SCORE = 0;

    int N;
    int M;
    int K;
    int J;
    int L;

    int B;// колво блоков

    int prev_action = 0;
    int score_prev_action = 0;

    vector<MyInterval> free_intervals;

    ///============================
    /// internal data
    ///============================

    randomizer rnd;

    struct SetInterval {
        int len = 0;
        MyBitSet<128> users;
        uint32_t beam_msk = 0;

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

    vector<MyUserInfo> users_info;

    vector<int> users_beam[32];

    vector<int> user_id_to_u;

    vector<int> users_order;

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
                   vector<Interval> start_intervals, int random_seed, vector<int> powers);

    ///===========================
    ///===========ACTIONS=========
    ///===========================

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

    ///==========================
    ///===========RANDOM=========
    ///==========================

    double temperature = 1;

    //add score: 0 693 288
    //TEST CASE: K=0 | tests: 666 | score: 99.2982% | 647764/652342 | time: 28137.4ms | max_time: 77.561ms | mean_time: 42.2483ms
    //TEST CASE: K=1 | tests: 215 | score: 97.8553% | 211483/216118 | time: 9188.19ms | max_time: 39.351ms | mean_time: 42.7358ms
    //TEST CASE: K=2 | tests: 80 | score: 97.7727% | 77523/79289 | time: 3777.18ms | max_time: 38.059ms | mean_time: 47.2147ms
    //TEST CASE: K=3 | tests: 39 | score: 96.4336% | 43615/45228 | time: 1849.04ms | max_time: 35.504ms | mean_time: 47.4114ms
    //TEST CASE: K=4 | tests: 0 | score: -nan% | 0/0 | time: 0ms | max_time: 0ms | mean_time: 0ms
    //TOTAL: tests: 1000 | score: 98.7319% | 980385/992977 | time: 42951.8ms | max_time: 77.561ms | mean_time: 42.9518ms
    bool is_good(Metric old_metric) {
        ASSERT(get_metric() == metric, "invalid metric");

        // 979437 -> 980155 -> 980332 -> 980459
        auto calc_f = [&](Metric m) {
            return 100 * m.accepted - 10 * m.unused_space - m.overflow - m.free_space;

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
            /*else if(METRIC_TYPE == 3){
                //980520
                //return m.accepted * 100 + m.free_space;
                //980526
                //return m.accepted * 10 + m.free_space;

                return m.accepted * 10 - m.overflow;
            }*/
            /*else if(METRIC_TYPE == 1){
                // add score: 0 834 199 51
                // 980488
                return m.accepted * 5 - m.unused_space;
            }
            else if(METRIC_TYPE == 2){
                return m.accepted + m.unused_space;
            }
            else if(METRIC_TYPE == 3){
                return 100 * m.accepted - m.unused_space - m.overflow;
            }*/
            /*else if(METRIC_TYPE == 3){
                return m.accepted * 10000 + m.overflow;
            }
            else if(METRIC_TYPE == 4){
                return m.accepted * 10 + m.free_space;
            }*/

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

        double f = calc_f(metric);
        double old_f = calc_f(old_metric);
        //ASSERT(f > 0, "invalid f");
        //ASSERT(old_f > 0, "invalid old_f");

        return f > old_f || rnd.get_d() < exp((f - old_f) / temperature);

        /*if (metric.accepted > old_metric.accepted) {
            return true;
        } else if (metric.accepted == old_metric.accepted) {
            //return true; // 979363
            //return false;// 966147

            auto calc_f = [](Metric m) {
                //return m.overflow + m.free_space; // 967490
                //return -m.overflow + m.free_space; // 970315
                //return -m.overflow + m.free_space * 10; // 970425
                //return -m.overflow + m.free_space * 100; // 970374
                //return -m.overflow + m.free_space * 15; // 970438
                //return -m.overflow + m.free_space * 12; // 970469
                //return m.free_space;
                return m.overflow;
            };

            //double f = calc_f(metric);
            //double old_f = calc_f(old_metric);
            //ASSERT(f > 0, "invalid f");
            //ASSERT(old_f > 0, "invalid old_f");

            //969488
            //978105

            //979529
            //return f < old_f || rnd.get_d() < exp((f - old_f) / temperature);

            //977855
            return rnd.get_d() < exp((old_metric.overflow - metric.overflow) / temperature);

            //962261
            return rnd.get_d() < exp((metric.overflow - old_metric.overflow) / temperature);
        }
        return rnd.get_d() < exp((metric.accepted - old_metric.accepted) / temperature);*/

        // 979437
        //return metric.accepted > old_metric.accepted || rnd.get_d() < exp((metric.accepted - old_metric.accepted) / temperature);
    }

    ///==========================
    ///===========INTERVAL=======
    ///==========================

    void interval_flow_over();

    void interval_increase_len();

    void interval_decrease_len();

    bool merge_verify(int b, int i);

    void interval_do_merge(int b, int i);

    void interval_do_split(int b, int i, int right_len);

    void interval_merge();

    void interval_split();

    ///======================
    ///===========USER=======
    ///======================

    void user_do_new_interval(int u);

    void user_add_left();

    void user_add_right();

    void user_remove_left();

    void user_remove_right();

    void user_new_interval();

    void user_do_swap_eq_beam(int u, int u2);

    //void user_do_remove_and_add(int u, int u2);

    void user_remove_and_add();

    void user_do_crop(int u);

    void user_crop();

    ///======================
    ///======ANNEALING=======
    ///======================

    vector<Interval> annealing(vector<Interval> reservedRBs,
                               vector<UserInfo> userInfos);
};

#include "egor_getters.hpp"

#include "egor_actions.hpp"

#include "egor_intervals.hpp"

#include "egor_users.hpp"

#include "egor_impl.hpp"

vector<Interval> Solver_egor(int N, int M, int K, int J, int L,
                             const vector<Interval> &reservedRBs,
                             const vector<UserInfo> &userInfos, const std::vector<Interval> &solution, int random_seed, vector<int> powers);
