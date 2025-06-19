#include "CraftingTableTile.hpp"
#include "world/level/Level.hpp"

CraftingTableTile::CraftingTableTile(int id) : Tile(id, 59, Material::wood)
{
}

bool CraftingTableTile::use(Level* level, const TilePos& pos, Player* player)
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
		player->startCrafting(pos);
		return true;
	}
}

int CraftingTableTile::getTexture(Facing::Name face) const
{
	switch (face) {
	case Facing::UP: return m_TextureFrame - 16;
	case Facing::DOWN: return Tile::wood->getTexture(face);
	case Facing::NORTH: case Facing::SOUTH: return m_TextureFrame + 1;
	default: return m_TextureFrame;
	}
}
