#include "random.h"

Random::Random() : mt(rd()) {}

Random &Random::getInstance()
{
    static Random instance;
    return instance;
}

int Random::gen(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(mt);
}