#pragma once

#include "ContainerScreen.hpp"

class ChestScreen : public ContainerScreen {
private:
    Container* m_inventory;
    Container* m_container;
    int m_containerRows = 0;

public:
    ChestScreen(Container* inventory, Container* container);

protected:
    void renderLabels() override;
    void renderBg(int mouseX, int mouseY, float partialTicks) override;
};
