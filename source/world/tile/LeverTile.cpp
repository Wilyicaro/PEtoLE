#include "LeverTile.hpp"
#include "world/level/Level.hpp"

LeverTile::LeverTile(int id, int texture) : Tile(id, texture, Material::decoration)
{
}

bool LeverTile::mayPlace(const Level* level, const TilePos& pos) const
{
	return level->isSolidTile(pos.west()) ? true : (level->isSolidTile(pos.east()) ? true : (level->isSolidTile(pos.north()) ? true : (level->isSolidTile(pos.south()) ? true : level->isSolidTile(pos.below()))));
}

AABB* LeverTile::getAABB(const Level*, const TilePos& pos)
{
	return nullptr;
}

void LeverTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	if (checkIfAttachedToBlock(level, pos)) {
		int var6 = level->getData(pos) & 7;
		bool var7 = false;
		if (!level->isSolidTile(pos.west()) && var6 == 1) {
			var7 = true;
		}

		if (!level->isSolidTile(pos.east()) && var6 == 2) {
			var7 = true;
		}

		if (!level->isSolidTile(pos.north()) && var6 == 3) {
			var7 = true;
		}

		if (!level->isSolidTile(pos.south()) && var6 == 4) {
			var7 = true;
		}

		if (!level->isSolidTile(pos.below()) && (var6 == 5 || var6 == 6)) {
			var7 = true;
		}

		if (var7) {
			spawnResources(level, pos, level->getData(pos));
			level->setTile(pos, 0);
		}
	}
}

bool LeverTile::use(Level* level, const TilePos& pos, Player* player)
{
	if (player->isSneaking() && player->getSelectedItem()) {
		return false;
	}
	else if (level->m_bIsOnline) {
		return true;
	}
	else {
		int var6 = level->getData(pos);
		int var7 = var6 & 7;
		int var8 = 8 - (var6 & 8);
		level->setData(pos, var7 + var8);
		level->setTilesDirty(pos, pos);
		level->playSound(Vec3(pos.x + 0.5, pos.y + 0.5, pos.z + 0.5), "random.click", 0.3F, var8 > 0 ? 0.6F : 0.5F);
		level->updateNeighborsAt(pos, m_ID);
		if (var7 == 1) {
			level->updateNeighborsAt(pos.west(), m_ID);
		}
		else if (var7 == 2) {
			level->updateNeighborsAt(pos.east(), m_ID);
		}
		else if (var7 == 3) {
			level->updateNeighborsAt(pos.north(), m_ID);
		}
		else if (var7 == 4) {
			level->updateNeighborsAt(pos.south(), m_ID);
		}
		else {
			level->updateNeighborsAt(pos.below(), m_ID);
		}

		return true;
	}
}

void LeverTile::attack(Level* level, const TilePos& pos, Player* player)
{
	if (!player->isSneaking())
		use(level, pos, player);
}


bool LeverTile::isSolidRender() const
{
	return false;
}

bool LeverTile::isCubeShaped() const
{
	return false;
}

int LeverTile::getRenderShape() const
{
	return SHAPE_LEVER;
}

bool LeverTile::checkIfAttachedToBlock(Level* level, const TilePos& pos) {
	if (!mayPlace(level, pos)) 
	{
		spawnResources(level, pos, level->getData(pos));
		level->setTile(pos, 0);
		return false;
	}
	else 
	{
		return true;
	}
}

void LeverTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	int var5 = level->getData(pos) & 7;
	float var6 = 3.0F / 16.0F;
	if (var5 == 1) {
		setShape(0.0F, 0.2F, 0.5F - var6, var6 * 2.0F, 0.8F, 0.5F + var6);
	}
	else if (var5 == 2) {
		setShape(1.0F - var6 * 2.0F, 0.2F, 0.5F - var6, 1.0F, 0.8F, 0.5F + var6);
	}
	else if (var5 == 3) {
		setShape(0.5F - var6, 0.2F, 0.0F, 0.5F + var6, 0.8F, var6 * 2.0F);
	}
	else if (var5 == 4) {
		setShape(0.5F - var6, 0.2F, 1.0F - var6 * 2.0F, 0.5F + var6, 0.8F, 1.0F);
	}
	else {
		var6 = 0.25F;
		setShape(0.5F - var6, 0.0F, 0.5F - var6, 0.5F + var6, 0.6F, 0.5F + var6);
	}

}

void LeverTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob, Facing::Name face)
{
	int var7 = level->getData(pos);
	int rot = Mth::floor(0.5f + (mob->m_rot.y * 4.0f / 360.0f)) & 3;
	if (level->isSolidTile(pos.west())) {
		var7 = 1;
	}
	else if (level->isSolidTile(pos.east())) {
		var7 = 2;
	}
	else if (level->isSolidTile(pos.north())) {
		var7 = 3;
	}
	else if (level->isSolidTile(pos.south())) {
		var7 = 4;
	}
	else if (level->isSolidTile(pos.below())) {
		if (face != Facing::DOWN && face != Facing::NORTH) {
			var7 = 6;
		}
		else {
			var7 = 5;
		}
	}

	if (face == Facing::UP && level->isSolidTile(pos.below())) {
		if (rot != 0 && rot != 2) {
			var7 = 6;
		}
		else {
			var7 = 5;
		}
	}

	if (face== Facing::NORTH && level->isSolidTile(pos.south())) {
		var7 = 4;
	}

	if (face == Facing::SOUTH && level->isSolidTile(pos.north())) {
		var7 = 3;
	}

	if (face == Facing::WEST && level->isSolidTile(pos.east())) {
		var7 = 2;
	}

	if (face == Facing::EAST && level->isSolidTile(pos.west())) {
		var7 = 1;
	}

	level->setData(pos, var7);
}

void LeverTile::onRemove(Level* level, const TilePos& pos)
{
	int var5 = level->getData(pos);
	if ((var5 & 8) > 0) {
		level->updateNeighborsAt(pos, m_ID);
		int var6 = var5 & 7;
		if (var6 == 1) {
			level->updateNeighborsAt(pos.west(), m_ID);
		}
		else if (var6 == 2) {
			level->updateNeighborsAt(pos.east(), m_ID);
		}
		else if (var6 == 3) {
			level->updateNeighborsAt(pos.north(), m_ID);
		}
		else if (var6 == 4) {
			level->updateNeighborsAt(pos.south(), m_ID);
		}
		else {
			level->updateNeighborsAt(pos.below(), m_ID);
		}
	}

	Tile::onRemove(level, pos);
}

int LeverTile::getDirectSignal(const Level* level, const TilePos& pos, Facing::Name face) const
{
	int var6 = level->getData(pos);
	if ((var6 & 8) == 0) {
		return false;
	}
	else {
		int var7 = var6 & 7;
		if (var7 == 5 && face == 1) {
			return true;
		}
		else if (var7 == 4 && face == 2) {
			return true;
		}
		else if (var7 == 3 && face == 3) {
			return true;
		}
		else if (var7 == 2 && face == 4) {
			return true;
		}
		else {
			return var7 == 1 && face == 5;
		}
	}
}
int LeverTile::getSignal(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	return (level->getData(pos) & 8) > 0;
}
bool LeverTile::isSignalSource() const
{
	return true;
}
