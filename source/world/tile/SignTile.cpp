#include "SignTile.hpp"
#include "world/level/Level.hpp"
#include "entity/SignTileEntity.hpp"

SignTile::SignTile(int id, bool isWall) : Tile(id, TEXTURE_PLANKS, Material::wood)
{
	m_bIsWall = isWall;
	setShape(0.25F, 0.0F, 0.25F, 0.75F, 1.0F, 0.75F);
}

void SignTile::neighborChanged(Level* level, const TilePos& pos, TileID tile)
{
	bool var6 = false;
	if (!m_bIsWall) {
		if (!level->getMaterial(pos.below())->isSolid()) {
			var6 = true;
		}
	}
	else {
		int var7 = level->getData(pos);
		var6 = true;
		if (var7 == 2 && level->getMaterial(pos.south())->isSolid())
			var6 = false;

		if (var7 == 3 && level->getMaterial(pos.north())->isSolid())
			var6 = false;

		if (var7 == 4 && level->getMaterial(pos.east())->isSolid())
			var6 = false;

		if (var7 == 5 && level->getMaterial(pos.west())->isSolid())
			var6 = false;
	}

	if (var6) {
		spawnResources(level, pos, level->getData(pos));
		level->setTile(pos, TILE_AIR);
	}

	Tile::neighborChanged(level, pos, tile);
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

bool SignTile::hasTileEntity() const
{
	return true;
}

std::shared_ptr<TileEntity> SignTile::newTileEntity()
{
	return std::make_shared<SignTileEntity>();
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

int SignTile::getResource(int, Random*) const
{
	return Item::sign->m_itemID;
}
