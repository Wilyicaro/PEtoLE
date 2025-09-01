#pragma once

#include <memory>
#include <array>
#include <string>
#include "TileEntity.hpp"
#include "world/level/Level.hpp"
#include "world/item/SimpleContainer.hpp"
#include <world/item/ItemInstance.hpp>

class DispenserTileEntity : public TileEntity, public SimpleContainer {
public:
    DispenserTileEntity();

    void load(CompoundIO tag) override;
    void save(CompoundIO tag) override;

    bool stillValid(Player* player) override;

    virtual void setChanged() override;
    bool removeProjectile(int id);
    std::shared_ptr<ItemInstance> removeRandomItem();

public:
    Random m_random;
};
