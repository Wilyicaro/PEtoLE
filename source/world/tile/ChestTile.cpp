#include "ChestTile.hpp"
#include "world/level/Level.hpp"
#include "world/item/CompoundContainer.hpp"
#include "entity/ChestTileEntity.hpp"

ChestTile::ChestTile(int id, int texture) : Tile(id, texture, Material::wood)
{
}

int ChestTile::getTexture(const LevelSource* level, const TilePos& pos, Facing::Name face) const {
    if (face == Facing::UP || face == Facing::DOWN) {
        return m_TextureFrame - 1;
    }

    int id_north = level->getTile(pos.north());
    int id_south = level->getTile(pos.south());
    int id_west = level->getTile(pos.west());
    int id_east = level->getTile(pos.east());

    bool isDoubleNS = (id_north == m_ID || id_south == m_ID);
    bool isDoubleWE = (id_west == m_ID || id_east == m_ID);

    if (!isDoubleNS && !isDoubleWE) {
        Facing::Name front = Facing::SOUTH;

        if (Tile::solid[id_north] && !Tile::solid[id_south]) front = Facing::SOUTH;
        else if (Tile::solid[id_south] && !Tile::solid[id_north]) front = Facing::NORTH;
        else if (Tile::solid[id_west] && !Tile::solid[id_east])  front = Facing::EAST;
        else if (Tile::solid[id_east] && !Tile::solid[id_west])  front = Facing::WEST;

        return face == front ? m_TextureFrame + 1 : m_TextureFrame;
    }

    bool left = false;
    Facing::Name front = Facing::SOUTH;
    int offset = 0;

    if (isDoubleWE) {
        left = id_west == m_ID;
        TilePos side = left ? pos.west() : pos.east();

        int id_behind = level->getTile(side.north());
        int id_infront = level->getTile(side.south());

        if ((Tile::solid[id_north] || Tile::solid[id_behind]) && !Tile::solid[id_south] && !Tile::solid[id_infront]) {
            front = Facing::SOUTH;
        }
        else if ((Tile::solid[id_south] || Tile::solid[id_infront]) && !Tile::solid[id_north] && !Tile::solid[id_behind]) {
            front = Facing::NORTH;
        }

        if (front == Facing::SOUTH) left = !left;

        if (face == front) return m_TextureFrame + (left ? 15 : 16);
        if (face == Facing::OPPOSITE[front]) return m_TextureFrame + (left ? 32 : 31);
        return m_TextureFrame;
    }

    if (isDoubleNS) {
        front = Facing::EAST;
        left = id_north == m_ID;
        TilePos side = left ? pos.north() : pos.south();

        int id_behind = level->getTile(side.west());
        int id_infront = level->getTile(side.east());

        if ((Tile::solid[id_west] || Tile::solid[id_behind]) && !Tile::solid[id_east] && !Tile::solid[id_infront]) {
            front = Facing::EAST;
        }
        else if ((Tile::solid[id_east] || Tile::solid[id_infront]) && !Tile::solid[id_west] && !Tile::solid[id_behind]) {
            front = Facing::WEST;
            left = !left;
        }

        if (face == front) return m_TextureFrame + (left ? 15 : 16);
        if (face == Facing::OPPOSITE[front]) return m_TextureFrame + (left ? 32 : 31);
        return m_TextureFrame;
    }

    return m_TextureFrame;
}

int ChestTile::getTexture(Facing::Name face) const
{
	switch (face) {
	case Facing::UP: case Facing::DOWN: return m_TextureFrame - 1;
	case Facing::SOUTH: return m_TextureFrame + 1;
	default: return m_TextureFrame;
	}
}

bool ChestTile::mayPlace(const Level* level, const TilePos& pos) const
{
    return !hasNeighbors(level, pos, 1);
}

bool ChestTile::hasNeighbors(const Level* level, const TilePos& pos, int count) const
{
    int neighbors = 0;
    for (Facing::Name i : Facing::HORIZONTAL)
    {
        TilePos relative = pos.relative(i);
        if (level->getTile(relative) == m_ID)
        {
            neighbors++;
            if (neighbors > count) return true;

            if (hasNeighbors(level, relative, 0)) return true;
        }
    }

    return false;
}

void ChestTile::onRemove(Level* level, const TilePos& pos){
    auto var5 = std::dynamic_pointer_cast<ChestTileEntity>(level->getTileEntity(pos));

    for (int var6 = 0; var6 < var5->getContainerSize(); ++var6) {
        auto var7 = var5->getItem(var6);
        if (var7) {
            float var8 = m_chestRandom.nextFloat() * 0.8F + 0.1F;
            float var9 = m_chestRandom.nextFloat() * 0.8F + 0.1F;
            float var10 = m_chestRandom.nextFloat() * 0.8F + 0.1F;

            while (var7->m_count) {
                int var11 = m_chestRandom.nextInt(21) + 10;
                if (var11 > var7->m_count) {
                    var11 = var7->m_count;
                }

                var7->m_count -= var11;
                auto var12 = std::make_shared<ItemEntity>(level, pos.offset(var8, var9, var10), new ItemInstance(var7->m_itemID, var11, var7->getAuxValue()));
                float var13 = 0.05F;
                var12->m_vel.x = (double)((float)m_chestRandom.nextGaussian() * var13);
                var12->m_vel.y = (double)((float)m_chestRandom.nextGaussian() * var13 + 0.2F);
                var12->m_vel.z = (double)((float)m_chestRandom.nextGaussian() * var13);
                level->addEntity(var12);
            }
        }
    }

    Tile::onRemove(level, pos);
}

bool ChestTile::use(Level* level, const TilePos& pos, Player* player) 
{
    if (level->m_bIsOnline) {
        return true;
    }
    if (level->isSolidTile(pos.above())) {
        return true;
    }
    else if (level->getTile(pos.west()) == m_ID && level->isSolidTile(pos.offset(-1, 1, 0))) {
        return true;
    }
    else if (level->getTile(pos.east()) == m_ID && level->isSolidTile(pos.offset(1, 1, 0))) {
        return true;
    }
    else if (level->getTile(pos.north()) == m_ID && level->isSolidTile(pos.offset(0, 1, -1))) {
        return true;
    }
    else if (level->getTile(pos.south()) == m_ID && level->isSolidTile(pos.offset(0, 1, 1))) {
        return true;
    }
    else {
        Container* container = std::dynamic_pointer_cast<Container>(level->getTileEntity(pos)).get();
        for (int rel = Facing::NORTH; rel <= Facing::EAST; rel++) 
        {
            TilePos relPos = pos.relative((Facing::Name)rel);
            if (level->getTile(pos.relative((Facing::Name)rel)) == m_ID)
            {
                if (rel % 2 == 0)
                    container = new CompoundContainer("gui.bigChest", std::dynamic_pointer_cast<Container>(level->getTileEntity(relPos)).get(), container);
                else 
                    container = new CompoundContainer("gui.bigChest", container, std::dynamic_pointer_cast<Container>(level->getTileEntity(relPos)).get());
                break;
            }
        }

        player->openContainer(container);
        return true;
    }
}

bool ChestTile::hasTileEntity() const
{
    return true;
}

std::shared_ptr<TileEntity> ChestTile::newTileEntity()
{
    return std::make_shared<ChestTileEntity>();
}
