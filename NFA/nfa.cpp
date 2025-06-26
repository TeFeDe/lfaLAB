#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>

std::set<std::string> epsilon_closure(const std::set<std::string>& states) {
    return states;
}

bool nfa_accepts(const std::map<std::string, std::set<std::string>>& transitions,
                 const std::string& start_state,
                 const std::set<std::string>& final_states,
                 const std::string& word) {
    std::set<std::string> current_states = {start_state};
    
    for (char symbol : word) {
        std::set<std::string> next_states;
        for (const auto& state : current_states) {
            std::string key = state + ',' + symbol;
            if (transitions.find(key) != transitions.end()) {
                next_states.insert(transitions.at(key).begin(), transitions.at(key).end());
            }
        }
        current_states = next_states;
        if (current_states.empty()) {
            return false;
        }
    }
    for (const auto& state : current_states) {
        if (final_states.find(state) != final_states.end()) {
            return true;
        }
    }
    return false;
}

void parse_nfa_from_file(const std::string& filename,
                         std::set<std::string>& states,
                         std::set<char>& alphabet,
                         std::string& start_state,
                         std::set<std::string>& final_states,
                         std::map<std::string, std::set<std::string>>& transitions,
                         std::vector<std::string>& words) {
    std::ifstream file(filename);
    std::string line;
    std::string reading_section;

    while (std::getline(file, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        if (line.empty()) continue;

        if (line == "Sigma:") {
            reading_section = "sigma";
        } else if (line == "States:") {
            reading_section = "states";
        } else if (line == "Start:") {
            std::getline(file, start_state);
            start_state.erase(std::remove(start_state.begin(), start_state.end(), '\r'), start_state.end());
        } else if (line == "Final:") {
            std::string finals;
            std::getline(file, finals);
            std::istringstream ss(finals);
            std::string state;
            while (ss >> state) {
                final_states.insert(state);
            }
        } else if (line == "Transitions:") {
            reading_section = "transitions";
        } else if (line == "Words:") {
            reading_section = "words";
        } else {
            if (reading_section == "sigma") {
                for (char ch : line) {
                    if (!isspace(ch)) alphabet.insert(ch);
                }
            } else if (reading_section == "states") {
                std::istringstream ss(line);
                std::string state;
                while (ss >> state) {
                    states.insert(state);
                }
            } else if (reading_section == "transitions") {
                std::istringstream ss(line);
                std::string src, sym, dest;
                ss >> src >> sym >> dest;
                std::string key = src + ',' + sym;
                transitions[key].insert(dest);
            } else if (reading_section == "words") {
                words.push_back(line);
            }
        }
    }
}

int main() {
    std::set<std::string> states;
    std::set<char> alphabet;
    std::string start_state;
    std::set<std::string> final_states;
    std::map<std::string, std::set<std::string>> transitions;
    std::vector<std::string> words;

    parse_nfa_from_file("data.in", states, alphabet, start_state, final_states, transitions, words);

    for (const auto& word : words) {
        bool result = nfa_accepts(transitions, start_state, final_states, word);
        std::cout << "Word '" << word << "': " << (result ? "ACCEPTED" : "REJECTED") << std::endl;
    }

    return 0;
}
