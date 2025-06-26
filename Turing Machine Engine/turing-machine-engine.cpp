#include <iostream>
#include <vector>
#include <string>

std::string turingMachineDuplicate(const std::string& initialString, int N) {
    std::vector<char> tape(initialString.begin(), initialString.end());
    int headPosition = 0;
    std::string state = "start";
    int currentDuplication = 0;

    while (currentDuplication < N - 1) {
        if (state == "start") {
            if (headPosition < tape.size()) {
                headPosition++;
            } else {
                state = "copy";
                headPosition = 0;
            }
        } else if (state == "copy") {
            if (headPosition < initialString.size()) {
                tape.push_back(initialString[headPosition]);
                headPosition++;
            } else {
                currentDuplication++;
                state = "start";
                headPosition = 0;
            }
        }
    }

    return std::string(tape.begin(), tape.end());
}

int main() {
    std::string initialString = "abc";
    int N = 3;

    std::string result = turingMachineDuplicate(initialString, N);
    std::cout << "Rezultatul duplicării de " << N << " ori a '" << initialString << "': " << result << std::endl;

    return 0;
}
