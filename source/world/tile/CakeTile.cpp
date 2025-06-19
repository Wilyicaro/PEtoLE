#include "CakeTile.hpp"
#include "world/level/Level.hpp"

CakeTile::CakeTile(int id, int texture) : Tile(id, texture, Material::cake)
{
}

bool CakeTile::mayPlace(const Level* level, const TilePos& pos) const
{
	return Tile::mayPlace(level, pos) && canSurvive(level, pos);
}

void CakeTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	if (!canSurvive(level, pos)) {
		spawnResources(level, pos, level->getData(pos));
		level->setTile(pos, 0);
	}
}

void CakeTile::eat(Level* level, const TilePos& pos, Player* player)
{
	if (player->m_health < 20) {
		player->heal(3);
		int var6 = level->getData(pos) + 1;
		if (var6 >= 6) {
			level->setTile(pos, 0);
		}
		else {
			level->setData(pos, var6);
			level->setTilesDirty(pos, pos);
		}
	}
}

void CakeTile::attack(Level* level, const TilePos& pos, Player* player)
{
	if (!player->isSneaking())
		eat(level, pos, player);
}

bool CakeTile::use(Level* level, const TilePos& pos, Player* player)
{
	if (player->isSneaking() && player->getSelectedItem()) return false;
	eat(level, pos, player);
	return true;
}


bool CakeTile::isSolidRender() const
{
	return false;
}

int CakeTile::getTexture(Facing::Name face, int data) const
{
	if (face == Facing::UP) {
		return m_TextureFrame;
	}
	else if (face == Facing::DOWN) {
		return m_TextureFrame + 3;
	}
	else {
		return data > 0 && face == Facing::WEST ? m_TextureFrame + 2 : m_TextureFrame + 1;
	}
}

int CakeTile::getTexture(Facing::Name face) const
{
	if (face == Facing::UP) {
		return m_TextureFrame;
	}
	else {
		return face == Facing::DOWN ? m_TextureFrame + 3 : m_TextureFrame + 1;
	}
}

bool CakeTile::isCubeShaped() const
{
	return false;
}

void CakeTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	int var5 = level->getData(pos);
	float var6 = 0.0625F;
	float var7 = (float)(1 + var5 * 2) / 16.0F;
	float var8 = 0.5F;
	setShape(var7, 0.0F, var6, 1.0F - var6, var8, 1.0F - var6);
}

void CakeTile::updateDefaultShape()
{
	float var1 = 0.0625F;
	float var2 = 0.5F;
	setShape(var1, 0.0F, var1, 1.0F - var1, var2, 1.0F - var1);
}

bool CakeTile::canSurvive(const Level* level, const TilePos& pos) const
{
	return level->getMaterial(pos.below())->isSolid();
}
