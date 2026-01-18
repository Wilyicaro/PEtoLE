#pragma once

#include "Toast.hpp"

#define TOAST_ANIMATION_TIME 600

class ToastInstance
{
public:
	ToastInstance(Toast*, ToastComponent*, int index, int slotCount);
	~ToastInstance();

	bool render(int);

	Toast* getToast()
	{
		return m_pToast;
	}

private:
	float getVisibility(int64_t);

public:
	const int m_index;
	const int m_slotCount;
	int64_t m_animationTime;
	int64_t m_visibleTime;
	bool m_bVisible;

private:
	Toast* m_pToast;
	ToastComponent* m_pToastComponent;
};