#include "ToastComponent.hpp"
#include "ToastInstance.hpp"
#include "client/app/Minecraft.hpp"
#include "stats/Achievement.hpp"

ToastComponent::ToastComponent(Minecraft* minecraft) : m_pMinecraft(minecraft)
{
}

void ToastComponent::render(int screenWidth)
{
	if (!m_pMinecraft->getOptions()->m_bHideGui)
	{

		for (auto it = m_visible.begin(); it != m_visible.end();)
		{
			ToastInstance* instance = *it;
			if (instance && instance->render(screenWidth))
			{
				for (int i = instance->m_index; i < instance->m_index + instance->m_slotCount; ++i)
					m_occupiedSlots.reset(i);
				it = m_visible.erase(it);
				delete instance;
			}
			else
				++it;
		}

		if (!m_queued.empty() && freeSlots() > 0)
		{
			for (auto it = m_queued.begin(); it != m_queued.end();)
			{
				Toast* toast = *it;
				int slotCount = toast->slotCount();
				int freeIndex = findFreeIndex(slotCount);
				if (freeIndex != -1)
				{
					m_visible.push_back(new ToastInstance(toast, this, freeIndex, slotCount));
					for (int i = freeIndex; i < freeIndex + slotCount; ++i)
						m_occupiedSlots.set(i);
					it = m_queued.erase(it);
				}
				else
					++it;
			}
		}
	}
}

void ToastComponent::addToast(Toast* toast)
{
	m_queued.push_back(toast);
}

ToastInstance* ToastComponent::getToastInstance(ToastToken token)
{
	for (ToastInstance* instance : m_visible)
	{
		if (instance->getToast()->getToken() == token)
			return instance;
	}

	return nullptr;
}

Toast* ToastComponent::getToast(ToastToken token)
{
	ToastInstance* instance = getToastInstance(token);

	if (instance)
		return instance->getToast();

	for (Toast* toast : m_queued)
	{
		if (toast->getToken() == token)
			return toast;
	}

	return nullptr;
}

void ToastComponent::awardAchievement(Achievement* achievement, bool desc)
{
	AchievementToast* old = dynamic_cast<AchievementToast*>(getToast(AchievementToast::TOKEN));
	if (old)
	{
		bool same = old->m_pAchievement == achievement && old->m_bDescription == desc;
		old->m_pAchievement = achievement;
		old->m_bDescription = desc;
		ToastInstance* instance = getToastInstance(AchievementToast::TOKEN);
		if (instance)
		{
			if (same)
				instance->m_animationTime = TOAST_ANIMATION_TIME;
			else
				instance->m_animationTime = instance->m_visibleTime = -1;
			instance->m_bVisible = old->m_bVisible = true;
		}
	}
	else
		addToast(new AchievementToast(achievement, desc));
}

void ToastComponent::clear()
{
	for (auto it = m_visible.begin(); it != m_visible.end();)
	{
		ToastInstance* instance = *it;
		it = m_visible.erase(it);
		delete instance;
	}
	m_occupiedSlots.reset();
	m_queued.clear();
}

int ToastComponent::findFreeIndex(int slotCount)
{
	if (freeSlots() >= slotCount)
	{
		int freeIndex = 0;
		for (int i = 0; i < TOAST_SLOT_COUNT; ++i)
		{
			if (m_occupiedSlots[i])
				freeIndex = 0;
			else if (++freeIndex == slotCount)
				return i + 1 - freeIndex;	
		}
	}
	return -1;
}

int ToastComponent::freeSlots()
{
	return TOAST_SLOT_COUNT - m_occupiedSlots.count();
}
