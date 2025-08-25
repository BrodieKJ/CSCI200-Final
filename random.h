#pragma once

#include <random>
using namespace std;

class Random
{

private:
    random_device rd;
    mt19937 mt;

    Random();

public:
    Random(const Random &) = delete;
    Random &operator=(const Random &) = delete;

    static Random &getInstance();

    int gen(int min, int max);
    mt19937 &getGenerator() { return mt; }
};