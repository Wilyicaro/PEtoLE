#pragma once

#include "ContainerScreen.hpp"

class CraftingScreen : public ContainerScreen {

public:

    CraftingScreen(Inventory* inventory, const TilePos& tilePos, Level* level);

protected:
    virtual void renderLabels() override;
    virtual void renderBg(int mouseX, int mouseY, float partialTick) override;
};