#pragma once

#include "../Screen.hpp"
#include "stats/StatsCounter.hpp"

class AchievementsScreen : public Screen
{
public:
    AchievementsScreen(StatsCounter* statsCounter);

    void init() override;
    void buttonClicked(Button* button) override;
    void keyPressed(int eventKey) override;
    void render(int mouseX, int mouseY, float partialTicks) override;
    void tick() override;

    bool isPauseScreen() override;

private:
    void renderTitle();
    void renderInside(int mouseX, int mouseY, float partialTicks);

    Button m_doneBtn;
    StatsCounter* m_pStatsCounter;

    int m_imageWidth = 256;
    int m_imageHeight = 202;

    int m_mouseX = 0;
    int m_mouseY = 0;

    double m_scrollX = 0.0;
    double m_scrollY = 0.0;

    double m_targetScrollX = 0.0;
    double m_targetScrollY = 0.0;

    double m_prevScrollX = 0.0;
    double m_prevScrollY = 0.0;

    int m_mouseButtonState = 0;

    const int minX, minY, maxX, maxY;
};