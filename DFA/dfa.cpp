#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>

using namespace std;

struct Data {
    vector<string> State;
    vector<string> Symbols;
    map<pair<string, string>, string> Rules;
    vector<string> Start_State;
    vector<string> Accept_States;
    vector<string> Tests;
};

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

bool isValidSymbol(const string& symbol, const vector<string>& symbols) {
    return find(symbols.begin(), symbols.end(), symbol) != symbols.end();
}

Data citire(const string& fisier) {
    ifstream file(fisier);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + fisier);
    }

    Data d;
    string line, currentSection;

    while (getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '/') {
            continue; // Skip empty lines or comments
        }

        if (line == "[State]") {
            currentSection = "State";
            d.State.clear();
        } else if (line == "[Symbols]") {
            currentSection = "Symbols";
            d.Symbols.clear();
        } else if (line == "[Rules]") {
            currentSection = "Rules";
            d.Rules.clear();
        } else if (line == "[Start_State]") {
            currentSection = "Start_State";
            d.Start_State.clear();
        } else if (line == "[Accept_States]") {
            currentSection = "Accept_States";
            d.Accept_States.clear();
        } else if (line == "[Tests]") {
            currentSection = "Tests";
            d.Tests.clear();
        } else {
            if (currentSection == "Rules") {
                stringstream ss(line);
                string state1, symbol, state2;
                if (!(getline(ss, state1, ',') && getline(ss, symbol, ',') && getline(ss, state2))) {
                    throw runtime_error("Invalid rule format: " + line);
                }
                state1 = trim(state1);
                symbol = trim(symbol);
                state2 = trim(state2);

                if (!isValidSymbol(symbol, d.Symbols)) {
                    throw runtime_error("Invalid symbol in rule: " + symbol);
                }
                if (find(d.State.begin(), d.State.end(), state1) == d.State.end() ||
                    find(d.State.begin(), d.State.end(), state2) == d.State.end()) {
                    throw runtime_error("State not found in rule: " + line);
                }

                d.Rules[make_pair(state1, symbol)] = state2;
            } else {
                if (currentSection == "Tests") {
                    for (char c : line) {
                        if (!isValidSymbol(string(1, c), d.Symbols)) {
                            throw runtime_error("Invalid symbol in test: " + line);
                        }
                    }
                }
                if (currentSection == "State") d.State.push_back(line);
                else if (currentSection == "Symbols") d.Symbols.push_back(line);
                else if (currentSection == "Start_State") d.Start_State.push_back(line);
                else if (currentSection == "Accept_States") d.Accept_States.push_back(line);
                else if (currentSection == "Tests") d.Tests.push_back(line);
            }
        }
    }

    if (d.State.empty() || d.Start_State.empty() || d.Accept_States.empty() || d.Rules.empty() || d.Tests.empty()) {
        throw runtime_error("Missing necessary fields in input.");
    }

    return d;
}

void validate(const Data& d) {
    for (const string& test : d.Tests) {
        string current_state = d.Start_State[0];
        bool success = true;
        for (char c : test) {
            string symbol(1, c);
            auto it = d.Rules.find(make_pair(current_state, symbol));
            if (it != d.Rules.end()) {
                current_state = it->second;
            } else {
                cout << test << ": Failed (invalid rule: " << current_state << ", " << symbol << ")\n";
                success = false;
                break;
            }
        }
        if (success && find(d.Accept_States.begin(), d.Accept_States.end(), current_state) != d.Accept_States.end()) {
            cout << test << ": Success\n";
        } else if (success) {
            cout << test << ": Failed\n";
        }
    }
}

int main() {
    try {
        Data d = citire("joc2.in");
        validate(d);
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
    }

    return 0;
}
