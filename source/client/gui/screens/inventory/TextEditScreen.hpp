#pragma once

#include "client/gui/Screen.hpp"
#include <memory>
#include <world/tile/entity/SignTileEntity.hpp>

class TextEditScreen : public Screen {

public:
    TextEditScreen(std::shared_ptr<SignTileEntity>);

    void init() override;
    void render(int mouseX, int mouseY, float partialTick) override;
    void removed() override;
	
    virtual void mouseClicked(int mouseX, int mouseY, int button) override;
    virtual void keyPressed(int key) override;
    virtual void keyboardNewChar(char chr) override;
    void buttonClicked(Button* pButton) override;
    virtual void tick() override;

private:
    std::string m_title;
    int m_frame;
    int m_line;
    std::shared_ptr<SignTileEntity> m_sign;
    Button m_btnDone;
};