#include "TileEntity.hpp"
#include "world/level/Level.hpp"
#include "FurnaceTileEntity.hpp"
#include "ChestTileEntity.hpp"
#include "MobSpawnerTileEntity.hpp"
//#include "DispenserTileEntity.hpp"
//#include "SignTileEntity.hpp"
//#include "MobSpawnerTileEntity.hpp"
//#include "MusicTileEntity.hpp"

std::unordered_map<std::string, TileEntityFactory> TileEntity::idClassMap;
std::unordered_map<std::type_index, std::string> TileEntity::classIdMap;

void TileEntity::setId(std::type_index type, const std::string& id, TileEntityFactory factory) {
    if (idClassMap.find(id) != idClassMap.end()) {
        throw std::runtime_error("Duplicate id: " + id);
    }
    idClassMap[id] = factory;
    classIdMap[type] = id;
}


void TileEntity::initTileEntities()
{
    registerTileEntity<FurnaceTileEntity>("Furnace");
    registerTileEntity<ChestTileEntity>("Chest");
    //registerTileEntity<DispenserTileEntity>("Trap");
    //registerTileEntity<SignTileEntity>("Sign");
    registerTileEntity<MobSpawnerTileEntity>("MobSpawner");
    //registerTileEntity<MusicTileEntity>("Music");
}

std::shared_ptr<TileEntity> TileEntity::loadStatic(std::shared_ptr<CompoundTag> tag) {
    std::string id = tag->contains("id") ? tag->getString("id") : "";
    auto it = idClassMap.find(id);
    if (it != idClassMap.end()) {
        auto tileEntity = it->second();
        tileEntity->load(tag);
        return tileEntity;
    } else {
        LOG_I("Skipping TileEntity with id %s", id.c_str());
        return nullptr;
    }
}

void TileEntity::load(std::shared_ptr<CompoundTag> tag) {
    m_pos.x = tag->getInt("x");
    m_pos.y = tag->getInt("y");
    m_pos.z = tag->getInt("z");
}

void TileEntity::save(std::shared_ptr<CompoundTag> tag) {
    auto it = classIdMap.find(std::type_index(typeid(*this)));
    if (it == classIdMap.end()) {
        throw std::runtime_error("Missing mapping for class: " + std::string(typeid(*this).name()));
    }
    tag->putString("id", it->second);
    tag->putInt("x", m_pos.x);
    tag->putInt("y", m_pos.y);
    tag->putInt("z", m_pos.z);
}

int TileEntity::getData() const {
    return m_pLevel->getData(m_pos);
}

void TileEntity::setData(int data) {
    m_pLevel->setData(m_pos, data);
}

void TileEntity::setChanged() {
    if (m_pLevel) {
        m_pLevel->tileEntityChanged(m_pos, shared_from_this());
    }
}

double TileEntity::distanceToSqr(const Vec3& vec ) const {
    double dx = double(m_pos.x) + 0.5 - vec.x;
    double dy = double(m_pos.y) + 0.5 - vec.y;
    double dz = double(m_pos.z) + 0.5 - vec.z;
    return dx * dx + dy * dy + dz * dz;
}

Tile* TileEntity::getTile() const {
    return Tile::tiles[m_pLevel->getTile(m_pos)];
}

std::string TileEntity::getId() const {
    auto it = classIdMap.find(std::type_index(typeid(*this)));
    if (it != classIdMap.end()) {
        return it->second;
    }
    return "Unknown";
}
