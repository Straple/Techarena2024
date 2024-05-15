#pragma once

#include "assert.hpp"
#include "my_bit_set.hpp"
#include "randomizer.hpp"
#include "selection_randomizer.hpp"
#include "tools.hpp"
#include "artem_solver.hpp"

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
int STEPS = 420;

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
                             const vector<UserInfo> &userInfos, const std::vector<Interval> &solution, int random_seed,
                             vector<int> powers, vector<int> metric_coefs);
