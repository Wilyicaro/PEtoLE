/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "ImprovedNoise.hpp"

real ImprovedNoise::field_4294_f = 0.5 * (Mth::sqrt(3.0) - 1.0);
real ImprovedNoise::field_4293_g = (3.0 - Mth::sqrt(3.0)) / 6.0;

ImprovedNoise::ImprovedNoise()
{
	init(new Random);
}

ImprovedNoise::ImprovedNoise(Random* pRandom)
{
	init(pRandom);
}

void ImprovedNoise::init(Random* pRandom)
{
	m_offsetX = pRandom->nextDouble() * 256.0;
	m_offsetY = pRandom->nextDouble() * 256.0;
	m_offsetZ = pRandom->nextDouble() * 256.0;
	
    for (int i = 0; i < 256; ++i)
        m_permutation[i] = i;

	for (int i = 0; i < 256; i++)
	{
		int x = pRandom->nextInt(256 - i) + i;
		int t = m_permutation[i];
		m_permutation[i] = m_permutation[x];
		m_permutation[x] = t;
		m_permutation[256 + i] = m_permutation[i];
	}
}

real ImprovedNoise::getValue(real x, real y)
{
	return getValue(x, y, 0.0);
}

real ImprovedNoise::getValue(real x, real y, real z)
{
	return noise(x, y, z);
}

real ImprovedNoise::lerp(real prog, real a, real b) const
{
	return a + (b - a) * prog;
}

real ImprovedNoise::grad(int hash, real x, real y, real z) const
{
	int h = hash & 0xF;
	real u = h < 8 ? x : y;
	real v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

real ImprovedNoise::grad2(int hash, real x, real z) const
{
    int h = hash & 15;
    real u = (1 - ((h & 8) >> 3)) * x;
    real v = h < 4 ? 0.0 : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

real ImprovedNoise::fade(real x) const
{
	return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
}

real ImprovedNoise::noise(real x, real y, real z)
{
    x += m_offsetX;
    y += m_offsetY;
    z += m_offsetZ;

    int X = Mth::floor(x) & 255,
        Y = Mth::floor(y) & 255,
        Z = Mth::floor(z) & 255;

    x -= Mth::floor(x);
    y -= Mth::floor(y);
    z -= Mth::floor(z);

    float u = fade(x),
        v = fade(y),
        w = fade(z);

    int* p = m_permutation;
    int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z,
        B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

    return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
        grad(p[BA], x - 1, y, z)),
        lerp(u, grad(p[AB], x, y - 1, z),
            grad(p[BB], x - 1, y - 1, z))),
        lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),
            grad(p[BA + 1], x - 1, y, z - 1)),
            lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
                grad(p[BB + 1], x - 1, y - 1, z - 1))));
}

void ImprovedNoise::add(real* output, real x, real y, real z, int xSize, int ySize, int zSize, real xScale, real yScale, real zScale, real amplitude) 
{
    int i = 0;

    if (ySize == 1) {
        int i = 0;
        real invAmp = 1.0 / amplitude;

        for (int xIdx = 0; xIdx < xSize; ++xIdx) {
            real dx = (x + xIdx) * xScale + m_offsetX;
            int xInt = Mth::floor(dx);
            int xLow = xInt & 255;
            dx -= xInt;
            real fadeX = fade(dx);

            for (int zIdx = 0; zIdx < zSize; ++zIdx) {
                real dz = (z + zIdx) * zScale + m_offsetZ;
                int zInt = Mth::floor(dz);
                int zLow = zInt & 255;
                dz -= zInt;
                real fadeZ = fade(dz);

                int a = m_permutation[xLow] + 0;
                int aa = m_permutation[a] + zLow;
                int b = m_permutation[xLow + 1] + 0;
                int ba = m_permutation[b] + zLow;

                real grad1 = grad2(m_permutation[aa], dx, dz);
                real grad2 = grad(m_permutation[ba], dx - 1.0, 0.0, dz);

                real lerp1 = lerp(fadeX, grad1, grad2);

                real grad3 = grad(m_permutation[aa + 1], dx, 0.0, dz - 1.0);
                real grad4 = grad(m_permutation[ba + 1], dx - 1.0, 0.0, dz - 1.0);

                real lerp2 = lerp(fadeX, grad3, grad4);

                real finalVal = lerp(fadeZ, lerp1, lerp2);

                output[i++] += finalVal * invAmp;
            }
        }
    }
    else {
        int i = 0;
        int vc = -1;
        real invAmp = 1.0 / amplitude;
        real lerpX1 = 0, lerpX2 = 0, lerpX3 = 0, lerpX4 = 0;

        for (int xIdx = 0; xIdx < xSize; ++xIdx) {
            real xPos = (x + xIdx) * xScale + m_offsetX;
            int xInt = Mth::floor(xPos);
            xPos -= xInt;
            int xLow = xInt & 255;
            real fadeX = fade(xPos);

            for (int zIdx = 0; zIdx < zSize; ++zIdx) {
                real zPos = (z + zIdx) * zScale + m_offsetZ;
                int zInt = Mth::floor(zPos);
                zPos -= zInt;
                int zLow = zInt & 255;
                real fadeZ = fade(zPos);

                for (int yIdx = 0; yIdx < ySize; ++yIdx) {
                    real yPos = (y + yIdx) * yScale + m_offsetY;
                    int yInt = Mth::floor(yPos);
                    yPos -= yInt;
                    int yLow = yInt & 255;
                    real fadeY = fade(yPos);

                    if (yIdx == 0 || yLow != vc) {
                        vc = yLow;
                        int A = m_permutation[xLow] + yLow;
                        int AA = m_permutation[A] + zLow;
                        int AB = m_permutation[A + 1] + zLow;
                        int B = m_permutation[xLow + 1] + yLow;
                        int BA = m_permutation[B] + zLow;
                        int BB = m_permutation[B + 1] + zLow;

                        lerpX1 = lerp(fadeX, grad(m_permutation[AA], xPos, yPos, zPos), grad(m_permutation[BA], xPos - 1, yPos, zPos));
                        lerpX2 = lerp(fadeX, grad(m_permutation[AB], xPos, yPos - 1, zPos), grad(m_permutation[BB], xPos - 1, yPos - 1, zPos));
                        lerpX3 = lerp(fadeX, grad(m_permutation[AA + 1], xPos, yPos, zPos - 1), grad(m_permutation[BA + 1], xPos - 1, yPos, zPos - 1));
                        lerpX4 = lerp(fadeX, grad(m_permutation[AB + 1], xPos, yPos - 1, zPos - 1), grad(m_permutation[BB + 1], xPos - 1, yPos - 1, zPos - 1));
                    }

                    real finalVal = lerp(fadeZ, lerp(fadeY, lerpX1, lerpX2), lerp(fadeY, lerpX3, lerpX4));

                    output[i++] += finalVal * invAmp;
                }
            }
        }
    }
}

void ImprovedNoise::addBiome(real* var1, real var2, real var4, int var6, int var7, real var8, real var10, real var12)
{
    int var14 = 0;

    for (int var15 = 0; var15 < var6; ++var15) {
        real var16 = (var2 + (real)var15) * var8 + m_offsetX;

        for (int var18 = 0; var18 < var7; ++var18) {
            real var19 = (var4 + (real)var18) * var10 + m_offsetY;
            real var27 = (var16 + var19) * field_4294_f;
            int var29 = Mth::floor(var16 + var27);
            int var30 = Mth::floor(var19 + var27);
            real var31 = (real)(var29 + var30) * field_4293_g;
            real var33 = (real)var29 - var31;
            real var35 = (real)var30 - var31;
            real var37 = var16 - var33;
            real var39 = var19 - var35;
            uint8_t var41;
            uint8_t var42;
            if (var37 > var39) {
                var41 = 1;
                var42 = 0;
            }
            else {
                var41 = 0;
                var42 = 1;
            }

            real var43 = var37 - real(var41) + field_4293_g;
            real var45 = var39 - real(var42) + field_4293_g;
            real var47 = var37 - 1.0 + 2.0 * field_4293_g;
            real var49 = var39 - 1.0 + 2.0 * field_4293_g;
            int var51 = var29 & 255;
            int var52 = var30 & 255;
            int var53 = m_permutation[var51 + m_permutation[var52]] % 12;
            int var54 = m_permutation[var51 + var41 + m_permutation[var52 + var42]] % 12;
            int var55 = m_permutation[var51 + 1 + m_permutation[var52 + 1]] % 12;
            real var56 = 0.5 - var37 * var37 - var39 * var39;
            real var21;
            if (var56 < 0.0) {
                var21 = 0.0;
            }
            else {
                var56 *= var56;
                var21 = var56 * var56 * func_4156_a(field_4296_d[var53], var37, var39);
            }

            real var58 = 0.5 - var43 * var43 - var45 * var45;
            real var23;
            if (var58 < 0.0) {
                var23 = 0.0;
            }
            else {
                var58 *= var58;
                var23 = var58 * var58 * func_4156_a(field_4296_d[var54], var43, var45);
            }

            real var60 = 0.5 - var47 * var47 - var49 * var49;
            real var25;
            if (var60 < 0.0) {
                var25 = 0.0;
            }
            else {
                var60 *= var60;
                var25 = var60 * var60 * func_4156_a(field_4296_d[var55], var47, var49);
            }

            int var10001 = var14++;
            var1[var10001] += 70.0 * (var21 + var23 + var25) * var12;
        }
    }

}
