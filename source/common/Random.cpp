#include "Random.hpp"
#include <cstdint>
#include <math.h>


std::atomic<int64_t> Random::seedUniquifierValue = { 8682522807148012LL };

int64_t Random::seedUniquifier() 
{
    int64_t current, next;
    do {
        current = seedUniquifierValue.load();
        next = current * 181783497276652981LL;
    } while (!seedUniquifierValue.compare_exchange_weak(current, next));
    return next;
}

Random::Random() {
    setSeed(seedUniquifier() ^ getTimeNano());
}

Random::Random(int64_t seedVal) 
{
    setSeed(seedVal);
}

void Random::setSeed(int64_t seedVal) 
{
    seed = (seedVal ^ multiplier) & mask;
    haveNextNextGaussian = false;
}

int32_t Random::next(int bits) 
{
    seed = (seed * multiplier + addend) & mask;
    return (int32_t)(seed >> (48 - bits));
}

int32_t Random::nextInt() 
{
    return next(32);
}

int32_t Random::nextInt(int bound) 
{
    if (bound <= 0) throw std::invalid_argument("bound must be positive");

    if ((bound & -bound) == bound) {
        return (int32_t)((bound * (int64_t)next(31)) >> 31);
    }

    int32_t bits, val;
    do {
        bits = next(31);
        val = bits % bound;
    } while (bits - val + (bound - 1) < 0);
    return val;
}

int64_t Random::nextLong() {
    return ((int64_t)(next(32)) << 32) + next(32);
}

float Random::nextFloat() {
    return next(24) / ((float)(1 << 24));
}

double Random::nextDouble() 
{
    return (((int64_t)next(26) << 27) + next(27)) / (double)(1LL << 53);
}

bool Random::nextBoolean() 
{
    return next(1) != 0;
}

double Random::nextGaussian() 
{
    if (haveNextNextGaussian) {
        haveNextNextGaussian = false;
        return nextNextGaussian;
    }

    double v1, v2, s;
    do {
        v1 = 2.0 * nextDouble() - 1.0;
        v2 = 2.0 * nextDouble() - 1.0;
        s = v1 * v1 + v2 * v2;
    } while (s >= 1.0 || s == 0.0);

    double multiplier = sqrt(-2.0 * log(s) / s);
    nextNextGaussian = v2 * multiplier;
    haveNextNextGaussian = true;
    return v1 * multiplier;
}
