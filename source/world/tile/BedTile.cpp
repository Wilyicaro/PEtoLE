#include "BedTile.hpp"
#include "world/level/Level.hpp"

BedTile::BedTile(int a, int b) : Tile(a, b, Material::wood)
{
	updateDefaultShape();
}

int BedTile::getTexture(Facing::Name face, int data) const
{
	if (Facing::isHorizontal(face)) {
		int var3 = getDirectionFromData(data);
		int var4 = bedDirection[var3][face];
		return isHead(data) ? (var4 == 2 ? m_TextureFrame + 2 + 16 : (var4 != 5 && var4 != 4 ? m_TextureFrame + 1 : m_TextureFrame + 1 + 16)) : (var4 == 3 ? m_TextureFrame - 1 + 16 : (var4 != 5 && var4 != 4 ? m_TextureFrame : m_TextureFrame + 16));
	}
	else {
		return isHead(data) ? m_TextureFrame + 1 : m_TextureFrame;
	}
}

void BedTile::updateShape(const LevelSource* level, const TilePos& pos)
{
	updateDefaultShape();
}

void BedTile::updateDefaultShape()
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
	if (isHead(data)) {
		if (level->getTile(pos.offset(-headBlockToFootBlockMap[dir][0], 0, -headBlockToFootBlockMap[dir][1])) != m_ID) {
			level->setTile(pos, 0);
		}
	}
	else if (level->getTile(pos.offset(headBlockToFootBlockMap[dir][0], 0, headBlockToFootBlockMap[dir][1])) != m_ID) {
		level->setTile(pos, 0);
		if (!level->m_bIsOnline) {
			Tile::spawnResources(level, pos, data);
		}
	}
}

bool BedTile::use(Level* level, const TilePos& pos, Player* player)
{
	if (level->m_bIsOnline) {
		return true;
	}
	else {
		int var6 = level->getData(pos);
		TilePos tp(pos);
		if (!isHead(var6)) {
			int var7 = getDirectionFromData(var6);
			tp.x += headBlockToFootBlockMap[var7][0];
			tp.z += headBlockToFootBlockMap[var7][1];
			if (level->getTile(pos) != m_ID) {
				return true;
			}

			var6 = level->getData(tp);
		}

		if (!level->m_pDimension->mayRespawn()) {
			level->setTile(tp, 0);
			int data = getDirectionFromData(var6);
			tp.x += headBlockToFootBlockMap[data][0];
			tp.z += headBlockToFootBlockMap[data][1];
			if (level->getTile(tp) == m_ID)
			{
				level->setTile(tp, 0);
			}

			level->explode(nullptr, tp.center(), 5.0F, true);
			return true;
		}
		else {
			if (isBedOccupied(var6)) {
				std::shared_ptr<Player> var14 = nullptr;

				for (auto& p : level->m_players)
				{
					if (p->isSleeping()) {
						if (p->m_respawnPos == tp)
							var14 = p;
					}
				}

				if (var14) {
					var14->displayClientMessage("tile.bed.occupied");
					return true;
				}

				setBedOccupied(level, tp, false);
			}

			Player::BedSleepingProblem var15 = player->sleep(tp);
			if (var15 == Player::BedSleepingProblem::OK)
			{
				setBedOccupied(level, tp, true);
				return true;
			}
			else
			{
				if (var15 == Player::BedSleepingProblem::NOT_POSSIBLE_NOW)
					player->displayClientMessage("tile.bed.noSleep");

				return true;
			}
		}
	}
}

int BedTile::getResource(int data, Random* random) const
{
	return isHead(data) ? 0 : Item::bed->m_itemID;
}

void BedTile::spawnResources(Level* level, const TilePos& pos, int data, float chance)
{
	if (!isHead(data))
	{
		Tile::spawnResources(level, pos, data, chance);
	}
}

void BedTile::setBedOccupied(Level* level, const TilePos& pos, bool b)
{
	int data = level->getData(pos);
	if (b) {
		data |= 4;
	}
	else {
		data &= -5;
	}

	level->setData(pos, data);
}

TilePos BedTile::getRespawnTilePos(const Level* level, const TilePos& pos, int steps)
{
	int var5 = level->getData(pos);
	int var6 = getDirectionFromData(var5);

	for (int var7 = 0; var7 <= 1; ++var7) {
		int startX = pos.x - headBlockToFootBlockMap[var6][0] * var7 - 1;
		int startZ = pos.z - headBlockToFootBlockMap[var6][1] * var7 - 1;

		TilePos tp(pos);
		for (tp.x = startX; tp.x <= startX + 2; ++tp.x)
		{
			for (tp.z = startZ; tp.z <= startZ + 2; ++tp.z)
			{
				if (level->isSolidTile(tp.below()) && level->isEmptyTile(tp) && level->isEmptyTile(tp.above()))
				{
					if (steps <= 0)
						return tp;

					--steps;
				}
			}
		}
	}

	return pos;
}
