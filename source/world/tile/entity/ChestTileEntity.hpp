#pragma once

#include <memory>
#include <array>
#include <string>
#include "TileEntity.hpp"
#include "world/level/Level.hpp"
#include "world/item/Container.hpp"
#include <world/item/ItemInstance.hpp>

class ChestTileEntity : public TileEntity, public Container {
public:
    ChestTileEntity();

    virtual int getContainerSize() override;
    virtual std::shared_ptr<ItemInstance> getItem(int index) override;
    virtual std::shared_ptr<ItemInstance> removeItem(int index, int count) override;
    virtual void setItem(int index, std::shared_ptr<ItemInstance> item) override;

    std::string getName() override;

    void load(std::shared_ptr<CompoundTag> tag) override;
    void save(std::shared_ptr<CompoundTag> tag) override;

    int getMaxStackSize() override;
    bool stillValid(Player* player) override;

    virtual void setChanged() override;

private:
    std::array<std::shared_ptr<ItemInstance>, 36> m_items;
};
