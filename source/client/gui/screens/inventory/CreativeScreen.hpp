#pragma once

#include "ContainerScreen.hpp"

class CreativeScreen : public ContainerScreen {
private:
    static Container* creativeGrid;

public:
    CreativeScreen(Container* inventory);
    void mouseClicked(int x, int y, int button) override;
    void mouseScrolled(double x, double y, int scroll) override;
    void mouseReleased(int x, int y, int button) override;
    void mouseDragged(double x, double y, int button, double deltaX, double deltaY) override;
    void slotClicked(Slot* slot, int index, int button, bool quick) override;

    void tick() override;

protected:
    void renderLabels() override;
    void renderBg(int mouseX, int mouseY, float partialTicks) override;

private:
    float m_scrolled = 0.0f;
    bool m_bIsScrolling;
};
