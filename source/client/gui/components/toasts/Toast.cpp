#include "Toast.hpp"
#include "stats/Achievement.hpp"
#include "client/renderer/Lighting.hpp"
#include "client/renderer/entity/ItemRenderer.hpp"

ToastToken Toast::NO_TOKEN = new ToastToken;

void Toast::playSound(Minecraft*)
{
}

ToastToken AchievementToast::TOKEN = new ToastToken;

AchievementToast::AchievementToast(Achievement* achivement, bool description) : m_pAchievement(achivement), m_bDescription(description)
{
}

void AchievementToast::render(ToastComponent& component, int64_t timeSinceLastVisible)
{
	component.m_pMinecraft->m_pTextures->loadAndBindTexture("achievement/bg.png");
	glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	blit(0, 0, 96, 202, 160, 32, 256, 256);
	if (m_bDescription)
		component.m_pMinecraft->m_pFont->drawWordWrap(m_pAchievement->getDescription(), 30, 7, -1, 120);
	else
	{
		component.m_pMinecraft->m_pFont->draw(Language::getInstance()->get("achievement.get"), 30, 7, -256);
		component.m_pMinecraft->m_pFont->draw(m_pAchievement->getName(), 30, 18, -1);
	}

	glPushMatrix();
	glRotatef(180.0F, 1.0F, 0.0F, 0.0F);
	Lighting::turnOn();
	glPopMatrix();
	glDisable(GL_LIGHTING);
	glEnable(GL_RESCALE_NORMAL);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	ItemRenderer::renderGuiItem(component.m_pMinecraft->m_pTextures, m_pAchievement->m_icon, 8, 8);
	glDisable(GL_LIGHTING);
	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);

	if (timeSinceLastVisible >= 1800L)
		m_bVisible = false;
}

SimpleToast::SimpleToast(const std::string& text, int timeVisible) : m_text(text), m_timeVisible(timeVisible)
{
}

void SimpleToast::render(ToastComponent& component, int64_t timeSinceLastVisible)
{
	component.m_pMinecraft->m_pTextures->loadAndBindTexture("achievement/bg.png");
	glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	blit(0, 0, 96, 202, 160, 32, 256, 256);
	component.m_pMinecraft->m_pFont->drawWordWrap(m_text, 5, 7, -1, 120);

	if (timeSinceLastVisible >= m_timeVisible)
		m_bVisible = false;
}
