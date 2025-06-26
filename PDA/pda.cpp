#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <stack>
#include <sstream>


struct PDA {
    std::set<std::string> states;
    std::set<char> input_alphabet;
    std::set<char> stack_alphabet;
    std::string initial_state;
    char initial_stack_symbol;
    std::set<std::string> final_states;
    std::map<std::tuple<std::string, char, char>, std::vector<std::pair<std::string, std::string>>> transitions;
};

PDA load_pda_config(const std::string& filename) {
    PDA pda;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        if (line.empty() || line[0] == '#') continue;

        if (line.rfind("STATES:", 0) == 0) {
            std::istringstream ss(line.substr(7));
            std::string state;
            while (std::getline(ss, state, ',')) {
                pda.states.insert(state);
            }
        } else if (line.rfind("INPUT_ALPHABET:", 0) == 0) {
            std::istringstream ss(line.substr(16));
            char symbol;
            while (ss >> symbol) {
                pda.input_alphabet.insert(symbol);
            }
        } else if (line.rfind("STACK_ALPHABET:", 0) == 0) {
            std::istringstream ss(line.substr(16));
            char symbol;
            while (ss >> symbol) {
                pda.stack_alphabet.insert(symbol);
            }
        } else if (line.rfind("INITIAL_STATE:", 0) == 0) {
            pda.initial_state = line.substr(14);
        } else if (line.rfind("INITIAL_STACK_SYMBOL:", 0) == 0) {
            pda.initial_stack_symbol = line[21];
        } else if (line.rfind("FINAL_STATES:", 0) == 0) {
            std::istringstream ss(line.substr(13));
            std::string state;
            while (std::getline(ss, state, ',')) {
                pda.final_states.insert(state);
            }
        } else if (line.find("->") != std::string::npos) {
            std::string left = line.substr(0, line.find("->"));
            std::string right = line.substr(line.find("->") + 2);

            std::istringstream left_ss(left.substr(1, left.size() - 2));
            std::istringstream right_ss(right.substr(1, right.size() - 2));
            
            std::string current_state, input_symbol_str, stack_top;
            char input_symbol, stack_symbol;
            left_ss >> current_state >> input_symbol_str >> stack_top;

            input_symbol = input_symbol_str == "ε" ? '\0' : input_symbol_str[0];
            stack_symbol = stack_top[0];

            std::string next_state, stack_replacement;
            right_ss >> next_state >> stack_replacement;

            if (stack_replacement == "ε") stack_replacement = "";

            auto key = std::make_tuple(current_state, input_symbol, stack_symbol);
            pda.transitions[key].emplace_back(next_state, stack_replacement);
        }
    }
    return pda;
}

bool simulate_pda(const PDA& pda, const std::string& input_string, int pos, const std::string& current_state, std::string stack) {
    if (pos == input_string.size()) {
        if (pda.final_states.find(current_state) != pda.final_states.end()) return true;

        if (!stack.empty()) {
            char stack_top = stack.back();
            auto key = std::make_tuple(current_state, '\0', stack_top);
            if (pda.transitions.find(key) != pda.transitions.end()) {
                for (const auto& [next_state, stack_replacement] : pda.transitions.at(key)) {
                    std::string new_stack = stack.substr(0, stack.size() - 1) + stack_replacement;
                    if (simulate_pda(pda, input_string, pos, next_state, new_stack)) return true;
                }
            }
        }
        return false;
    }

    char current_symbol = input_string[pos];
    if (!stack.empty()) {
        char stack_top = stack.back();
        auto key = std::make_tuple(current_state, current_symbol, stack_top);
        if (pda.transitions.find(key) != pda.transitions.end()) {
            for (const auto& [next_state, stack_replacement] : pda.transitions.at(key)) {
                std::string new_stack = stack.substr(0, stack.size() - 1) + stack_replacement;
                if (simulate_pda(pda, input_string, pos + 1, next_state, new_stack)) return true;
            }
        }
    }

    if (!stack.empty()) {
        char stack_top = stack.back();
        auto key = std::make_tuple(current_state, '\0', stack_top);
        if (pda.transitions.find(key) != pda.transitions.end()) {
            for (const auto& [next_state, stack_replacement] : pda.transitions.at(key)) {
                std::string new_stack = stack.substr(0, stack.size() - 1) + stack_replacement;
                if (simulate_pda(pda, input_string, pos, next_state, new_stack)) return true;
            }
        }
    }
    return false;
}

bool accept(const PDA& pda, const std::string& input_string) {
    return simulate_pda(pda, input_string, 0, pda.initial_state, std::string(1, pda.initial_stack_symbol));
}

int main() {
    PDA pda = load_pda_config("data.in");

    std::vector<std::string> test_strings = {"ab", "aabb", "aaabbb", "aaaabbbb", "a", "b", "ba", "aab", "abb", "abab", ""};

    std::cout << "Testare PDA\n";
    std::cout << "Limbajul recunoscut: L = {a^n b^n | n ≥ 1}\n\n";

    for (const auto& test_string : test_strings) {
        std::string display = test_string.empty() ? "ε (string vid)" : test_string;
        std::string result = accept(pda, test_string) ? "ACCEPTAT" : "RESPINS";
        std::cout << "String: '" << display << "' -> " << result << '\n';
    }

    return 0;
}
