#pragma once

#include "../Screen.hpp"

class ChangeProfileScreen : public Screen
{
public:
	ChangeProfileScreen(const std::string& initialName);
	
	virtual void init() override;
	virtual void buttonClicked(Button* pButton) override;
	virtual void render(int x, int y, float f) override;
	virtual void onTextBoxUpdated(int id) override;

private:
	TextInputBox m_nameBox;
	Button m_btnDone;
};