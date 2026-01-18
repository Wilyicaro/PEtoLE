#pragma once

#include "../../GuiComponent.hpp"
#include "ToastComponent.hpp"
#include "client/app/Minecraft.hpp"

#define DEFAULT_TOAST_WIDTH 160
#define DEFAULT_TOAST_HEIGHT 32

class Toast : public GuiComponent
{
public:
    virtual void render(ToastComponent&, int64_t timeSinceLastVisible) = 0;

    virtual int width() const
    {
        return DEFAULT_TOAST_WIDTH;
    }

    virtual int height() const
    {
        return DEFAULT_TOAST_HEIGHT;
    }

    virtual int slotCount() const
    {
        return ceilf(height() / 32.0f);
    }

    virtual ToastToken getToken() const
    {
        return NO_TOKEN;
    }

    virtual void playSound(Minecraft*);

public:
    bool m_bVisible = true;
    static ToastToken NO_TOKEN;
};

class AchievementToast : public Toast
{
public:
    AchievementToast(Achievement*, bool description = false);

    void render(ToastComponent&, int64_t timeSinceLastVisible) override;

    ToastToken getToken() const override
    {
        return TOKEN;
    }

public:
    Achievement* m_pAchievement;
    bool m_bDescription;
    static ToastToken TOKEN;
};

class SimpleToast : public Toast
{
public:
    SimpleToast(const std::string&, int timeVisible = 1800);

    void render(ToastComponent&, int64_t timeSinceLastVisible) override;

public:
    std::string m_text;
    int m_timeVisible;
};