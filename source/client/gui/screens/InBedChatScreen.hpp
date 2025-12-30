#pragma once

#include "ChatScreen.hpp"

class InBedChatScreen : public ChatScreen
{
public:
	InBedChatScreen();
	virtual void init() override;
	virtual void buttonClicked(Button*) override;
	virtual void onClose() override;
	virtual void tick() override;
	void sendWakeUp();

private:
	Button m_btnWake;
};

