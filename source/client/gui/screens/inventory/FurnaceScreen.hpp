#pragma once

#include "ContainerScreen.hpp"
#include "world/tile/entity/FurnaceTileEntity.hpp"

class FurnaceScreen : public ContainerScreen {
private:
    Inventory* m_inventory;
    std::shared_ptr<FurnaceTileEntity> m_furnace;

public:
    FurnaceScreen(Inventory* inventory, std::shared_ptr<FurnaceTileEntity> container);

protected:
    void renderLabels() override;
    void renderBg(int mouseX, int mouseY, float partialTicks) override;
};
