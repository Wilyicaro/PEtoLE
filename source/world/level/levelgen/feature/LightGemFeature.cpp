#include "Feature.hpp"
#include "world/level/Level.hpp"

bool LightGemFeature::place(Level* level, Random* random, const TilePos& pos)
{
	if (!level->isEmptyTile(pos))
		return false;

	if (level->getTile(pos.above()) != Tile::netherrack->m_ID)
		return false;

	level->setTile(pos, Tile::glowstone->m_ID);

	TilePos actual;
	for(int var6 = 0; var6 < 1500; ++var6) 
	{
		actual.x = pos.x + random->nextInt(8) - random->nextInt(8);
		actual.y = pos.y - random->nextInt(12);
		actual.z = pos.z + random->nextInt(8) - random->nextInt(8);
		if(level->isEmptyTile(actual))
		{
            int relCount = 0;

            for (Facing::Name face : Facing::ALL) {
                if (level->getTile(actual.relative(face)) == Tile::glowstone->m_ID)
                    ++relCount;
            }

            if (relCount == 1)
                level->setTile(actual, Tile::glowstone->m_ID);
		}
	}

	return true;
}
