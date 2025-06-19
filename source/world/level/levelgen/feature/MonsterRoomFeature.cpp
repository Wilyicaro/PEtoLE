/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "Feature.hpp"
#include "world/level/Level.hpp"
#include <world/tile/entity/MobSpawnerTileEntity.hpp>
#include <world/tile/entity/ChestTileEntity.hpp>

bool MonsterRoomFeature::place(Level* level, Random* random, const TilePos& pos)
{
    uint8_t var6 = 3;
    int var7 = random->nextInt(2) + 2;
    int var8 = random->nextInt(2) + 2;
    int var9 = 0;

    TilePos tp;
    for (tp.x = pos.x - var7 - 1; tp.x <= pos.x + var7 + 1; ++tp.x) {
        for (tp.y = pos.y - 1; tp.y <= pos.y + var6 + 1; ++tp.y) {
            for (tp.z = pos.z - var8 - 1; tp.z <= pos.z + var8 + 1; ++tp.z) {
                Material* var13 = level->getMaterial(tp);
                if (tp.y == pos.y - 1 && !var13->isSolid()) {
                    return false;
                }

                if (tp.y == pos.y + var6 + 1 && !var13->isSolid()) {
                    return false;
                }

                if ((tp.x == pos.x - var7 - 1 || tp.x == pos.x + var7 + 1 || tp.z == pos.z - var8 - 1 || tp.z == pos.z + var8 + 1) && tp.y == pos.y && level->isEmptyTile(tp) && level->isEmptyTile(tp.above())) {
                    ++var9;
                }
            }
        }
    }

    if (var9 >= 1 && var9 <= 5) {
        for (tp.x = pos.x - var7 - 1; tp.x <= pos.x + var7 + 1; ++tp.x) {
            for (tp.y = pos.y + var6; tp.y >= pos.y - 1; --tp.y) {
                for (tp.z = pos.z - var8 - 1; tp.z <= pos.z + var8 + 1; ++tp.z) {
                    if (tp.x != pos.x - var7 - 1 && tp.y != pos.y - 1 && tp.z != pos.z - var8 - 1 && tp.x != pos.x + var7 + 1 && tp.y != pos.y + var6 + 1 && tp.z != pos.z + var8 + 1) {
                        level->setTile(tp, TILE_AIR);
                    }
                    else if (tp.y >= 0 && !level->getMaterial(tp.below())->isSolid()) {
                        level->setTile(tp, TILE_AIR);
                    }
                    else if (level->getMaterial(tp)->isSolid()) {
                        if (tp.y == pos.y - 1 && random->nextInt(4) != 0) {
                            level->setTile(tp, Tile::mossStone->m_ID);
                        }
                        else {
                           level->setTile(tp, Tile::cobblestone->m_ID);
                        }
                    }
                }
            }
        }

        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j) {
                int x = pos.x + random->nextInt(var7 * 2 + 1) - var7;
                int z = pos.z + random->nextInt(var8 * 2 + 1) - var8;
                TilePos chestPos(x, pos.y, z);

                if (level->isEmptyTile(chestPos)) {
                    int solidSides = 0;

                    for (Facing::Name rel : Facing::HORIZONTAL)
                    {
                        if (level->getMaterial(chestPos.relative(rel))->isSolid()) ++solidSides;
                    }

                    if (solidSides == 1) {
                        level->setTile(chestPos, Tile::chest->m_ID);

                        auto chest = std::dynamic_pointer_cast<ChestTileEntity>(level->getTileEntity(chestPos));

                        for (int k = 0; k < 8; ++k) {
                            auto item = randomItem(random);
                            if (item) {
                                chest->setItem(random->nextInt(chest->getContainerSize()), item);
                            }
                        }

                        break;
                    }
                }
            }
        }

        level->setTile(pos, Tile::mobSpawner->m_ID);
        auto var19 = std::dynamic_pointer_cast<MobSpawnerTileEntity>(level->getTileEntity(pos));
        auto entity = randomEntityId(random);
        if (var19) var19->setEntityId(entity);
        return true;
    }
    else {
        return false;
    }
}

std::shared_ptr<ItemInstance> MonsterRoomFeature::randomItem(Random* random) {
    int var2 = random->nextInt(11);
    if (var2 == 0) {
        return std::make_shared<ItemInstance>(Item::saddle);
    }
    else if (var2 == 1) {
        return std::make_shared<ItemInstance>(Item::ironIngot, random->nextInt(4) + 1);
    }
    else if (var2 == 2) {
        return std::make_shared<ItemInstance>(Item::bread);
    }
    else if (var2 == 3) {
        return std::make_shared<ItemInstance>(Item::wheat, random->nextInt(4) + 1);
    }
    else if (var2 == 4) {
        return std::make_shared<ItemInstance>(Item::sulphur, random->nextInt(4) + 1);
    }
    else if (var2 == 5) {
        return std::make_shared<ItemInstance>(Item::string, random->nextInt(4) + 1);
    }
    else if (var2 == 6) {
        return std::make_shared<ItemInstance>(Item::emptyBucket);
    }
    else if (var2 == 7 && random->nextInt(100) == 0) {
        return std::make_shared<ItemInstance>(Item::goldApple);
    }
    else if (var2 == 8 && random->nextInt(2) == 0) {
        return std::make_shared<ItemInstance>(Item::redStone, random->nextInt(4) + 1);
    }
    else {
        return var2 == 9 && random->nextInt(10) == 0 ? std::make_shared<ItemInstance>(Item::items[Item::record_01->m_itemID + random->nextInt(2)]) : nullptr;
    }
}

std::string MonsterRoomFeature::randomEntityId(Random* random) {
    int var2 = random->nextInt(4);
    //switch (var2)
    //{
    //case 0: return EntityType::skeleton->getName();
    //case 3: return EntityType::spider->getName();
    //default: return EntityType::zombie->getName();
    //}
    return EntityType::cow->getName();
}
