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

vector<Interval> Solver(int N, int M, int K, int J, int L,
                        vector<Interval> reservedRBs,
                        vector<UserInfo> userInfos) {
    // INITIALLY CALC THEORY MAX SCORE
    THEORY_MAX_SCORE = get_theory_max_score(N, M, K, J, L, reservedRBs, userInfos);

    auto artem_answer = Solver_Artem_grad(N, M, K, J, L, reservedRBs, userInfos);
    /*auto artem_score = get_solution_score(N, M, K, J, L, reservedRBs, userInfos, artem_answer);
    ASSERT(THEORY_MAX_SCORE >= artem_score, "WA THEORMAX");
    if (THEORY_MAX_SCORE <= artem_score) {
        return artem_answer;
    }*/

    auto egor_answer = Solver_egor(N, M, K, J, L, reservedRBs, userInfos, artem_answer, 42);
    return egor_answer;

    /*auto get_egor_blocked = ans_to_blocked_ans(M, K, reservedRBs, egor_answer);
    optimize(N, M, K, J, L, reservedRBs, userInfos, get_egor_blocked);
    egor_answer.clear();
    vector<Interval> answer;
    for (int i = 0; i < get_egor_blocked.size(); i++) {
        for (int g = 0; g < get_egor_blocked[i].size(); g++) {
            if (get_egor_blocked[i][g].users.size()) {
                egor_answer.push_back(get_egor_blocked[i][g]);
            }
        }
    }
    auto egor_score = get_solution_score(N, M, K, J, L, reservedRBs, userInfos, egor_answer);

    if (egor_score > artem_score) {
        return egor_answer;
    } else {
        return artem_answer;
    }*/
}
