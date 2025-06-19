#pragma once

#include <memory>
#include "ContainerMenu.hpp"
#include "world/entity/Player.hpp"
#include <world/tile/entity/FurnaceTileEntity.hpp>

class FurnaceMenu : public ContainerMenu {
private:
    std::shared_ptr<FurnaceTileEntity> m_furnace;

public:
    FurnaceMenu(Inventory* inventory, std::shared_ptr<FurnaceTileEntity> container);

    bool stillValid(Player* player) const override;
};
