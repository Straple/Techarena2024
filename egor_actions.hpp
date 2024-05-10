#pragma once

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

void EgorTaskSolver::change_user_len(int u, int c) {
    auto &users = users_beam[users_info[u].beam];

    /*cout << "kek=======================================\n";
    for (int i = 0; i + 1 < users.size(); i++) {
        cout << users_info[users[i]].sum_len << ' ';
    }
    cout << endl;*/

#ifdef MY_DEBUG_MODE
    for (int i = 0; i + 1 < users.size(); i++) {
        if (users_info[users[i]].sum_len < users_info[users[i + 1]].sum_len) {
            ASSERT(false, "WTF");
        }
    }
#endif

    {
        metric.accepted -= min(users_info[u].rbNeed, users_info[u].sum_len);
        metric.overflow -= max(0, users_info[u].sum_len - users_info[u].rbNeed);

        users_info[u].sum_len += c;

        metric.accepted += min(users_info[u].rbNeed, users_info[u].sum_len);
        metric.overflow += max(0, users_info[u].sum_len - users_info[u].rbNeed);
    }

    // TODO: пройтись по users_beam
    // просвапать нужным образом

    /*cout << "lol=======================================\n";
    for (int i = 0; i < users.size(); i++) {
        cout << users_info[users[i]].sum_len << ' ';
    }
    cout << endl;*/

    //for (int i = 0; i < users.size(); i++)
    {
        // find
        //if (users[i] == u)
        {
            //cout << "find: " << i << endl;

            int i = users_info[u].pos;
            ASSERT(users[i] == u, "failed");
            ASSERT(i == users_info[u].pos, "failed");

            // move left
            while (i > 0 && users_info[users[i - 1]].sum_len < users_info[u].sum_len) {
                user_do_swap_eq_beam(users[i - 1], u);
                swap(users[i - 1], users[i]);
                i--;

                /*cout << "left=======================================\n";
                for (int i = 0; i < users.size(); i++) {
                    cout << users_info[users[i]].sum_len << ' ';
                }
                cout << endl;*/
            }

            // move right
            while (i + 1 < users.size() && users_info[u].sum_len < users_info[users[i + 1]].sum_len) {
                user_do_swap_eq_beam(users[i + 1], u);
                swap(users[i + 1], users[i]);
                i++;

                /*cout << "right=======================================\n";
                for (int i = 0; i < users.size(); i++) {
                    cout << users_info[users[i]].sum_len << ' ';
                }
                cout << endl;*/
            }

            //cout << endl;

#ifdef MY_DEBUG_MODE
            for (int i = 0; i + 1 < users.size(); i++) {
                if (users_info[users[i]].sum_len < users_info[users[i + 1]].sum_len) {
                    ASSERT(false, "WTF");
                }
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

    change_user_len(u, interval.len);
    /*metric.accepted -= min(user.rbNeed, user.sum_len);
    metric.overflow -= max(0, user.sum_len - user.rbNeed);

    user.sum_len += interval.len;
    metric.accepted += min(user.rbNeed, user.sum_len);
    metric.overflow += max(0, user.sum_len - user.rbNeed);*/

    interval.users.insert(u);
    interval.beam_msk ^= (uint32_t(1) << user.beam);

    metric.free_space -= interval.len;
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

    change_user_len(u, -interval.len);
    /*metric.accepted -= min(user.rbNeed, user.sum_len);
    metric.overflow -= max(0, user.sum_len - user.rbNeed);

    user.sum_len -= interval.len;

    metric.accepted += min(user.rbNeed, user.sum_len);
    metric.overflow += max(0, user.sum_len - user.rbNeed);*/

    interval.users.erase(u);
    interval.beam_msk ^= (uint32_t(1) << user.beam);

    metric.free_space += interval.len;
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
