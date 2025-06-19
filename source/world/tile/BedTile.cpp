#include "BedTile.hpp"
#include "world/level/Level.hpp"

BedTile::BedTile(int a, int b) : Tile(a, b, Material::wood)
{
}

int BedTile::getTexture(Facing::Name face, int data) const
{
	if (data != 1 && data != 0) {
		int var3 = getDirectionFromData(data);
		int var4 = bedDirection[var3][data];
		return isBlockHeadOfBed(data) ? (var4 == 2 ? m_TextureFrame + 2 + 16 : (var4 != 5 && var4 != 4 ? m_TextureFrame + 1 : m_TextureFrame + 1 + 16)) : (var4 == 3 ? m_TextureFrame - 1 + 16 : (var4 != 5 && var4 != 4 ? m_TextureFrame : m_TextureFrame + 16));
	}
	else {
		return isBlockHeadOfBed(data) ? m_TextureFrame + 1 : m_TextureFrame;
	}
}

void BedTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	setShape(0, 0, 0, 1, 9 / 16.0f, 1);
}


bool BedTile::isSolidRender() const
{
	return false;
}

bool BedTile::isCubeShaped() const
{
	return false;
}

int BedTile::getRenderShape() const
{
	return SHAPE_BED;
}

void BedTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	int data = level->getData(pos);
	int dir = getDirectionFromData(data);
	if (isBlockHeadOfBed(data)) {
		if (level->getTile(pos.offset(-headBlockToFootBlockMap[dir][0], 0, -headBlockToFootBlockMap[dir][1])) != m_ID) {
			level->setTile(pos, 0);
		}
	}
	else if (level->getTile(pos.offset(headBlockToFootBlockMap[dir][0], 0, headBlockToFootBlockMap[dir][1])) != m_ID) {
		level->setTile(pos, 0);
		if (!level->m_bIsOnline) {
			spawnResources(level, pos, data);
		}
	}
}

//int BedTile::use(Level* level, const TilePos& pos, Player* player)
//{
//}

TilePos BedTile::getNearestEmptyChunkTilePos(const Level* level, const TilePos& pos, int steps)
{
	int var5 = level->getData(pos);
	int var6 = getDirectionFromData(var5);

	for (int var7 = 0; var7 <= 1; ++var7) {
		int var8 = pos.x - headBlockToFootBlockMap[var6][0] * var7 - 1;
		int var9 = pos.z - headBlockToFootBlockMap[var6][1] * var7 - 1;
		int var10 = var8 + 2;
		int var11 = var9 + 2;

		for (int var12 = var8; var12 <= var10; ++var12) {
			for (int var13 = var9; var13 <= var11; ++var13) {
				if (level->isSolidTile(pos.below()) && level->isEmptyTile(pos) && level->isEmptyTile(pos.above())) {
					if (steps <= 0) {
						return TilePos(var12, pos.y, var13);
					}

					--steps;
				}
			}
		}
	}

	return nullptr;
}
