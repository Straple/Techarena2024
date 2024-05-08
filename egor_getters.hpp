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

