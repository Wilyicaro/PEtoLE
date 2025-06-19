#include "LargeCaveFeature.hpp"

void LargeCaveFeature::addFeature(Level* level, int ax, int az, int x, int z, TileID* tiles, int unk)
{
    int x1 = m_random.nextInt(m_random.nextInt(m_random.nextInt(40) + 1) + 1);

    if (m_random.nextInt(15) != 0)
        x1 = 0;

    for (int i = 0; i < x1; i++)
    {
        real rx = 16 * ax + m_random.nextInt(16);
        real ry = m_random.nextInt(m_random.nextInt(120) + 8);
        real rz = 16 * az + m_random.nextInt(16);

        int nTunnels = 1;
        if (m_random.nextInt(4) == 0)
        {
            addRoom(x, z, tiles, rx, ry, rz);
            nTunnels += m_random.nextInt(4);
        }

        for (int j = 0; j < nTunnels; j++)
        {
            float ang1 = 2.0f * float(M_PI) * m_random.nextFloat();
            float x2 = (2.0f * (m_random.nextFloat() - 0.5f)) / 8.0f;
            float step = (2.0f * m_random.nextFloat()) + m_random.nextFloat();
            addTunnel(x, z, tiles, rx, ry, rz, step, ang1, x2, 0, 0, 1.0f);
        }
    }
}

void LargeCaveFeature::addRoom(int x, int z, TileID* tiles, real rx, real ry, real rz)
{
    addTunnel(x, z, tiles, rx, ry, rz, 1.0f + 6.0f * m_random.nextFloat(), 0.0f, 0.0f, -1, -1, 0.5);
}

void LargeCaveFeature::addTunnel(int x, int z, TileID* tiles, real rx, real ry, real rz, float x1, float ang, float x2, int step, int dist, real x5)
{
    real xMid = real(x * 16 + 8);
    real zMid = real(z * 16 + 8);

    float yRota = 0.0;
    float xRota = 0.0;
    Random random(m_random.nextLong());

    if (dist <= 0)
    {
        int max = m_radius * 16 - 16;
        dist = max - random.nextInt(max / 4);
    }

    bool singleStep = false;
    if (step == -1)
    {
        step = dist / 2;
        singleStep = true;
    }

    int splitPoint = random.nextInt(dist / 2) + dist / 4;
    bool steep = random.nextInt(6) == 0;
    for (; step < dist; step++)
    {
        real rad = Mth::sin((step * M_PI) / (float)dist) * x1 + 1.5;
        real yRad = rad * x5;
        float cos_pitch = Mth::cos(x2);
        rx += (Mth::cos(ang) * cos_pitch);
        ry += sin(x2);
        rz = rz + (Mth::sin(ang) * cos_pitch);
        if (steep)
            x2 *= 0.92f;
        else
            x2 *= 0.7f;

        x2 += xRota * 0.1f;
        ang += yRota * 0.1f;
        xRota *= 0.9f;
        yRota *= 12.0f / 16.0f;
        float v18 = random.nextFloat() - random.nextFloat();
        float v19 = random.nextFloat();
        xRota += 2.0f * v18 * v19;
        float v21 = random.nextFloat() - random.nextFloat();
        float v22 = random.nextFloat();
        yRota += (v21 * v22) * 4.0f;
        if (!singleStep && step == splitPoint && x1 > 1.0f)
        {
            addTunnel(
                x,
                z,
                tiles,
                rx,
                ry,
                rz,
                (random.nextFloat() * 0.5f) + 0.5f,
                (float(M_PI) / -2.0f) + ang,
                x2 / 3.0f,
                step,
                dist,
                1.0f);
            addTunnel(
                x,
                z,
                tiles,
                rx,
                ry,
                rz,
                (random.nextFloat() * 0.5f) + 0.5f,
                (float(M_PI) / 2.0f) + ang,
                x2 / 3.0f,
                step,
                dist,
                1.0f);
            return;
        }
        if (singleStep || random.nextInt(4) != 0)
        {
            real v53 = (dist - step);
            real v52 = (x1 + 2.0f) + 16.0f;
            if (((((rx - xMid) * (rx - xMid))
                + ((rz - zMid) * (rz - zMid)))
                - (v53 * v53)) > (v52 * v52))
                return;
            if (((xMid - 16.0f) + (rad * -2.0f)) <= rx &&
                ((zMid - 16.0f) + (rad * -2.0f)) <= rz &&
                ((xMid + 16.0f) + (rad * +2.0f)) >= rx &&
                ((zMid + 16.0f) + (rad * +2.0f)) >= rz)
            {
                int v51 = -16 * x + Mth::floor(rx - rad) - 1;
                int v50 = -16 * x + Mth::floor(rx + rad) + 1;
                int v49 = Mth::floor(ry - yRad) - 1;
                int v48 = Mth::floor(ry + yRad) + 1;
                int v47 = -16 * z + Mth::floor(rz - rad) - 1;
                int v46 = -16 * z + Mth::floor(rz + rad) + 1;
                if (v51 < 0)   v51 = 0;
                if (v50 > 16)  v50 = 16;
                if (v49 <= 0)  v49 = 1;
                if (v48 > 120) v48 = 120;
                if (v47 < 0)   v47 = 0;
                if (v46 > 16)  v46 = 16;
                bool v67 = false;
                for (int i = v51; v67 != 1 && i < v50; ++i)
                {
                    for (int j = v47; v67 != 1 && j < v46; ++j)
                    {
                        for (int k = v48 + 1; v67 != 1 && v49 - 1 <= k; --k)
                        {
                            int v42 = k + ((j + 16 * i) << 7);
                            if (k >= 0 && k <= 127)
                            {
                                if (tiles[v42] == Tile::water->m_ID || tiles[v42] == Tile::calmWater->m_ID)
                                    v67 = 1;
                                if (v49 - 1 != k && i != v51 && v50 - 1 != i && j != v47 && v46 - 1 != j)
                                    k = v49;
                            }
                        }
                    }
                }
                if (!v67)
                {
                    for (int l = v51; l < v50; ++l)
                    {
                        real v40 = ((real(l + 16 * x) + 0.5) - rx) / rad;
                        for (int m = v47; m < v46; ++m)
                        {
                            real v38 = ((real(m + 16 * z) + 0.5f) - rz) / rad;
                            int v37 = v48 + ((m + 16 * l) << 7);
                            bool v66 = 0;
                            if ((v40 * v40) + (v38 * v38) < 1.0f)
                            {
                                for (int n = v48 - 1; n >= v49; --n)
                                {
                                    real v35 = ((real(n) + 0.5) - ry) / yRad;
                                    if (v35 > -0.7f
                                        && v40 * v40 + v35 * v35 + v38 * v38 < 1.0f)
                                    {
                                        TileID v34 = tiles[v37];
                                        if (Tile::grass->m_ID == v34)
                                            v66 = 1;
                                        if (Tile::stone->m_ID == v34 ||
                                            Tile::dirt->m_ID == v34 ||
                                            Tile::grass->m_ID == v34)
                                        {
                                            if (n > 9)
                                            {
                                                tiles[v37] = 0;
                                                if (v66)
                                                {
                                                    if (tiles[v37 - 1] == Tile::dirt->m_ID)
                                                        tiles[v37 - 1] = Tile::grass->m_ID;
                                                }
                                            }
                                            else
                                            {
                                                tiles[v37] = Tile::lava->m_ID;
                                            }
                                        }
                                    }
                                    --v37;
                                }
                            }
                        }
                    }
                    if (singleStep)
                        return;
                }
            }
        }
    }
}
