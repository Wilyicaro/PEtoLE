#include "PortalForcer.hpp"
#include "world/tile/PortalTile.hpp"

void PortalForcer::force(Level* level, Entity* entity)
{
    if (!findPortal(level, entity))
    {
        createPortal(level, entity);
        findPortal(level, entity);
    }
}

bool PortalForcer::findPortal(Level* level, Entity* entity)
{
    const constexpr short var3 = 128;
    real var4 = -1.0;
    TilePos cPos;
    int var9 = Mth::floor(entity->m_pos.x);
    int var10 = Mth::floor(entity->m_pos.z);
    real fy;
    TilePos tp;
    for (tp.x = var9 - var3; tp.x <= var9 + var3; ++tp.x)
    {
        real var12 = tp.x + 0.5 - entity->m_pos.x;

        for (tp.z = var10 - var3; tp.z <= var10 + var3; ++tp.z)
        {
            real var15 = tp.z + 0.5 - entity->m_pos.z;

            for (tp.y = 127; tp.y >= 0; --tp.y) {
                if (level->getTile(tp) == Tile::portal->m_ID)
                {
                    while (level->getTile(tp.below()) == Tile::portal->m_ID)
                        --tp.y;

                    fy = tp.y + 0.5 - entity->m_pos.y;
                    real var20 = var12 * var12 + fy * fy + var15 * var15;
                    if (var4 < 0.0 || var20 < var4)
                    {
                        var4 = var20;
                        cPos = tp;
                    }
                }
            }
        }
    }

    if (var4 >= 0.0) {
        Vec3 movePos = cPos.center();
        if (level->getTile(cPos.west()) == Tile::portal->m_ID)
            movePos.x -= 0.5;

        if (level->getTile(cPos.east()) == Tile::portal->m_ID)
            movePos.x += 0.5;

        if (level->getTile(cPos.north()) == Tile::portal->m_ID)
            movePos.z -= 0.5;

        if (level->getTile(cPos.south()) == Tile::portal->m_ID)
            movePos.z += 0.5;

        entity->moveTo(movePos, Vec2(entity->m_rot.y, 0.0F));
        entity->m_vel = Vec3::ZERO;
        return true;
    }
    else
        return false;
}

bool PortalForcer::createPortal(Level* level, Entity* entity) {
    const int searchRadius = 16;
    real bestDistance = -1.0;
    int px = Mth::floor(entity->m_pos.x);
    int py = Mth::floor(entity->m_pos.y);
    int pz = Mth::floor(entity->m_pos.z);

    int bestX = px;
    int bestY = py;
    int bestZ = pz;
    int bestOrientation = 0;

    int baseOrientation = m_random.nextInt(4);

    TilePos tp;
    for (int x = px - searchRadius; x <= px + searchRadius; ++x) {
        tp.x = x;
        real dx = x + 0.5 - entity->m_pos.x;
        for (int z = pz - searchRadius; z <= pz + searchRadius; ++z) {
            tp.z = z;
            real dz = z + 0.5 - entity->m_pos.z;

            for (int y = 127; y >= 0; --y) {
                tp.y = y;
                if (!level->isEmptyTile(tp)) continue;
                int ny = y;
                while (ny > 0 && level->isEmptyTile(tp.below())) --ny;

                for (int o = baseOrientation; o < baseOrientation + 4; ++o) {
                    int dxo = o % 2;
                    int dzo = 1 - dxo;
                    if (o % 4 >= 2) {
                        dxo = -dxo;
                        dzo = -dzo;
                    }

                    bool valid = true;
                    for (int w = 0; w < 3 && valid; ++w) {
                        for (int h = 0; h < 4 && valid; ++h) {
                            for (int d = -1; d < 4 && valid; ++d) {
                                tp.x = x + (h - 1) * dxo + w * dzo;
                                tp.y = ny + d;
                                tp.z = z + (h - 1) * dzo - w * dxo;

                                if ((d < 0 && !level->getMaterial(tp)->isSolid()) ||
                                    (d >= 0 && !level->isEmptyTile(tp))) {
                                    valid = false;
                                }
                            }
                        }
                    }

                    if (valid) {
                        real dy = ny + 0.5 - entity->m_pos.y;
                        real dist = dx * dx + dy * dy + dz * dz;
                        if (bestDistance < 0.0 || dist < bestDistance) {
                            bestDistance = dist;
                            bestX = x;
                            bestY = ny;
                            bestZ = z;
                            bestOrientation = o % 4;
                        }
                    }
                }
            }
        }
    }

    int dxo = bestOrientation % 2;
    int dzo = 1 - dxo;
    if (bestOrientation >= 2) {
        dxo = -dxo;
        dzo = -dzo;
    }

    if (bestDistance < 0.0) {
        bestY = Mth::clamp(py, 70, 118);

        for (int w = -1; w <= 1; ++w) {
            for (int h = 1; h < 3; ++h) {
                for (int d = -1; d < 3; ++d) {
                    tp.x = bestX + (h - 1) * dxo + w * dzo;
                    tp.y = bestY + d;
                    tp.z = bestZ + (h - 1) * dzo - w * dxo;
                    bool isFrame = d < 0;
                    level->setTile(tp, isFrame ? Tile::obsidian->m_ID : 0);
                }
            }
        }
    }

    for (int pass = 0; pass < 4; ++pass) {
        level->m_noNeighborUpdate = true;

        for (int h = 0; h < 4; ++h) {
            for (int d = -1; d < 4; ++d) {
                tp.x = bestX + (h - 1) * dxo;
                tp.y = bestY + d;
                tp.z = bestZ + (h - 1) * dzo;
                bool isFrame = h == 0 || h == 3 || d == -1 || d == 3;
                level->setTile(tp, isFrame ? Tile::obsidian->m_ID : Tile::portal->m_ID);
            }
        }

        level->m_noNeighborUpdate = false;

        for (int h = 0; h < 4; ++h) {
            for (int d = -1; d < 4; ++d) {
                tp.x = bestX + (h - 1) * dxo;
                tp.y = bestY + d;
                tp.z = bestZ + (h - 1) * dzo;
                level->updateNeighborsAt(tp, level->getTile(tp));
            }
        }
    }

    return true;
}
