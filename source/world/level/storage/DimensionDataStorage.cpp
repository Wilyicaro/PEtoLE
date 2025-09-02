#include "DimensionDataStorage.hpp"
#include <cstdio>
#include <stdexcept>
#include "MinecraftServer.hpp"
#include <common/NbtIo.hpp>
#include <world/level/saveddata/MapItemSavedData.hpp>

DimensionDataStorage::DimensionDataStorage(MinecraftServer* server) : m_server(server)
{
    readMapIds();
}

template <typename T>
std::shared_ptr<T> DimensionDataStorage::computeIfAbsent(const std::string& key)
{
    auto it = m_cache.find(key);
    if (it != m_cache.end()) {
        return std::dynamic_pointer_cast<T>(it->second);
    }

    std::shared_ptr<T> data = nullptr;

    if (m_server)
    {
        auto tag = NbtIo::read(m_server->m_path + "/data/" + key + ".dat");
        if (tag)
        {
            data = std::make_shared<T>(key);
            data->load(tag->getCompound("data"));
        }
    }

    if (data)
    {
        m_cache[key] = data;
        m_dirtyList.push_back(data);
    }

    return data;
}

template std::shared_ptr<MapItemSavedData>
DimensionDataStorage::computeIfAbsent<MapItemSavedData>(const std::string& name);

void DimensionDataStorage::set(const std::string& key, std::shared_ptr<SavedData> data)
{
    if (!data) {
        throw std::runtime_error("Can't set null data");
    }

    auto it = m_cache.find(key);
    if (it != m_cache.end()) {
        auto& old = it->second;
        m_dirtyList.erase(std::remove(m_dirtyList.begin(), m_dirtyList.end(), old), m_dirtyList.end());
        m_cache.erase(it);
    }

    m_cache[key] = data;
    m_dirtyList.push_back(data);
}

int DimensionDataStorage::getFreeMapId(const std::string& key)
{
    short id = 0;
    auto it = m_usedAuxIds.find(key);
    if (it != m_usedAuxIds.end()) {
        id = it->second + 1;
    }
    m_usedAuxIds[key] = id;

    if (!m_server) return id;

    std::string path = m_server->m_path + "/data/idcounts.dat";
    FILE* f = fopen(path.c_str(), "wb");
    if (f)
    {
        auto tag = std::make_shared<CompoundTag>();
        for (auto& kv : m_usedAuxIds)
            tag->putShort(kv.first, kv.second);

        NbtIo::write(tag, f);
    }

    return id;
}

void DimensionDataStorage::save(const std::shared_ptr<SavedData>& data)
{
    if (!m_server) return;

    std::string path = m_server->m_path + "/data/" + data->m_id + ".dat";
    FILE* f = fopen(path.c_str(), "wb");
    if (!f) return;

    auto root = std::make_shared<CompoundTag>();
    auto tag = std::make_shared<CompoundTag>();
    data->save(tag);
    root->put("data", tag);

    NbtIo::write(root, f);
}

void DimensionDataStorage::readMapIds()
{
    m_usedAuxIds.clear();
    if (!m_server) return;

    auto tag = NbtIo::read(m_server->m_path + "/data/idcounts.dat");
    if (tag)
    {
        for (auto& kv : tag->tags)
        {
            if (kv.second->getType() == TagType::TAG_Short)
            {
                m_usedAuxIds[kv.first] = std::dynamic_pointer_cast<ShortTag>(kv.second)->getValue();
            }
        }
    }
}
