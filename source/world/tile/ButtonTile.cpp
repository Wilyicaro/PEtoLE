#include "ButtonTile.hpp"
#include "world/level/Level.hpp"

ButtonTile::ButtonTile(int id, int texture) : Tile(id, texture, Material::decoration)
{
	setTicking(true);
}

AABB* ButtonTile::getAABB(const Level*, const TilePos& pos)
{
	return nullptr;
}

bool ButtonTile::mayPlace(const Level* level, const TilePos& pos, Facing::Name face) const
{
	return level->isNormalTile(pos.west()) && face == Facing::EAST || level->isNormalTile(pos.east()) && face == Facing::WEST || level->isNormalTile(pos.north()) && face == Facing::SOUTH || level->isNormalTile(pos.south()) && face == Facing::NORTH;
}

bool ButtonTile::mayPlace(const Level* level, const TilePos& pos) const
{
	return level->isNormalTile(pos.west()) || level->isNormalTile(pos.east()) || level->isNormalTile(pos.north()) || level->isNormalTile(pos.south());
}

void ButtonTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	if (checkCanSurvive(level, pos)) {
		int var6 = level->getData(pos) & 7;
		bool var7 = false;
		if (!level->isNormalTile(pos.west()) && var6 == 1) {
			var7 = true;
		}

		if (!level->isNormalTile(pos.east()) && var6 == 2) {
			var7 = true;
		}

		if (!level->isNormalTile(pos.north()) && var6 == 3) {
			var7 = true;
		}

		if (!level->isNormalTile(pos.south()) && var6 == 4) {
			var7 = true;
		}

		if (var7) {
			spawnResources(level, pos, level->getData(pos));
			level->setTile(pos, 0);
		}
	}
}

void ButtonTile::attack(Level* level, const TilePos& pos, Player* player)
{
	if (!player->isSneaking())
		use(level, pos, player);
}

bool ButtonTile::use(Level* level, const TilePos& pos, Player* player)
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

		level->addToTickNextTick(pos, m_ID, getTickDelay());

		return true;
	}
}


int ButtonTile::getTickDelay() const
{
	return 20;
}

bool ButtonTile::isSolidRender() const
{
	return false;
}

bool ButtonTile::isCubeShaped() const
{
	return false;
}

bool ButtonTile::checkCanSurvive(Level* level, const TilePos& pos) {
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

void ButtonTile::tick(Level* level, const TilePos& pos, Random*)
{
	if (!level->m_bIsOnline) {
		int var6 = level->getData(pos);
		if ((var6 & 8) != 0) {
			level->setData(pos, var6 & 7);
			level->updateNeighborsAt(pos, m_ID);
			int var7 = var6 & 7;
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

			level->playSound(pos.center(), "random.click", 0.3F, 0.5F);
			level->setTilesDirty(pos, pos);
		}
	}
}

void ButtonTile::onPlace(Level* pLevel, const TilePos& pos)
{
	if (pLevel->isNormalTile(pos.west())) {
		pLevel->setData(pos, 1);
	}
	else if (pLevel->isNormalTile(pos.east())) {
		pLevel->setData(pos, 2);
	}
	else if (pLevel->isNormalTile(pos.north())) {
		pLevel->setData(pos, 3);
	}
	else if (pLevel->isNormalTile(pos.south())) {
		pLevel->setData(pos, 4);
	}

	checkCanSurvive(pLevel, pos);
}

void ButtonTile::onRemove(Level* pLevel, const TilePos& pos)
{
	int var5 = pLevel->getData(pos);
	if ((var5 & 8) > 0) {
		pLevel->updateNeighborsAt(pos, m_ID);
		int var6 = var5 & 7;
		if (var6 == 1) {
			pLevel->updateNeighborsAt(pos.west(), m_ID);
		}
		else if (var6 == 2) {
			pLevel->updateNeighborsAt(pos.east(), m_ID);
		}
		else if (var6 == 3) {
			pLevel->updateNeighborsAt(pos.north(), m_ID);
		}
		else if (var6 == 4) {
			pLevel->updateNeighborsAt(pos.south(), m_ID);
		}
		else {
			pLevel->updateNeighborsAt(pos.below(), m_ID);
		}
	}

	Tile::onRemove(pLevel, pos);
}

int ButtonTile::getSignal(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	return (level->getData(pos) & 8) > 0;
}

int ButtonTile::getDirectSignal(const Level* level, const TilePos& pos, Facing::Name face) const
{
	int var6 = level->getData(pos);
	if ((var6 & 8) == 0) {
		return false;
	}
	else {
		int var7 = var6 & 7;
		if (var7 == 5 && face == Facing::UP) {
			return true;
		}
		else if (var7 == 4 && face == Facing::NORTH) {
			return true;
		}
		else if (var7 == 3 && face == Facing::SOUTH) {
			return true;
		}
		else if (var7 == 2 && face == Facing::WEST) {
			return true;
		}
		else {
			return var7 == 1 && face == Facing::EAST;
		}
	}
}

bool ButtonTile::isSignalSource() const
{
	return true;
}

void ButtonTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	int var5 = level->getData(pos);
	int var6 = var5 & 7;
	bool var7 = (var5 & 8) > 0;
	const constexpr float var8 = 0.375F;
	const constexpr float var9 = 0.625F;
	const constexpr float var10 = 0.1875F;
	float var11 = 0.125F;
	if (var7) {
		var11 = 0.0625F;
	}

	if (var6 == 1) {
		setShape(0.0F, var8, 0.5F - var10, var11, var9, 0.5F + var10);
	}
	else if (var6 == 2) {
		setShape(1.0F - var11, var8, 0.5F - var10, 1.0F, var9, 0.5F + var10);
	}
	else if (var6 == 3) {
		setShape(0.5F - var10, var8, 0.0F, 0.5F + var10, var9, var11);
	}
	else if (var6 == 4) {
		setShape(0.5F - var10, var8, 1.0F - var11, 0.5F + var10, var9, 1.0F);
	}


}

void ButtonTile::updateDefaultShape()
{
	const constexpr float var1 = 0.1875F;
	const constexpr float var2 = 0.125F;
	const constexpr float var3 = 0.125F;
	setShape(0.5F - var1, 0.5F - var2, 0.5F - var3, 0.5F + var1, 0.5F + var2, 0.5F + var3);
}

void ButtonTile::setPlacedOnFace(Level* level, const TilePos& pos, Facing::Name face)
{
	int var6 = level->getData(pos);
	int var7 = var6 & 8;
	var6 &= 7;
	if (face == Facing::NORTH && level->isNormalTile(pos.south()))
	{
		var6 = 4;
	}
	else if (face == Facing::SOUTH && level->isNormalTile(pos.north()))
	{
		var6 = 3;
	}
	else if (face == Facing::WEST && level->isNormalTile(pos.east()))
	{
		var6 = 2;
	}
	else if (face == Facing::EAST && level->isNormalTile(pos.west())) {
		var6 = 1;
	}
	else var6 = level->isNormalTile(pos.west()) ? 1 : (level->isNormalTile(pos.east()) ? 2 : (level->isNormalTile(pos.north()) ? 3 : (level->isNormalTile(pos.south()) ? 4 : 1)));

	level->setData(pos, var6 + var7);
}


