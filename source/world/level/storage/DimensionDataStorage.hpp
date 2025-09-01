#pragma once

#include <unordered_map>
#include <world/level/saveddata/SavedData.hpp>

class MinecraftServer;

class DimensionDataStorage {
public:
    DimensionDataStorage(MinecraftServer* server);

    template <typename T>
    std::shared_ptr<T> computeIfAbsent(const std::string& key);

    void set(const std::string& key, std::shared_ptr<SavedData> data);

    void save()
    {
        for (auto& d : m_dirtyList)
        {
            if (d->isDirty())
            {
                save(d);
                d->setDirty(false);
            }
        }
    }

    int getFreeMapId(const std::string& key);

private:
    void save(const std::shared_ptr<SavedData>& data);

    void readMapIds();

private:
    MinecraftServer* m_server;
    std::unordered_map<std::string, std::shared_ptr<SavedData>> m_cache;
    std::vector<std::shared_ptr<SavedData>> m_dirtyList;
    std::unordered_map<std::string, short> m_usedAuxIds;
};
