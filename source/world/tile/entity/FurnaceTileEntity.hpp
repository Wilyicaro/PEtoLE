#pragma once

#include <memory>
#include <array>
#include <string>
#include <unordered_map>
#include "TileEntity.hpp"
#include "world/level/Level.hpp"
#include "world/item/Container.hpp"
#include <world/item/ItemInstance.hpp>

class FurnaceTileEntity : public TileEntity, public Container {
public:
    FurnaceTileEntity();

    virtual int getContainerSize() override;
    virtual std::shared_ptr<ItemInstance> getItem(int index) override;
    virtual std::shared_ptr<ItemInstance> removeItem(int index, int count) override;
    virtual void setItem(int index, std::shared_ptr<ItemInstance> item) override;

    std::string getName() override;

    void load(std::shared_ptr<CompoundTag> tag) override;
    void save(std::shared_ptr<CompoundTag> tag) override;

    virtual void tick() override;
    void burn();
    int getMaxStackSize() override;
    int getBurnProgress(int height);
    int getLitProgress(int height);
    bool isLit();
    bool stillValid(Player* player) override;



    virtual void setChanged() override;

    int m_litTime = 0;
    int m_litDuration = 0;
    int m_tickCount = 0;

private:
    std::array<std::shared_ptr<ItemInstance>, 3> m_items;

    static std::unordered_map<int, int> m_burnDuration;

    bool canBurn();
};
