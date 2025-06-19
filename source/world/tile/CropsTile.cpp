#include "CropsTile.hpp"
#include "world/level/Level.hpp"

CropsTile::CropsTile(int id, int texture) : Bush(id, texture)
{
	setTicking(true);
}

bool CropsTile::isSolidRender() const
{
	return false;
}

bool CropsTile::isCubeShaped() const
{
	return false;
}

int CropsTile::getRenderShape() const
{	
	return SHAPE_CROPS;
}

void CropsTile::tick(Level* level, const TilePos& pos, Random* random)
{
	Bush::tick(level, pos, random);
	if (level->getRawBrightness(pos.above()) >= 9) {
		int var6 = level->getData(pos);
		if (var6 < 7) {
			float var7 = getGrowthRate(level, pos);
			if (random->nextInt((int)(100.0F / var7)) == 0) {
				++var6;
				level->setData(pos, var6);
			}
		}
	}
}

float CropsTile::getGrowthRate(Level* level, const TilePos& pos) {
	float rate = 1.0F;
	TileID north = level->getTile(pos.north());
	TileID south = level->getTile(pos.south());
	TileID west = level->getTile(pos.west());
	TileID east = level->getTile(pos.east());
	TileID nw = level->getTile(pos.offset(-1, 0, -1));
	TileID ne = level->getTile(pos.offset(1, 0, -1));
	TileID se = level->getTile(pos.offset(1, 0, 1));
	TileID sw = level->getTile(pos.offset(-1, 0, 1));
	bool hor = west == m_ID || east == m_ID;
	bool vert = north == m_ID || south == m_ID;
	bool diag = nw == m_ID || ne == m_ID || se == m_ID || sw == m_ID;

	TilePos tp = pos.below();
	for (tp.x = pos.x; tp.x <= pos.x + 1; ++tp.x) {
		for (tp.z = pos.z; tp.z <= pos.z + 1; ++tp.z) {
			TileID var19 = level->getTile(tp);
			float var20 = 0.0F;
			if (var19 == Tile::farmland->m_ID) {
				var20 = 1.0F;
				if (level->getData(tp) > 0) {
					var20 = 3.0F;
				}
			}

			if (tp.x != pos.x || tp.z!= pos.z) {
				var20 /= 4.0F;
			}

			rate += var20;
		}
	}

	if (diag || hor && vert) {
		rate /= 2.0F;
	}

	return rate;
}

void CropsTile::fertilize(Level* level, const TilePos& pos) {
	level->setData(pos, 7);
}

int CropsTile::getResource(int x, Random* random) const
{
	return x == 7 ? Item::wheat->m_itemID : -1;
}

void CropsTile::spawnResources(Level* level, const TilePos& pos, int data, float chance)
{
	Bush::spawnResources(level, pos, data, chance);
	if (!level->m_bIsOnline) {
		for (int i = 0; i < 3; i++)
		{
			if (level->m_random.nextInt(15) > data)
				continue;


			Vec3 o((level->m_random.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f,
				(level->m_random.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f,
				(level->m_random.nextFloat() * 0.7f) + (1.0f - 0.7f) * 0.5f);

			ItemInstance inst(Item::seeds, 1, getSpawnResourcesAuxValue(data));
			auto pEntity = std::make_shared<ItemEntity>(level, o + pos, &inst);
			pEntity->m_throwTime = 10;

			level->addEntity(pEntity);
		}
	}
}


void CropsTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	setShape(0, 0, 0, 1, 0.25f, 1);
}


int CropsTile::getTexture(Facing::Name face, int data) const
{
	if (data < 0) {
		data = 7;
	}

	return m_TextureFrame + data;

}
