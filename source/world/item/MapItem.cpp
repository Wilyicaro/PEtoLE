#include "MapItem.hpp"
#include "world/level/Level.hpp"
#include "network/Packet.hpp"

MapItem::MapItem(int id) : ComplexItem(id)
{
    m_maxStackSize = 1;
}

std::shared_ptr<MapItemSavedData> MapItem::createSavedData(short id, Level* level)
{
	auto savedData = level->getSavedData<MapItemSavedData>("map_" + std::to_string(id));
	if (!savedData)
	{
		id = level->getFreeMapId("map");
		savedData = std::make_shared<MapItemSavedData>("map_" + std::to_string(id));
		level->setSavedData(savedData->m_id, savedData);
	}

	return savedData;
}

std::shared_ptr<MapItemSavedData> MapItem::getMapSavedData(const std::shared_ptr<ItemInstance>& item, Level* level)
{
	auto savedData = level->getSavedData<MapItemSavedData>("map_" + std::to_string(item->getAuxValue()));
	if (!savedData)
	{
		item->setAuxValue(level->getFreeMapId("map"));
		savedData = std::make_shared<MapItemSavedData>("map_" + std::to_string(item->getAuxValue()));
		savedData->m_xCenter = level->getLevelData().getSpawn().x;
		savedData->m_zCenter = level->getLevelData().getSpawn().z;;
		savedData->m_scale = 3;
		savedData->m_dimension = level->m_pDimension->m_ID;
		savedData->setDirty();
		level->setSavedData(savedData->m_id, savedData);
	}

	return savedData;
}

void MapItem::inventoryTick(const std::shared_ptr<ItemInstance>& item, Level* level, Entity* entity, int, bool isSelected)
{
	if (!level->m_bIsOnline)
	{
		auto var6 = getMapSavedData(item, level);
		if (entity->isPlayer())
		{
			var6->tickCarriedBy(std::dynamic_pointer_cast<Player>(entity->shared_from_this()), item);
		}

		if (isSelected) {
			update(level, entity, var6);
		}

	}
}

void MapItem::update(Level* level, Entity* entity, const std::shared_ptr<MapItemSavedData>& data)
{
    if (level->m_pDimension->m_ID != data->m_dimension) {
        return;
    }

    const int mapWidth = 128;
    const int mapHeight = 128;
    const int scale = 1 << data->m_scale;

    const int centerX = data->m_xCenter;
    const int centerZ = data->m_zCenter;

    int mapX = Mth::floor(entity->m_pos.x - centerX) / scale + mapWidth / 2;
    int mapZ = Mth::floor(entity->m_pos.z - centerZ) / scale + mapHeight / 2;

    int radius = mapWidth / scale;
    if (level->m_pDimension->m_bHasCeiling)
        radius /= 2;

    ++data->m_ticks;

    for (int xOnMap = mapX - radius + 1; xOnMap < mapX + radius; ++xOnMap)
    {
        if ((xOnMap & 15) != (data->m_ticks & 15))
        {
            continue;
        }

        int minDirtyZ = 255;
        int maxDirtyZ = 0;
        real prevAvgHeight = 0.0;

        for (int zOnMap = mapZ - radius - 1; zOnMap < mapZ + radius; ++zOnMap) {
            if (xOnMap < 0 || zOnMap < -1 || xOnMap >= mapWidth || zOnMap >= mapHeight) {
                continue;
            }

            int dx = xOnMap - mapX;
            int dz = zOnMap - mapZ;
            bool outsideCircle = dx * dx + dz * dz > (radius - 2) * (radius - 2);

            TilePos tilePos(
                (centerX / scale + xOnMap - mapWidth / 2) * scale,
                0,
                (centerZ / scale + zOnMap - mapHeight / 2) * scale
            );

            int blockCount[256] = {};
            int waterDepth = 0;
            real avgHeight = 0.0;

            if (!level->isValidPos(tilePos)) continue;

            LevelChunk* chunk = level->getChunkAt(tilePos);

            if (level->m_pDimension->m_bHasCeiling)
            {
                int noise = tilePos.x + tilePos.z * 231871;
                noise = noise * noise * 31287121 + noise * 11;
                if ((noise >> 20 & 1) == 0) {
                    blockCount[Tile::dirt->m_ID] += 10;
                }
                else {
                    blockCount[Tile::stone->m_ID] += 10;
                }
                avgHeight = 100.0;
            }
            else
            {
                ChunkTilePos cPos;
                for (int cDx = 0; cDx < scale; cDx++)
                {
                    cPos.x = (tilePos.x & 15) + cDx;
                    for (int cDz = 0; cDz < scale; cDz++)
                    {
                        cPos.z = (tilePos.z & 15) + cDz;
                        cPos.y = 0;
                        cPos.y = chunk->getHeightmap(cPos) + 1;

                        int tileId = 0;
                        if (cPos.y > 1)
                        {
                            while (true)
                            {
                                ChunkTilePos below(cPos.x, cPos.y - 1, cPos.z);

                                tileId = chunk->getTile(below);
                                if (tileId == 0)
                                {
                                    cPos.y--;
                                    continue;
                                }

                                auto mat = Tile::tiles[tileId]->m_pMaterial;
                                if (cPos.y > 0 && mat->m_pMapColor == MapColor::air)
                                {
                                    cPos.y--;
                                    continue;
                                }

                                if (mat->isLiquid())
                                {
                                    int liquidDepth = 0;
                                    ChunkTilePos ctp = below;
                                    while (true)
                                    {
                                        int belowId = chunk->getTile(ctp);
                                        ctp.y--;
                                        liquidDepth++;
                                        if (ctp.y <= 0 || belowId == 0 || !Tile::tiles[belowId]->m_pMaterial->isLiquid()) {
                                            break;
                                        }
                                    }
                                    waterDepth += liquidDepth;
                                }
                                break;
                            }
                        }

                        avgHeight += real(cPos.y) / real(scale * scale);
                        blockCount[tileId]++;
                    }
                }
            }

            waterDepth /= (scale * scale);

            int dominantBlock = 0;
            int dominantCount = 0;
            for (int id = 0; id < 256; ++id) {
                if (blockCount[id] > dominantCount) {
                    dominantBlock = id;
                    dominantCount = blockCount[id];
                }
            }

            real heightDiff = (avgHeight - prevAvgHeight) * 4.0 / (scale + 4)
                + ((xOnMap + zOnMap & 1) - 0.5) * 0.4;

            uint8_t shade = 1;
            if (heightDiff > 0.6) shade = 2;
            if (heightDiff < -0.6) shade = 0;

            int colorId = 0;
            if (dominantBlock > 0)
            {
                const MapColor* color = Tile::tiles[dominantBlock]->m_pMaterial->m_pMapColor;
                if (color == MapColor::water)
                {
                    real waterShade = waterDepth * 0.1 + (xOnMap + zOnMap & 1) * 0.2;
                    shade = (waterShade < 0.5) ? 2 : (waterShade > 0.9 ? 0 : 1);
                }
                colorId = color->m_id;
            }

            prevAvgHeight = avgHeight;

            if (zOnMap >= 0 && dx * dx + dz * dz < radius * radius && (!outsideCircle || (xOnMap + zOnMap & 1) != 0))
            {
                uint8_t newColor = colorId * 4 + shade;
                uint8_t& currentColor = data->m_colors[xOnMap + zOnMap * mapWidth];

                if (currentColor != newColor) {
                    if (minDirtyZ > zOnMap) minDirtyZ = zOnMap;
                    if (maxDirtyZ < zOnMap) maxDirtyZ = zOnMap;
                    currentColor = newColor;
                }
            }
        }

        if (minDirtyZ <= maxDirtyZ)
            data->markDirty(xOnMap, minDirtyZ, maxDirtyZ);
    }
}

void MapItem::onCraftedBy(const std::shared_ptr<ItemInstance>& item, Player* player, Level* level)
{
	item->setAuxValue(level->getFreeMapId("map"));
	auto savedData = std::make_shared<MapItemSavedData>("map_" + std::to_string(item->getAuxValue()));
	level->setSavedData(savedData->m_id, savedData);
	savedData->m_xCenter = Mth::floor(player->m_pos.x);
	savedData->m_zCenter = Mth::floor(player->m_pos.z);
	savedData->m_scale = 3;
	savedData->m_dimension = level->m_pDimension->m_ID;
	savedData->setDirty();
}

Packet* MapItem::getUpdatePacket(const std::shared_ptr<ItemInstance>& map, Level* level, const std::shared_ptr<Player>& player)
{
    std::vector<uint8_t> data = getMapSavedData(map, level)->getColors(map, level, player);
    return data.empty() ? nullptr : new MapItemDataPacket(Item::map->m_itemID, map->getAuxValue(), data);
}
