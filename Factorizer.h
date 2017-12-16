#pragma once
#include <vector>

class Factorizer {

public:

    explicit Factorizer(uint64_t n);
    explicit Factorizer(std::vector<uint64_t> factors);

    std::vector<uint64_t> getFactors() const;
    uint64_t getNumber() const;
    std::string toString() const;

private:

    std::vector<uint64_t> sieve;
    mutable std::vector<uint64_t> factors;
    mutable uint64_t number = 0;

    void getSieve();

};