#pragma once

#include "../Screen.hpp"

class EditWorldScreen : public Screen
{
public:
	EditWorldScreen(Screen* parent, const LevelSummary& summary);
	
	virtual void init() override;
	virtual void buttonClicked(Button* pButton) override;
	virtual void render(int x, int y, float f) override;
	virtual void onClose() override;

private:
	Screen* m_pParent;
	LevelSummary m_summary;
	TextInputBox m_nameBox;
	Button m_btnDone;
	Button m_btnCancel;
};