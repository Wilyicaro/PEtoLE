#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <typeinfo>
#include <iostream>
#include "Entity.hpp"

using EntityFactory = std::function<std::shared_ptr<Entity>(Level*)>;

class EntityIO {

public:
    static bool contains(const std::string& id) {
        return EntityType::getByName(id) != nullptr;
    }

    static std::shared_ptr<Entity> newEntity(const std::string& id, Level* level) {
        auto it = EntityType::getByName(id);
        return it ? it->newEntity(level) : nullptr;
    }

    static std::shared_ptr<Entity> newById(int id, Level* level) {
        auto it = EntityType::getById(id);
        if (it) return it->newEntity(level);
        LOG_I("Skipping Entity with id %d", id);
        return nullptr;
    }

    static std::shared_ptr<Entity> loadStatic(std::shared_ptr<CompoundTag> tag, Level* level) {
        std::string id = tag->contains("id") ? tag->getString("id") : "";
        auto entity = newEntity(id, level);
        if (entity) 
        {
            entity->load(tag);
        }
        else 
        {
            LOG_I("Skipping Entity with id %s", id.c_str());
        }
        return entity;
    }
};
