#include "TrapDoorTile.hpp"
#include "world/level/Level.hpp"
#include "world/item/Item.hpp"

TrapDoorTile::TrapDoorTile(int ID, Material* pMtl) : Tile(ID, pMtl)
{
	m_TextureFrame = TEXTURE_TRAPDOOR;

	if (pMtl == Material::metal)
		m_TextureFrame++;

	Tile::setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.1875f, 1.0f);
}

bool TrapDoorTile::use(Level* level, const TilePos& pos, Player* player)
{
	// well, you know, iron doors can't be opened by right clicking
	if (m_pMaterial == Material::metal)
		return true;

	int data = level->getData(pos);

	level->setData(pos, data ^ 4);

	level->levelEvent(player, LevelEvent::SOUND_OPEN_OR_CLOSE, pos, 0);

	return true;
}

void TrapDoorTile::attack(Level* level, const TilePos& pos, Player* player)
{
	if (!player->isSneaking())
		use(level, pos, player);
}

HitResult TrapDoorTile::clip(const Level* level, const TilePos& pos, Vec3 v1, Vec3 v2)
{
	return Tile::clip(level, pos, v1, v2);
}

AABB* TrapDoorTile::getAABB(const Level* level, const TilePos& pos)
{
	updateShape(level, pos);
	return Tile::getAABB(level, pos);
}

AABB TrapDoorTile::getTileAABB(const Level* level, const TilePos& pos)
{
	updateShape(level, pos);
	return Tile::getTileAABB(level, pos);
}

bool TrapDoorTile::isCubeShaped() const
{
	return false;
}

bool TrapDoorTile::isSolidRender() const
{
	return false;
}

bool TrapDoorTile::mayPlace(const Level* level, const TilePos& pos, Facing::Name face) const
{
	return Facing::isHorizontal(face) && level->isNormalTile(pos.relative(Facing::OPPOSITE[face]));
}

void TrapDoorTile::setPlacedOnFace(Level* level, const TilePos& pos, Facing::Name face)
{
	uint8_t data = 0;
	if (face == Facing::SOUTH)
		data = 0;

	if (face == Facing::NORTH)
		data = 1;

	if (face == Facing::WEST)
		data = 2;

	if (face == Facing::EAST)
		data = 3;

	level->setData(pos, data);
}

void TrapDoorTile::setShape(int data)
{
	Tile::setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.1875f, 1.0f);
	if (!isOpen(data)) return;
	switch (data & 3)
	{
		case 0:
			Tile::setShape(0.0f, 0.0f, 0.8125f, 1.0f, 1.0f, 1.0f);
			break;
		case 1:
			Tile::setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.1875f);
			break;
		case 2:
			Tile::setShape(0.8125f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
			break;
		case 3:
			Tile::setShape(0.0f, 0.0f, 0.0f, 0.1875f, 1.0f, 1.0f);
			break;
	}
}

void TrapDoorTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	setShape(level->getData(pos));
}

void TrapDoorTile::updateDefaultShape()
{
	const constexpr float var1 = 3.0F / 16.0F;
	Tile::setShape(0.0F, 0.5F - var1 / 2.0F, 0.0F, 1.0F, 0.5F + var1 / 2.0F, 1.0F);
}

void TrapDoorTile::setOpen(Level* level, const TilePos& pos, bool bOpen)
{
	int data = level->getData(pos);

	if (isOpen(level->getData(pos)) != bOpen)
	{
		level->setData(pos, data ^ 4);

		level->levelEvent(nullptr, LevelEvent::SOUND_OPEN_OR_CLOSE, pos, 0);
	}
}

void TrapDoorTile::neighborChanged(Level* level, const TilePos& pos, TileID newTile)
{
	if (level->m_bIsOnline) {
		int data = level->getData(pos);
		TilePos tp(pos);
		if ((data & 3) == 0) {
			--tp.z;
		}

		if ((data & 3) == 1) {
			--tp.z;
		}

		if ((data & 3) == 2) {
			++tp.x;
		}

		if ((data & 3) == 3) {
			--tp.x;
		}

		if (!level->isNormalTile(tp)) {
			level->setData(pos, 0);
			spawnResources(level, pos, data);
		}

		if (newTile > 0 && Tile::tiles[newTile]->isSignalSource())
			setOpen(level, pos, level->hasNeighborSignal(pos));
	}
}
