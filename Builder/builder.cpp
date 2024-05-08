#include <bits/stdc++.h>
using namespace std;
#include "../assert.hpp"

ofstream output("submission.cpp");

set<string> vis;

bool readline(ifstream &input, string &line) {
    if (!getline(input, line)) {
        return false;
    }
    while (!line.empty() && isspace(line.back())) {
        line.pop_back();
    }
    while (!line.empty() && isspace(line[0])) {
        line.erase(line.begin());
    }
    return true;
}

void my_write(ifstream &input) {
    string line;
    if (!readline(input, line)) {
        return;
    }

    if(line == "#pragma once") {
        // no write
    }else if (line.size() >= 10 && line.substr(0, 10) == "#include \"") {
        // parse include
        line = line.substr(10);
        ASSERT(!line.empty(), "line is empty");
        ASSERT(line.back() == '\"', "invalid line");
        line.pop_back();

        if (!vis.contains(line)) {
            vis.insert(line);
            cout << "INCLUDE: \"" << line << "\"" << endl;
            ifstream this_file(line);
            ASSERT(this_file && this_file.is_open(), "unable to open file");
            my_write(this_file);
        } else {
            cout << "ALREADY INCLUDED: \"" << line << "\"" << endl;
        }
    } else {
        if (!line.empty()) {
            output << line << '\n';
        }
    }
    my_write(input);
}

int main() {

    ifstream input("solution.cpp");
    ASSERT(input && input.is_open(), "unable to open file");
    my_write(input);
}