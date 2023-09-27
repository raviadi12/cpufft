#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <CL/cl.hpp>

std::vector<std::string> cartesianProduct(const std::string &charset, int length) {
    if (length == 1) {
        std::vector<std::string> singleChars;
        for (char c : charset) {
            singleChars.push_back(std::string(1, c));
        }
        return singleChars;
    }

    std::vector<std::string> result;
    std::vector<std::string> shorter = cartesianProduct(charset, length - 1);
    for (const std::string &str : shorter) {
        for (char c : charset) {
            result.push_back(str + c);
        }
    }
    return result;
}

int main() {
    const std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const std::string targetPassword = "LuAa";
    auto start = std::chrono::high_resolution_clock::now();

    for (int length = 1; length <= targetPassword.length(); ++length) {
        std::vector<std::string> combinations = cartesianProduct(charset, length);
        for (const std::string &guess : combinations) {
            if (guess == targetPassword) {
                auto stop = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
                double elapsedTime = duration.count() / 1e6;  // Convert to seconds
                std::cout << "The time it takes to Brute Force " << guess << " is " << elapsedTime << " seconds" << std::endl;
                std::cout << "Press Enter to continue...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                return 0;
            }
        }
    }

    std::cout << "Password not found" << std::endl;
    return 1;
}
