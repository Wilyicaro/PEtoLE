#include "FurnaceTile.hpp"
#include "world/level/Level.hpp"
#include "entity/FurnaceTileEntity.hpp"

bool FurnaceTile::keepFurnaceInventory = false;

FurnaceTile::FurnaceTile(int id, bool active) : Tile(id, TEXTURE_FURNACE_SIDE, Material::stone)
{
	m_active = active;
}

int FurnaceTile::getTexture(const LevelSource* level, const TilePos& pos, Facing::Name face) const
{
	switch (face) {
	case Facing::UP: case Facing::DOWN: return m_TextureFrame + 17;
	default: 
		int meta = level->getData(pos);
		return face != meta ? m_TextureFrame : (m_active ? m_TextureFrame + 16 : m_TextureFrame - 1);
	}
}

void FurnaceTile::animateTick(Level* level, const TilePos& pos, Random* random) {
	if (m_active) {
		int data = level->getData(pos);
		Vec3 particlePos(pos.x + 0.5, pos.y + 0.0F + random->nextFloat() * 6.0F / 16.0, pos.z + 0.5F);
		const float var10 = 0.52F;
		float var11 = random->nextFloat() * 0.6F - 0.3F;
		if (data == 4 || data == 5)
		{
			particlePos.x += data == 4 ? -var10 : var10;
			particlePos.z += var11;
		}
		else if (data == 2 || data == 3)
		{
			particlePos.x += var11;
			particlePos.z += data == 2 ? -var10 : var10;
		}

		level->addParticle("smoke", particlePos);
		level->addParticle("flame", particlePos);
	}
}

int FurnaceTile::getTexture(Facing::Name face) const
{
	switch (face) {
	case Facing::UP: case Facing::DOWN: return m_TextureFrame + 17;
	case Facing::SOUTH: return m_active ? m_TextureFrame + 16 : m_TextureFrame - 1;
	default: return m_TextureFrame;
	}
}

void FurnaceTile::onPlace(Level* level, const TilePos& pos)
{
	Tile::onPlace(level, pos);
	recalcLockDir(level, pos);
}

bool FurnaceTile::use(Level* level, const TilePos& pos, Player* player)
{
	if (player->isSneaking() && player->getSelectedItem())
	{
		return false;
	}
	if (level->m_bIsOnline)
	{
		return true;
	}
	else
	{
		player->openFurnace(std::dynamic_pointer_cast<FurnaceTileEntity>(level->getTileEntity(pos)));
		return true;
	}
}

void FurnaceTile::setPlacedBy(Level* level, const TilePos& pos, Mob* mob, Facing::Name face)
{
	int rot = Mth::floor(0.5f + (mob->m_rot.y * 4.0f / 360.0f)) & 3;

	int data = 4;

	switch (rot)
	{
	case 0: data = 2; break;
	case 1: data = 5; break;
	case 2: data = 3; break;
	}

	level->setData(pos, data);
}

void FurnaceTile::onRemove(Level* level, const TilePos& pos) {
	if (keepFurnaceInventory) return;

	auto var5 = std::dynamic_pointer_cast<FurnaceTileEntity>(level->getTileEntity(pos));

	for (int var6 = 0; var6 < var5->getContainerSize(); ++var6) {
		auto var7 = var5->getItem(var6);
		if (var7) {
			float var8 = m_furnaceRandom.nextFloat() * 0.8F + 0.1F;
			float var9 = m_furnaceRandom.nextFloat() * 0.8F + 0.1F;
			float var10 = m_furnaceRandom.nextFloat() * 0.8F + 0.1F;

			while (var7->m_count) {
				int var11 = m_furnaceRandom.nextInt(21) + 10;
				if (var11 > var7->m_count) {
					var11 = var7->m_count;
				}

				var7->m_count -= var11;
				auto var12 = std::make_shared<ItemEntity>(level, pos.offset(var8, var9, var10), std::make_shared<ItemInstance>(var7->m_itemID, var11, var7->getAuxValue()));
				float var13 = 0.05F;
				var12->m_vel.x = (double)((float)m_furnaceRandom.nextGaussian() * var13);
				var12->m_vel.y = (double)((float)m_furnaceRandom.nextGaussian() * var13 + 0.2F);
				var12->m_vel.z = (double)((float)m_furnaceRandom.nextGaussian() * var13);
				level->addEntity(var12);
			}
		}
	}
}

void FurnaceTile::setLit(bool var0, Level* level, const TilePos& pos) {
	int var5 = level->getData(pos);
	auto var6 = level->getTileEntity(pos);
	keepFurnaceInventory = true;
	if (var0) {
		level->setTile(pos, Tile::furnaceLit->m_ID);
	}
	else {
		level->setTile(pos, Tile::furnace->m_ID);
	}
	keepFurnaceInventory = false;

	level->setData(pos, var5);
	level->setTileEntity(pos, var6);
}

bool FurnaceTile::hasTileEntity() const
{
	return true;
}

std::shared_ptr<TileEntity> FurnaceTile::newTileEntity()
{
	return std::make_shared<FurnaceTileEntity>();
}

int FurnaceTile::getResource(int, Random*) const
{
	return Tile::furnace->m_ID;
}

void FurnaceTile::recalcLockDir(Level* level, const TilePos& pos) {
	int var5 = level->getTile(pos.north());
	int var6 = level->getTile(pos.south());
	int var7 = level->getTile(pos.west());
	int var8 = level->getTile(pos.east());
	uint8_t var9 = 3;
	if (Tile::solid[var5] && !Tile::solid[var6]) {
		var9 = 3;
	}

	if (Tile::solid[var6] && !Tile::solid[var5]) {
		var9 = 2;
	}

	if (Tile::solid[var7] && !Tile::solid[var8]) {
		var9 = 5;
	}

	if (Tile::solid[var8] && !Tile::solid[var7]) {
		var9 = 4;
	}

	level->setData(pos, var9);
}
