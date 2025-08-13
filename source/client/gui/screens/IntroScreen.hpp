#pragma once

#include "../Screen.hpp"

class IntroScreen : public Screen
{
public:
	void render(int, int, float) override;
	void updateEvents() override;
	bool isInGameScreen() override;
};

