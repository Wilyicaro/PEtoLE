#include "IntroScreen.hpp"
#include "StartMenuScreen.hpp"

bool IntroScreen::isInGameScreen()
{
	return false;
}

void IntroScreen::render(int a, int b, float c)
{
	fill(0, 0, m_width, m_height, 0xFFFFFFFF);
	m_pMinecraft->m_pTextures->loadAndBindTexture("title/mojang.png");
	blit((m_width - 256) / 2, (m_height - 256) / 2, 0, 0, 256, 256);
}

void IntroScreen::updateEvents()
{
	if (m_pMinecraft->m_async.empty())
		m_pMinecraft->setScreen(new StartMenuScreen);
}
