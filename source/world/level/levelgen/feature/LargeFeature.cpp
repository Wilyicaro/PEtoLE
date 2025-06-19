#include "LargeFeature.hpp"

LargeFeature::LargeFeature()
{
	m_radius = 8;
}

LargeFeature::~LargeFeature()
{
}

void LargeFeature::apply(ChunkSource* csrc, Level* level, int x, int z, uint8_t* tiles, int unk)
{
	m_random.setSeed(level->getSeed());

	int64_t x1 = 2 * (m_random.nextLong() / 2) + 1;
	int64_t x2 = 2 * (m_random.nextLong() / 2) + 1;

	for (int ax = x - m_radius; ax <= x + m_radius; ax++)
	{
		for (int az = z - m_radius; az <= z + m_radius; az++)
		{
			m_random.setSeed(((int64_t(ax) * x1) + (int64_t(az) * x2)) ^ level->getSeed());
			addFeature(level, ax, az, x, z, tiles, unk);
		}
	}
}
