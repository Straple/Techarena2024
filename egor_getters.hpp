#pragma once

///==========================
///===========GETTERS========
///==========================

vector<Interval> EgorTaskSolver::get_total_answer() const {
    vector<Interval> answer;
    for (int block = 0; block < B; block++) {
        int start = free_intervals[block].start;
        for (auto [len, users, beam_msk]: intervals[block]) {
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
