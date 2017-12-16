#include <cmath>
#include <string>
#include "Factorizer.h"


Factorizer::Factorizer(uint64_t n) {

    this->number = n;
    this->getSieve();

}

Factorizer::Factorizer(std::vector<uint64_t> factors) {

    this->factors = factors;

}


void Factorizer::getSieve() {

    uint64_t n = this->number;

    this->sieve.resize(n + 1, 0);

    for (uint64_t i = 2; i < std::sqrt(n); i++) {

        if (this->sieve[i] != 0) {
            continue;
        }

        uint64_t currentNumber = i * i;

        while (currentNumber <= n) {
            this->sieve[currentNumber] = i;
            currentNumber += i;
        }
    }

}

std::vector<uint64_t> Factorizer::getFactors() const {

    if (!this->factors.empty()) {
        return this->factors;
    }

    uint64_t a = this->number;
    while (this->sieve[a] != 0) {
        this->factors.push_back(this->sieve[a]);
        a /= this->sieve[a];
    }

    this->factors.push_back(a);

    std::sort(this->factors.begin(), this->factors.end());

    return this->factors;

}

uint64_t Factorizer::getNumber() const {

    if (this->number != 0) {
        return this->number;
    }

    uint64_t result = 1;

    for (auto factor: this->factors) {
        result *= factor;
    }

    this->number = result;

    return result;

}

std::string Factorizer::toString() const {

    std::string result;

    if (this->factors.empty()) {
        this->getFactors();
    }

    for (auto i: this->factors) {
        result += std::to_string(i);
        result += " ";
    }

    return result;
}

