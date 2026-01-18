#include "ToastInstance.hpp"

ToastInstance::ToastInstance(Toast* toast, ToastComponent* toastComponent, int index, int slotCount) : 
	m_pToast(toast), 
	m_pToastComponent(toastComponent), 
	m_index(index), 
	m_slotCount(slotCount), 
	m_animationTime(-1), 
	m_visibleTime(-1),
	m_bVisible(true)
{
}

ToastInstance::~ToastInstance()
{
	delete m_pToast;
}

bool ToastInstance::render(int screenWidth)
{
	int64_t millis = getMillis();
	if (m_animationTime == -1L)
	{
		m_animationTime = millis;
		m_pToast->playSound(m_pToastComponent->m_pMinecraft);
	}
	if (m_bVisible && millis - m_animationTime <= TOAST_ANIMATION_TIME)
		m_visibleTime = millis;
	glPushMatrix();
	glTranslatef(screenWidth - m_pToast->width(), -m_pToast->height() + (m_pToast->height() + m_index * DEFAULT_TOAST_HEIGHT) * getVisibility(millis), 800.0F);
	m_pToast->render(*m_pToastComponent, millis - m_visibleTime);
	glPopMatrix();
	if (m_pToast->m_bVisible != m_bVisible)
	{
		m_animationTime = millis - int64_t((1.0F - getVisibility(millis)) * TOAST_ANIMATION_TIME);
		m_bVisible = m_pToast->m_bVisible;
		m_pToast->playSound(m_pToastComponent->m_pMinecraft);
	}
	return !m_pToast->m_bVisible && millis - m_animationTime > TOAST_ANIMATION_TIME;
}

float ToastInstance::getVisibility(int64_t time)
{
	float clamped = Mth::clamp(float(time - m_animationTime) / TOAST_ANIMATION_TIME, 0.0F, 1.0F);
	clamped *= clamped;
	return !m_bVisible ? 1.0F - clamped : clamped;
}
