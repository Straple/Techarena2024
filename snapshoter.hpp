#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;
using namespace std;

#include "tools.hpp"

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