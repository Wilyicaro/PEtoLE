/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <stdint.h>
#include "common/Utils.hpp"


int getTimeMs();

class Random
{
private:
    int64_t seed;
    bool haveNextNextGaussian = false;
    double nextNextGaussian = 0.0;

    static constexpr int64_t multiplier = 0x5DEECE66DLL;
    static constexpr int64_t addend = 0xBLL;
    static constexpr int64_t mask = (1LL << 48) - 1;
    static std::atomic<int64_t> seedUniquifierValue;

    static int64_t seedUniquifier();
public:
    Random();
    Random(int64_t seedVal);
    void setSeed(int64_t seedVal);

    int32_t next(int bits);
    int32_t nextInt();
    int32_t nextInt(int bound);
    int64_t nextLong();
    float nextFloat();
    double nextDouble();
    bool nextBoolean();
    double nextGaussian();
};
