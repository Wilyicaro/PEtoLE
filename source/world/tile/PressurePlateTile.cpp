#include "PressurePlateTile.hpp"
#include "world/level/Level.hpp"

PressurePlateTile::PressurePlateTile(int id, int texture, Sensitivity sensitivity) : Tile(id, texture, Material::decoration), m_sensitivity(sensitivity)
{
	setTicking(true);
	setShape(0.0625F, 0.0F, 0.0625F, 0.9375F, 0.03125F, 0.9375F);
}

AABB* PressurePlateTile::getAABB(const Level*, const TilePos& pos)
{
	return nullptr;
}

bool PressurePlateTile::mayPlace(const Level* level, const TilePos& pos) const
{
	return level->isNormalTile(pos.below());
}

void PressurePlateTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	bool var6 = false;
	if (!level->isNormalTile(pos.below())) {
		var6 = true;
	}

	if (var6) {
		spawnResources(level, pos, level->getData(pos));
		level->setTile(pos, TILE_AIR);
	}
}

int PressurePlateTile::getTickDelay() const
{
	return 20;
}

bool PressurePlateTile::isSolidRender() const
{
	return false;
}

bool PressurePlateTile::isCubeShaped() const
{
	return false;
}

void PressurePlateTile::checkPressed(Level* level, const TilePos& pos) const
{
	bool var5 = level->getData(pos) == 1;
	bool var6 = false;
	float var7 = 0.125F;
	std::vector<std::shared_ptr<Entity>> var8;
	switch (m_sensitivity) {
	case Sensitivity::everything:
		var8 = level->getEntities(nullptr, AABB(pos.x + var7, pos.y, pos.z + var7, pos.x + 1 - var7, pos.y + 0.25, pos.z + 1 - var7));
		break;
	case Sensitivity::mobs:
		var8 = level->getEntitiesOfCategory(EntityCategories::MOB, AABB(pos.x + var7, pos.y, pos.z + var7, pos.x + 1 - var7, pos.y + 0.25, pos.z + 1 - var7));
		break;
	case Sensitivity::players:
		var8 = level->getPlayers(AABB(pos.x + var7, pos.y, pos.z + var7, pos.x + 1 - var7, pos.y + 0.25, pos.z + 1 - var7));
		break;
	}

	if (var8.size() > 0) {
		var6 = true;
	}

	if (var6 && !var5) {
		level->setData(pos, 1);
		level->updateNeighborsAt(pos, m_ID);
		level->updateNeighborsAt(pos.below(), m_ID);
		level->setTilesDirty(pos, pos);
		level->playSound(pos.center(), "random.click", 0.3F, 0.6F);
	}

	if (!var6 && var5) {
		level->setData(pos, 0);
		level->updateNeighborsAt(pos, m_ID);
		level->updateNeighborsAt(pos.below(), m_ID);
		level->setTilesDirty(pos, pos);
		level->playSound(Vec3(pos.x + 0.5, pos.y + 0.1, pos.z + 0.5), "random.click", 0.3F, 0.5F);
	}

	if (var6) {
		level->addToTickNextTick(pos, m_ID, getTickDelay());
	}
}

void PressurePlateTile::tick(Level* level, const TilePos& pos, Random*)
{
	if (!level->m_bIsOnline && level->getData(pos) != 0)
		checkPressed(level, pos);
}

void PressurePlateTile::entityInside(Level* level, const TilePos& pos, Entity*) const
{
	if (!level->m_bIsOnline && level->getData(pos) != 1)
		checkPressed(level, pos);
}

void PressurePlateTile::onPlace(Level* pLevel, const TilePos& pos)
{
}

void PressurePlateTile::onRemove(Level* pLevel, const TilePos& pos)
{
	if (pLevel->getData(pos) > 0) {
		pLevel->updateNeighborsAt(pos, m_ID);
		pLevel->updateNeighborsAt(pos.below(), m_ID);
	}

	Tile::onRemove(pLevel, pos);
}

int PressurePlateTile::getSignal(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	return level->getData(pos) > 0;
}

int PressurePlateTile::getDirectSignal(const Level* level, const TilePos& pos, Facing::Name face) const
{
	if (level->getData(pos) == 0)
		return false;
	else
		return face == Facing::UP;
}

bool PressurePlateTile::isSignalSource() const
{
	return true;
}

void PressurePlateTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	bool var5 = level->getData(pos) == 1;
	const constexpr float var6 = 0.0625F;
	if (var5)
		setShape(var6, 0.0F, var6, 1.0F - var6, 0.03125F, 1.0F - var6);
	else
		setShape(var6, 0.0F, var6, 1.0F - var6, 0.0625F, 1.0F - var6);
}

void PressurePlateTile::updateDefaultShape()
{
	setShape(0.0F, 0.375F, 0.0F, 1.0F, 0.625F, 1.0F);
}