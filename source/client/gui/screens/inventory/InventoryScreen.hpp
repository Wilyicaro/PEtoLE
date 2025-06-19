#pragma once

#include "ContainerScreen.hpp"

class InventoryScreen : public ContainerScreen {

public:

    InventoryScreen(Player* player);

protected:
    virtual void renderLabels() override;
    virtual void renderBg(int mouseX, int mouseY, float partialTick) override;
};