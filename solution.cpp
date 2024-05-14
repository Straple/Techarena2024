#include "assert.hpp"
#include "global_variables.hpp"
#include "my_bit_set.hpp"
#include "randomizer.hpp"
#include "selection_randomizer.hpp"
#include "timer.hpp"

#include "snapshoter.hpp"
#include "tools.hpp"

#include "artem_solver.hpp"
#include "egor_solver.hpp"

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
    if (THEORY_MAX_SCORE <= artem_score/* || clock() * 1.0 / CLOCKS_PER_SEC > 0.95*/) {
        return artem_answer;
    }

    auto egor_answer = Solver_egor(N, M, K, J, L, reservedRBs, userInfos, artem_answer, 42,
                                   //vector<int>{0, 3, 1, 7, 0, 89, 0, 0, 90, 40, 23} // OK
                                   //          0  1  2  3  4   5  6  7   8   9  10
                                   //vector<int>{5, 0, 0, 0, 0, 5, 5, 5, 5, 5},
                                   vector<int>{5, 5, 5},
                                   vector<int>{100, 0, 0, 0});
    //vector<int>{5, 0, 0, 0, 0, 5, 5, 0, 5, 5, 5});

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
    auto answer = Solver_IMPL(N, M, K, J, L, reservedRBs, userInfos);
    normalize_id(answer, back_mapping);
    return answer;
}

vector<Interval> Solver(int N, int M, int K, int J, int L,
                        vector<Interval> reservedRBs,
                        vector<UserInfo> userInfos) {
    THEORY_MAX_SCORE = get_theory_max_score(N, M, K, J, L, reservedRBs, userInfos);
    auto answer = Solver_BUFF(N, M, K, J, L, reservedRBs, userInfos);
#ifdef MY_DEBUG_MODE
    get_solution_score(N, M, K, J, L, reservedRBs, userInfos, answer);
#endif
    return answer;
}
