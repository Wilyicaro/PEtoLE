#pragma once

#include "client/gui/Screen.hpp"
#include <memory>
#include <world/item/ContainerMenu.hpp>
#include <world/item/Slot.hpp>

class ContainerScreen : public Screen {
protected:
    int m_imageWidth = 176;
    int m_imageHeight = 166;
    int m_leftPos;
    int m_topPos;

public:
    ContainerMenu* menu;

    ContainerScreen(ContainerMenu* menu);

    void init() override;
    void render(int mouseX, int mouseY, float partialTick) override;
    void removed() override;
    bool isPauseScreen() override;

    virtual void slotsChanged(Container* container);
	
    virtual void mouseClicked(int mouseX, int mouseY, int button) override;
    virtual void mouseReleased(int mouseX, int mouseY, int button) override;
    virtual void mouseDragged(double x, double y, int button, double deltaX, double deltaY) override;
    virtual void slotClicked(int mouseX, int mouseY, int button);
    virtual void slotClicked(Slot* slot, int index, int button, bool quick);
    virtual void keyPressed(int key) override;

protected:
    virtual void renderLabels() = 0;
    virtual void renderBg(int mouseX, int mouseY, float partialTick) = 0;

private:
    void renderSlot(Slot* slot);
    Slot* findSlot(int mouseX, int mouseY);
    bool isHovering(Slot* slot, int mouseX, int mouseY) const;

    int m_timeSlotDragged = 0;
};