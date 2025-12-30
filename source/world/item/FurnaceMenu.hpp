#pragma once

#include <memory>
#include "ContainerMenu.hpp"
#include "world/entity/Player.hpp"
#include <world/tile/entity/FurnaceTileEntity.hpp>

class FurnaceMenu : public ContainerMenu {
private:
    std::shared_ptr<FurnaceTileEntity> m_furnace;
    int m_lastCookTime = 0;
    int m_lastBurnTime = 0;
    int m_lastLitDuration = 0;

public:
    FurnaceMenu(Inventory* inventory, std::shared_ptr<FurnaceTileEntity> container);

    virtual bool stillValid(Player* player) const override;

    virtual void addSlotListener(std::shared_ptr<ContainerListener>) override;
    virtual void broadcastChanges() override;
    virtual void setData(int, int) override;
    virtual std::shared_ptr<ItemInstance> quickMoveStack(int index) override;
};
