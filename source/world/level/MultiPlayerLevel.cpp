#include "MultiPlayerLevel.hpp"
#include "world/level/levelgen/chunk/MultiPlayerChunkCache.hpp"

MultiPlayerLevel::MultiPlayerLevel(int64_t seed, Dimension* pDimension) : Level()
{
    //@NOTE: Should be managed by ClientSideNetworkHandler
    m_pDataStorage = new DimensionDataStorage(nullptr);
	m_levelData.setSeed(seed);
	m_levelData.setLevelName("MpServer");
	init(pDimension);
}

void MultiPlayerLevel::tick()
{
    tickWeather();
    setTime(getTime() + 1);
    int newDark = getSkyDarken(1.0F);
    int i;
    if (newDark != m_skyDarken)
    {
        m_skyDarken = newDark;

        for (auto& l : m_levelListeners)
            l->skyColorChanged();
    }

    auto it = m_reEntries.begin();
    for (i = 0; i < 10 && it != m_reEntries.end(); ++i)
    {
        if (std::find(m_entities.begin(), m_entities.end(), *it) == m_entities.end() && addEntity(*it))
            it = m_reEntries.begin();
        else
            ++it;
    }

    for (size_t i = 0; i < m_updatesToReset.size(); )
    {
        auto& r = m_updatesToReset[i];
        if (--r.m_ticks == 0)
        {
            Level::setTileAndDataNoUpdate(r.m_pos, r.m_tile, r.m_data);
            Level::sendTileUpdated(r.m_pos);
            m_updatesToReset.erase(m_updatesToReset.begin() + i);
        }
        else
            ++i;
    }

}

void MultiPlayerLevel::clearResetRegion(const TilePos& pos, const TilePos& pos1)
{
    for (size_t i = 0; i < m_updatesToReset.size(); )
    {
        auto& r = m_updatesToReset[i];
        if (r.m_pos.x >= pos.x && r.m_pos.y >= pos.y && r.m_pos.z >= pos.z && r.m_pos.x <= pos1.x && r.m_pos.y <= pos1.y && r.m_pos.z <= pos1.z)
            m_updatesToReset.erase(m_updatesToReset.begin() + i);
        else
            ++i;
    }
}

ChunkSource* MultiPlayerLevel::createChunkSource()
{
	return new MultiPlayerChunkCache(this);
}

void MultiPlayerLevel::tickTiles()
{
}

bool MultiPlayerLevel::tickPendingTicks(bool b)
{
    return false;
}

void MultiPlayerLevel::addToTickNextTick(const TilePos& tilePos, int, int)
{
}

bool MultiPlayerLevel::addEntity(std::shared_ptr<Entity> e)
{
    bool ok = Level::addEntity(e);
    m_forced.insert(e);
    if (!ok)
        m_reEntries.insert(e);

    return ok;
}

bool MultiPlayerLevel::removeEntity(const std::shared_ptr<Entity>& e)
{
    m_forced.erase(e);
    return Level::removeEntity(e);
}

void MultiPlayerLevel::entityAdded(const std::shared_ptr<Entity>& pEnt)
{
    Level::entityAdded(pEnt);
    if (std::find(m_reEntries.begin(), m_reEntries.end(), pEnt->shared_from_this()) != m_reEntries.end())
        m_reEntries.erase(pEnt->shared_from_this());
}

void MultiPlayerLevel::entityRemoved(const std::shared_ptr<Entity>& pEnt)
{
    Level::entityRemoved(pEnt);
    if (std::find(m_forced.begin(), m_forced.end(), pEnt->shared_from_this()) != m_forced.end())
        m_reEntries.insert(pEnt->shared_from_this());
}

void MultiPlayerLevel::putEntity(int id, std::shared_ptr<Entity> e)
{
    auto old = getEntity(id);
    if (old)
        removeEntity(old);

    m_forced.insert(e);
    e->m_EntityID = id;
    if (!addEntity(e))
        m_reEntries.insert(e);

    m_entitiesById[id] = e;
}

std::shared_ptr<Entity> MultiPlayerLevel::getEntity(int id)
{
    auto it = m_entitiesById.find(id);
    return it != m_entitiesById.end() ? it->second : nullptr;
}

std::shared_ptr<Entity> MultiPlayerLevel::removeEntity(int id)
{
    auto it = m_entitiesById.find(id);
    if (it != m_entitiesById.end())
    {
        auto ent = it->second;
        m_entitiesById.erase(it);
        m_forced.erase(ent);
        removeEntity(ent);
        return ent;
    }

    return nullptr;
}

bool MultiPlayerLevel::setDataNoUpdate(const TilePos& pos, int data)
{
    int t = getTile(pos);
    int d = getData(pos);
    if (Level::setDataNoUpdate(pos, data))
    {
        m_updatesToReset.push_back(ResetInfo(pos, t, d));
        return true;
    }
    else
        return false;
}

bool MultiPlayerLevel::setTileAndDataNoUpdate(const TilePos& pos, TileID tile, int data)
{
    int t = getTile(pos);
    int d = getData(pos);
    if (Level::setTileAndDataNoUpdate(pos, tile, data))
    {
        m_updatesToReset.push_back(ResetInfo(pos, t, d));
        return true;
    }
    else
        return false;
}

bool MultiPlayerLevel::setTileNoUpdate(const TilePos& pos, TileID tile)
{
    int t = getTile(pos);
    int d = getData(pos);
    if (Level::setTileNoUpdate(pos, tile))
    {
        m_updatesToReset.push_back(ResetInfo(pos, t, d));
        return true;
    }
    else
        return false;
}

bool MultiPlayerLevel::doSetTileAndDataNoUpdate(const TilePos& pos, TileID tile, int data)
{
    clearResetRegion(pos, pos);
    if (Level::setTileAndDataNoUpdate(pos, tile, data))
    {
        tileUpdated(pos, tile);
        return true;
    }
    else
        return false;
}

