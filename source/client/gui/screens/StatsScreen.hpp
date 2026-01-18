#pragma once

#include "../Screen.hpp"
#include "../components/ScrolledSelectionList.hpp"

class StatsScreen : public Screen
{
public:
    StatsScreen(Screen* lastScreen, StatsCounter* stats);

    void init() override;
    void buttonClicked(Button* button) override;
    void render(int mouseX, int mouseY, float partialTicks) override;
    void drawItemSlot(int x, int y, int itemId);
    void drawSlotBackground(int x, int y, int u = 0, int v = 0);

private:
    void initButtons();

public:
    StatsCounter* m_pStatsCounter;
    Button m_doneBtn;
    Button m_generalBtn;
    Button m_blocksBtn;
    Button m_itemsBtn;
   
private:
    Screen* m_pLastScreen;

    std::unique_ptr<ScrolledSelectionList> m_pGeneralStatsList;
    std::unique_ptr<ScrolledSelectionList> m_pBlockStatsList;
    std::unique_ptr<ScrolledSelectionList> m_pItemStatsList;
    std::shared_ptr<ItemInstance> m_pRenderItemInstance;

    ScrolledSelectionList* m_pActiveList = nullptr;
};