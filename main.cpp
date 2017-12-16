#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <queue>
#include "Factorizer.h"
#include "Threader.h"

std::string factorize(uint64_t number) {
    Factorizer f(number);

    std::string result =  std::to_string(number) + ": " + f.toString();
    return result;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cout << "Please, pass input and output file names" << std::endl;
        exit(1);
    }

    int threadsCount = 1;

    if (argc == 4) {
        threadsCount = atoi(argv[3]);
    }

    Threader<uint64_t>(std::string(argv[1]), std::string(argv[2]), factorize, threadsCount);

    return 0;
}