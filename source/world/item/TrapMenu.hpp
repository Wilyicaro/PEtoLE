#pragma once

#include <memory>
#include "ContainerMenu.hpp"
#include "world/tile/entity/DispenserTileEntity.hpp"
#include "world/entity/Player.hpp"

class TrapMenu : public ContainerMenu {
private:
    std::shared_ptr<DispenserTileEntity> m_trap;

public:
    TrapMenu(Container* inventory, std::shared_ptr<DispenserTileEntity> trap);

    bool stillValid(Player* player) const override;
    virtual std::shared_ptr<ItemInstance> quickMoveStack(int index) override;
};
