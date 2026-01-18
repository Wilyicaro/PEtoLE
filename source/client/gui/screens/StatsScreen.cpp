#include "StatsScreen.hpp"
#include "client/renderer/Lighting.hpp"
#include "client/renderer/entity/ItemRenderer.hpp"

class StatListBase : public ScrolledSelectionList
{
public:
    StatListBase(StatsScreen* parent) : ScrolledSelectionList(parent->m_pMinecraft, parent->m_width, parent->m_height, 32, parent->m_height - 64, 20), m_pParent(parent)
    {
        setRenderSelection(false);
        setRenderHeader(true, 20);
    }

    int getItemCount() override
    {
        return m_statItemList.size();
    }

    void selectItem(int i, bool bl) override
    {
    }

    bool isSelectedItem(int item) override
    {
        return false;
    }

    void renderBackground(float) override
    {
        m_pParent->renderBackground();
    }

    virtual void renderHeader(int itemX, int itemY, Tesselator& tesselator) override
    {
        if (!Mouse::isButtonDown(BUTTON_LEFT))
            m_headerPressed = -1;

        if (m_headerPressed == 0)
            m_pParent->drawSlotBackground(itemX + 115 - 18 + 1, itemY + 1 + 1, 0, 0);
        else
            m_pParent->drawSlotBackground(itemX + 115 - 18, itemY + 1, 0, 18);

        if (m_headerPressed == 1)
            m_pParent->drawSlotBackground(itemX + 165 - 18 + 1, itemY + 1 + 1, 0, 0);
        else
            m_pParent->drawSlotBackground(itemX + 165 - 18, itemY + 1, 0, 18);

        if (m_headerPressed == 2)
            m_pParent->drawSlotBackground(itemX + 215 - 18 + 1, itemY + 1 + 1, 0, 0);
        else
            m_pParent->drawSlotBackground(itemX + 215 - 18, itemY + 1, 0, 18);

        if (m_sortColumn != -1)
        {
            int arrowX = 79;
            int arrowY = 18;
            if (m_sortColumn == 1) arrowX = 129;
            else if (m_sortColumn == 2) arrowX = 179;

            if (m_sortDirection == 1) arrowY = 36;

            m_pParent->drawSlotBackground(itemX + arrowX, itemY + 1, arrowY, 0);
        }
    }

    void renderTooltip(const std::string& tooltip, int mouseX, int mouseY)
    {
        if (!tooltip.empty())
        {
            int tooltipX = mouseX + 12;
            int tooltipY = mouseY - 12;
            int width = m_pParent->m_pFont->width(tooltip);
            fillGradient(tooltipX - 3, tooltipY - 3, tooltipX + width + 3, tooltipY + 8 + 3, -1073741824, -1073741824);
            m_pParent->m_pFont->drawShadow(tooltip, tooltipX, tooltipY, -1);
        }
    }

    void renderDecorations(int mouseX, int mouseY) override
    {
        if (mouseY >= m_y0 && mouseY <= m_y1)
        {
            int index = getItemAtPosition(mouseX, mouseY);
            int left = m_pParent->m_width / 2 - 92 - 16;
            if (index >= 0)
            {
                if (mouseX < left + 40 || mouseX > left + 40 + 20)
                    return;

                IdStat* idStat = m_statItemList[index];

                if (idStat)
                    renderTooltip(Language::getInstance()->get(Item::items[idStat->getTypeId()]->getDescriptionId()), mouseX, mouseY);
            }
            else
            {
                if (mouseX >= left + 115 - 18 && mouseX <= left + 115)
                    renderTooltip(Language::getInstance()->get(getHeaderName(0)), mouseX, mouseY);
                else if (mouseX >= left + 165 - 18 && mouseX <= left + 165)
                    renderTooltip(Language::getInstance()->get(getHeaderName(1)), mouseX, mouseY);
                else if (mouseX >= left + 215 - 18 && mouseX <= left + 215)
                    renderTooltip(Language::getInstance()->get(getHeaderName(2)), mouseX, mouseY);
            }

        }
    }

    void mouseClicked(int mouseX, int mouseY) override
    {
        ScrolledSelectionList::mouseClicked(mouseX, mouseY);

        m_headerPressed = -1;

        if (mouseX >= 79 && mouseX < 115)
            m_headerPressed = 0;
        else if (mouseX >= 129 && mouseX < 165)
            m_headerPressed = 1;
        else if (mouseX >= 179 && mouseX < 215)
            m_headerPressed = 2;

        if (m_headerPressed >= 0)
        {
            sortByColumn(m_headerPressed);
            m_pMinecraft->m_pSoundEngine->playUI("random.click", 1.0f, 1.0f);
        }
    }

    void sortByColumn(int column)
    {
        if (column != m_sortColumn)
        {
            m_sortColumn = column;
            m_sortDirection = -1;
        }
        else if (m_sortDirection == -1)
            m_sortDirection = 1;
        else
        {
            m_sortColumn = -1;
            m_sortDirection = 0;
        }

        std::sort(m_statItemList.begin(), m_statItemList.end(), m_itemStatSorter);
    }

    void renderStat(Stat* stat, int x, int y, bool evenRow)
    {
        std::string value = stat ? stat->format(m_pParent->m_pStatsCounter->getStatValue(stat)) : "-";
        m_pParent->m_pFont->drawShadow(value, x - m_pParent->m_pFont->width(value), y + 5, evenRow ? 0xFFFFFF : 0x909090);
    }

protected:
    StatsScreen* m_pParent;
    int m_headerPressed = -1;
    std::vector<IdStat*> m_statItemList;
    std::function<bool(IdStat*, IdStat*)> m_itemStatSorter;
    int m_sortColumn = -1;
    int m_sortDirection = 0;

    virtual std::string getHeaderName(int column) = 0;
};

class GeneralStatisticsList : public ScrolledSelectionList
{
public:
    GeneralStatisticsList(StatsScreen* parent) : ScrolledSelectionList(parent->m_pMinecraft, parent->m_width, parent->m_height, 32, parent->m_height - 64, 10), m_pParent(parent)
    {
        setRenderSelection(false);
    }
   
    void selectItem(int i, bool bl) override
    {
    }

    bool isSelectedItem(int item) override
    {
        return false;
    }

    int getItemCount() override
    {
        return Stats::customStats.size();
    }

    int getMaxPosition() override
    {
        return getItemCount() * 10;
    }

    void renderBackground(float) override
    {
        m_pParent->renderBackground();
    }

    void renderItem(int index, int x, int y, int entryWidth, Tesselator& tesselator) override
    {
        Stat* stat = Stats::customStats[index];

        m_pParent->m_pFont->draw(stat->getName(), x + 2, y + 1, index % 2 == 0 ? 0xFFFFFF : 0x909090);

        std::string value = stat->format(m_pParent->m_pStatsCounter->getStatValue(stat));
        m_pParent->m_pFont->draw(value, x + 2 + 213 - m_pParent->m_pFont->width(value), y + 1, index % 2 == 0 ? 0xFFFFFF : 0x909090);
    }

private:
    StatsScreen* m_pParent;
};

class BlockStatisticsList : public StatListBase
{
public:
    BlockStatisticsList(StatsScreen* parent) : StatListBase(parent)
    {
        for (IdStat* stat : Stats::tileStats)
        {
            int id = stat->getTypeId();
            bool hasStat = parent->m_pStatsCounter->getStatValue(stat) > 0 ||
                (Stats::statItemBreak[id] && parent->m_pStatsCounter->getStatValue(Stats::statItemBreak[id]) > 0) ||
                (Stats::itemCrafted[id] && parent->m_pStatsCounter->getStatValue(Stats::itemCrafted[id]) > 0);

            if (hasStat)
                m_statItemList.push_back(stat);
        }

        m_itemStatSorter = [this, parent](IdStat* a, IdStat* b) {
            int idA = a->getTypeId();
            int idB = b->getTypeId();

            Stat* statA = nullptr;
            Stat* statB = nullptr;

            if (m_sortColumn == 0)
            {
                statA = Stats::itemCrafted[idA];
                statB = Stats::itemCrafted[idB];
            }
            else if (m_sortColumn == 1)
            {
                statA = Stats::statItemUsed[idA];
                statB = Stats::statItemUsed[idB];
            }
            else if (m_sortColumn == 2)
            {
                statA = Stats::statMineBlock[idA];
                statB = Stats::statMineBlock[idB];
            }

            if (statA || statB)
            {
                if (!statA) return true;
                if (!statB) return false;

                int valueA = parent->m_pStatsCounter->getStatValue(statA);
                int valueB = parent->m_pStatsCounter->getStatValue(statB);

                if (valueA != valueB) {
                    return (valueA - valueB) * m_sortDirection > 0;
                }
            }

            return idA - idB > 0;
            };
    }

    virtual void renderHeader(int itemX, int itemY, Tesselator& tesselator) override
    {
        StatListBase::renderHeader(itemX, itemY, tesselator);
        if (m_headerPressed == 0)
            m_pParent->drawSlotBackground(itemX + 115 - 18 + 1, itemY + 1 + 1, 18, 18);
        else
            m_pParent->drawSlotBackground(itemX + 115 - 18, itemY + 1, 18, 18);

        if (m_headerPressed == 1)
            m_pParent->drawSlotBackground(itemX + 165 - 18 + 1, itemY + 1 + 1, 36, 18);
        else
            m_pParent->drawSlotBackground(itemX + 165 - 18, itemY + 1, 36, 18);

        if (m_headerPressed == 2)
            m_pParent->drawSlotBackground(itemX + 215 - 18 + 1, itemY + 1 + 1, 54, 18);
        else
            m_pParent->drawSlotBackground(itemX + 215 - 18, itemY + 1, 54, 18);
    }

    std::string getHeaderName(int column) override
    {
        return column == 0 ? "stat.crafted" :
            column == 1 ? "stat.used" :
            "stat.mined";
    }

    void renderItem(int index, int x, int y, int entryWidth, Tesselator& tesselator) override
    {
        IdStat* stat = m_statItemList[index];
        int id = stat->getTypeId();

        m_pParent->drawItemSlot(x + 40, y, id);

        renderStat(Stats::itemCrafted[id], x + 115, y, index % 2 == 0);
        renderStat(Stats::statItemUsed[id], x + 165, y, index % 2 == 0);
        renderStat(stat, x + 215, y, index % 2 == 0);
    }
};

class ItemStatisticsList : public StatListBase
{
public:
    ItemStatisticsList(StatsScreen* parent) : StatListBase(parent)
    {
        for (IdStat* stat : Stats::itemStats)
        {
            int id = stat->getTypeId();
            bool hasStat = parent->m_pStatsCounter->getStatValue(stat) > 0 ||
                (Stats::statItemUsed[id] && parent->m_pStatsCounter->getStatValue(Stats::statItemUsed[id]) > 0) ||
                (Stats::itemCrafted[id] && parent->m_pStatsCounter->getStatValue(Stats::itemCrafted[id]) > 0);

            if (hasStat) {
                m_statItemList.push_back(stat);
            }
        }

        m_itemStatSorter = [this, parent](IdStat* a, IdStat* b) {
            int idA = a->getTypeId();
            int idB = b->getTypeId();

            Stat* statA = nullptr;
            Stat* statB = nullptr;

            if (m_sortColumn == 0)
            {
                statA = Stats::statItemBreak[idA];
                statB = Stats::statItemBreak[idB];
            }
            else if (m_sortColumn == 1)
            {
                statA = Stats::itemCrafted[idA];
                statB = Stats::itemCrafted[idB];
            }
            else if (m_sortColumn == 2)
            {
                statA = Stats::statItemUsed[idA];
                statB = Stats::statItemUsed[idB];
            }

            if (statA || statB)
            {
                if (!statA) return true;
                if (!statB) return false;

                int valueA = parent->m_pStatsCounter->getStatValue(statA);
                int valueB = parent->m_pStatsCounter->getStatValue(statB);

                if (valueA != valueB) {
                    return ((valueA - valueB) * m_sortDirection) > 0;
                }
            }

            return idA - idB > 0;
            };
    }

    virtual void renderHeader(int itemX, int itemY, Tesselator& tesselator) override
    {
        StatListBase::renderHeader(itemX, itemY, tesselator);
        if (m_headerPressed == 0)
            m_pParent->drawSlotBackground(itemX + 115 - 18 + 1, itemY + 1 + 1, 72, 18);
        else
            m_pParent->drawSlotBackground(itemX + 115 - 18, itemY + 1, 72, 18);

        if (m_headerPressed == 1)
            m_pParent->drawSlotBackground(itemX + 165 - 18 + 1, itemY + 1 + 1, 18, 18);
        else
            m_pParent->drawSlotBackground(itemX + 165 - 18, itemY + 1, 18, 18);

        if (m_headerPressed == 2)
            m_pParent->drawSlotBackground(itemX + 215 - 18 + 1, itemY + 1 + 1, 36, 18);
        else
            m_pParent->drawSlotBackground(itemX + 215 - 18, itemY + 1, 36, 18);
    }

    std::string getHeaderName(int column) override
    {
        return column == 0 ? "stat.depleted" :
            column == 1 ? "stat.crafted" :
            "stat.used";
    }

    void renderItem(int index, int x, int y, int entryWidth, Tesselator& tesselator) override
    {
        IdStat* stat = m_statItemList[index];
        int id = stat->getTypeId();

        m_pParent->drawItemSlot(x + 40, y, id);

        renderStat(Stats::statItemBreak[id], x + 115, y, index % 2 == 0);
        renderStat(Stats::itemCrafted[id], x + 165, y, index % 2 == 0);
        renderStat(stat, x + 215, y, index % 2 == 0);
    }
};

StatsScreen::StatsScreen(Screen* lastScreen, StatsCounter* stats) :
    m_pLastScreen(lastScreen),
    m_pStatsCounter(stats),
    m_pRenderItemInstance(std::make_shared<ItemInstance>(0, 0, 0)),
    m_doneBtn(0, 0, 0, 150, 20, ""),
    m_generalBtn(1, 0, 0, 100, 20, ""),
    m_blocksBtn(2, 0, 0, 100, 20, ""),
    m_itemsBtn(3, 0, 0, 100, 20, "")
{
    m_bDeletePrevious = false;
}

void StatsScreen::init()
{
    m_pActiveList = nullptr;
    m_pGeneralStatsList = std::make_unique<GeneralStatisticsList>(this);
    m_pBlockStatsList = std::make_unique<BlockStatisticsList>(this);
    m_pItemStatsList = std::make_unique<ItemStatisticsList>(this);

    m_pActiveList = m_pGeneralStatsList.get();

    initButtons();
}

void StatsScreen::initButtons()
{
    m_doneBtn.m_text = Language::getInstance()->get("gui.done");
    m_generalBtn.m_text = Language::getInstance()->get("stat.generalButton");
    m_blocksBtn.m_text = Language::getInstance()->get("stat.blocksButton");
    m_itemsBtn.m_text = Language::getInstance()->get("stat.itemsButton");

    m_doneBtn.m_xPos = m_width / 2 + 4;
    m_generalBtn.m_xPos = m_width / 2 - 154;
    m_blocksBtn.m_xPos = m_width / 2 - 46;
    m_itemsBtn.m_xPos = m_width / 2 + 62;

    m_doneBtn.m_yPos = m_height - 28;
    m_generalBtn.m_yPos = m_blocksBtn.m_yPos = m_itemsBtn.m_yPos = m_height - 52;

    m_buttons.push_back(&m_doneBtn);
    m_buttons.push_back(&m_generalBtn);
    m_buttons.push_back(&m_blocksBtn);
    m_buttons.push_back(&m_itemsBtn);

    if (m_pItemStatsList->getItemCount() == 0)
        m_itemsBtn.m_bEnabled = false;
    if (m_pBlockStatsList->getItemCount() == 0)
        m_blocksBtn.m_bEnabled = false;
}

void StatsScreen::buttonClicked(Button* button)
{
    if (!button->m_bEnabled) return;

    if (button->m_buttonId == m_doneBtn.m_buttonId)
        m_pMinecraft->setScreen(m_pLastScreen);
    else if (button->m_buttonId == m_generalBtn.m_buttonId)
        m_pActiveList = m_pGeneralStatsList.get();
    else if (button->m_buttonId == m_blocksBtn.m_buttonId)
        m_pActiveList = m_pBlockStatsList.get();
    else if (button->m_buttonId == m_itemsBtn.m_buttonId)
        m_pActiveList = m_pItemStatsList.get();
}

void StatsScreen::render(int mouseX, int mouseY, float partialTicks)
{
    if (m_pActiveList)
        m_pActiveList->render(mouseX, mouseY, partialTicks);

    drawCenteredString(m_pFont, "Statistics", m_width / 2, 20, 0xFFFFFF);

    Screen::render(mouseX, mouseY, partialTicks);
}

void StatsScreen::drawItemSlot(int x, int y, int itemId)
{
    drawSlotBackground(x + 1, y + 1);
    glEnable(GL_RESCALE_NORMAL);
    glPushMatrix();
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    Lighting::turnOn();
    glPopMatrix();

    m_pRenderItemInstance->m_itemID = itemId;
    ItemRenderer::renderGuiItem(m_pMinecraft->m_pTextures, m_pRenderItemInstance, x + 2, y + 2);

    Lighting::turnOff();
    glDisable(GL_RESCALE_NORMAL);
}

void StatsScreen::drawSlotBackground(int x, int y, int u, int v)
{
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/slot.png");
    blit(x, y, u, v, 18, 18, 128, 128);
}