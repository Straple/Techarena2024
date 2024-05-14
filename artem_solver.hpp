#pragma once

#include "assert.hpp"
#include "snapshoter.hpp"
#include "tools.hpp"

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