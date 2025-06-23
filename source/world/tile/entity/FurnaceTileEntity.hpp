#pragma once

#include <memory>
#include <array>
#include <string>
#include <unordered_map>
#include "TileEntity.hpp"
#include "world/level/Level.hpp"
#include "world/item/SimpleContainer.hpp"
#include <world/item/ItemInstance.hpp>

class FurnaceTileEntity : public SimpleContainer, public TileEntity {
public:
    FurnaceTileEntity();

    void load(std::shared_ptr<CompoundTag> tag) override;
    void save(std::shared_ptr<CompoundTag> tag) override;

    virtual void tick() override;
    void burn();
    int getBurnProgress(int height);
    int getLitProgress(int height);
    bool isLit();
    bool stillValid(Player* player) override;



    virtual void setChanged() override;

    int m_litTime = 0;
    int m_litDuration = 0;
    int m_tickCount = 0;

private:
    bool canBurn();
};
