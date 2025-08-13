#include "RepeaterTile.hpp"
#include "world/level/Level.hpp"

RepeaterTile::RepeaterTile(int id, bool powered) : Tile(id, TEXTURE_STONE_SLAB_TOP, Material::decoration), m_bIsPowered(powered)
{
	setShape(0.0F, 0.0F, 0.0F, 1.0F, 2.0F / 16.0F, 1.0F);
}

bool RepeaterTile::mayPlace(const Level* level, const TilePos& pos) const
{
	return level->isSolidTile(pos.below()) && Tile::mayPlace(level, pos);
}

bool RepeaterTile::canSurvive(const Level* level, const TilePos& pos) const
{
	return level->isSolidTile(pos.below()) && Tile::canSurvive(level, pos);
}

void RepeaterTile::tick(Level* level, const TilePos& pos, Random* random)
{
	int var6 = level->getData(pos);
	bool var7 = canPower(level, pos, var6);
	if (m_bIsPowered && !var7)
		level->setTileAndData(pos, Tile::repeater->m_ID, var6);
	else if (!m_bIsPowered)
	{
		level->setTileAndData(pos, Tile::repeaterLit->m_ID, var6);
		if (!var7) {
			int var8 = (var6 & 12) >> 2;
			level->addToTickNextTick(pos, Tile::repeaterLit->m_ID, repeaterFaceTicks[var8] * 2);
		}
	}
}

void RepeaterTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	if (!canSurvive(level, pos))
	{
		spawnResources(level, pos, level->getData(pos));
		level->setTile(pos, TILE_AIR);
	}
	else
	{
		int data = level->getData(pos);
		bool power = canPower(level, pos, data);
		int faceData = (data & 12) >> 2;
		if ((m_bIsPowered && !power) || (!m_bIsPowered && power))
			level->addToTickNextTick(pos, m_ID, repeaterFaceTicks[faceData] * 2);
	}
}

bool RepeaterTile::isSolidRender() const
{
	return false;
}

bool RepeaterTile::isCubeShaped() const
{
	return false;
}

int RepeaterTile::getTexture(Facing::Name face, int data) const
{
	return face == Facing::DOWN ? (m_bIsPowered ? 99 : 115) : (face == Facing::UP ? (m_bIsPowered ? 147 : 131) : 5);
}

int RepeaterTile::getTexture(Facing::Name face) const
{
	return getTexture(face, 0);
}

bool RepeaterTile::shouldRenderFace(const LevelSource*, const TilePos& pos, Facing::Name face) const
{
	return Facing::isHorizontal(face);
}

int RepeaterTile::getRenderShape() const
{
	return SHAPE_REPEATER;
}

void RepeaterTile::onPlace(Level* pLevel, const TilePos& pos)
{
	for (Facing::Name face : Facing::ALL)
		pLevel->neighborChanged(pos.relative(face), m_ID);
}

int RepeaterTile::getSignal(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	if (!m_bIsPowered)
		return false;
	else
	{
		int faceData = level->getData(pos) & 3;
		return faceData == 0 && face == Facing::SOUTH ? true : (faceData == 1 && face == Facing::WEST ? true : (faceData == 2 && face == Facing::NORTH ? true : faceData == 3 && face == Facing::EAST));
	}
}

int RepeaterTile::getDirectSignal(const Level* level, const TilePos& pos, Facing::Name face) const
{
	return getSignal(level, pos, face);
}

bool RepeaterTile::isSignalSource() const
{
	return false;
}

int RepeaterTile::getResource(int, Random*) const
{
	return Item::diode->m_itemID;
}

void RepeaterTile::animateTick(Level* level, const TilePos& pos, Random* random)
{
	if (m_bIsPowered)
	{
		int data = level->getData(pos);
		Vec3 pPos;
		pPos.x = pos.x + 0.5F + (random->nextFloat() - 0.5F) * 0.2;
		pPos.y = pos.y + 0.4F + (random->nextFloat() - 0.5F) * 0.2;
		pPos.z = pos.z + 0.5F + (random->nextFloat() - 0.5F) * 0.2;
		if (random->nextInt(2) == 0) {
			switch (data & 3) {
			case 0:
				pPos.z -= 0.3125;
				break;
			case 1:
				pPos.x += 0.3125;
				break;
			case 2:
				pPos.z += 0.3125;
				break;
			case 3:
				pPos.x -= 0.3125;
			}
		}
		else {
			int faceData = (data & 12) >> 2;
			switch (data & 3) {
			case 0:
				pPos.z += repeaterFacing[faceData];
				break;
			case 1:
				pPos.x -= repeaterFacing[faceData];
				break;
			case 2:
				pPos.z -= repeaterFacing[faceData];
				break;
			case 3:
				pPos.x += repeaterFacing[faceData];
			}
		}

		level->addParticle("reddust", pPos);
	}
}

bool RepeaterTile::canPower(Level* level, const TilePos& pos, int data)
{
	int var6 = data & 3;
	switch (var6)
	{
	case 0:
		return level->getSignal(pos.south(), Facing::SOUTH) || level->getTile(pos.south()) == Tile::redstoneDust->m_ID && level->getData(pos.south()) > 0;
	case 1:
		return level->getSignal(pos.west(), Facing::WEST) || level->getTile(pos.west()) == Tile::redstoneDust->m_ID && level->getData(pos.west()) > 0;
	case 2:
		return level->getSignal(pos.north(), Facing::NORTH) || level->getTile(pos.north()) == Tile::redstoneDust->m_ID && level->getData(pos.north()) > 0;
	case 3:
		return level->getSignal(pos.east(), Facing::EAST) || level->getTile(pos.east()) == Tile::redstoneDust->m_ID && level->getData(pos.east()) > 0;
	default:
		return false;
	}
}

bool RepeaterTile::use(Level* level, const TilePos& pos, Player* player)
{
	int var6 = level->getData(pos);
	int var7 = (var6 & 12) >> 2;
	var7 = var7 + 1 << 2 & 12;
	level->setData(pos, var7 | var6 & 3);
	return true;
}

void RepeaterTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob, Facing::Name face)
{
	int rot = ((Mth::floor(0.5f + (mob->m_rot.y * 4.0f / 360.0f)) & 3) + 2) % 4;
	level->setData(pos, rot);
	if (canPower(level, pos, rot))
		level->addToTickNextTick(pos, m_ID, 1);
}
