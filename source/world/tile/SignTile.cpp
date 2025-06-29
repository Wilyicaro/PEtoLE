#include "SignTile.hpp"
#include "world/level/Level.hpp"

SignTile::SignTile(int id, bool isWall) : Tile(id,  Material::wood)
{
	m_bIsWall = isWall;
	setShape(0.25F, 0.0F, 0.25F, 0.75F, 1.0F, 0.75F);
}

bool SignTile::mayPlace(const Level* level, const TilePos& pos) const
{
	return level->isSolidTile(pos.west()) ? true : (level->isSolidTile(pos.east()) ? true : (level->isSolidTile(pos.north()) ? true : (level->isSolidTile(pos.south()) ? true : level->isSolidTile(pos.below()))));
}

void SignTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
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

bool SignTile::isSolidRender() const
{
	return false;
}

bool SignTile::isCubeShaped() const
{
	return false;
}

AABB* SignTile::getAABB(const Level* level, const TilePos& pos)
{
	return nullptr;
}

int SignTile::getRenderShape() const
{
	return -1;
}

bool SignTile::checkIfAttachedToBlock(Level* level, const TilePos& pos) {
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

void SignTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	if (!m_bIsWall) return;

	int var5 = level->getData(pos);
	const constexpr float var6 = 9.0F / 32.0F;
	const constexpr float var7 = 25.0F / 32.0F;
	const constexpr float var8 = 0.0F;
	const constexpr float var9 = 1.0F;
	const constexpr float var10 = 2.0F / 16.0F;
	setShape(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	if (var5 == 2) {
		setShape(var8, var6, 1.0F - var10, var9, var7, 1.0F);
	}

	if (var5 == 3) {
		setShape(var8, var6, 0.0F, var9, var7, var10);
	}

	if (var5 == 4) {
		setShape(1.0F - var10, var6, var8, 1.0F, var7, var9);
	}

	if (var5 == 5) {
		setShape(0.0F, var6, var8, var10, var7, var9);
	}

}

void SignTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob, Facing::Name face)
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