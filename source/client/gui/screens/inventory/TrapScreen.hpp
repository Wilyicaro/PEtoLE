#pragma once

#include "ContainerScreen.hpp"
#include "world/tile/entity/DispenserTileEntity.hpp"

class TrapScreen : public ContainerScreen {
private:
    Container* m_inventory;
    std::shared_ptr<DispenserTileEntity> m_trap;

public:
    TrapScreen(Container* inventory, std::shared_ptr<DispenserTileEntity> trap);

protected:
    void renderLabels() override;
    void renderBg(int mouseX, int mouseY, float partialTicks) override;
};
