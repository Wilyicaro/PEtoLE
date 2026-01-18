#pragma once

#include <vector>
#include <bitset>
#include <deque>

class Minecraft;
class Toast;
class ToastInstance;
class Achievement;

#define TOAST_SLOT_COUNT 5

typedef void* ToastToken;

class ToastComponent
{
public:
    ToastComponent(Minecraft*);

    void render(int);

    void addToast(Toast* toast);

    ToastInstance* getToastInstance(ToastToken token);

    Toast* getToast(ToastToken token);

    void awardAchievement(Achievement*, bool desc = false);

    void clear();

private:
    int findFreeIndex(int);
    int freeSlots();

public:
    Minecraft* m_pMinecraft;

private:
    std::vector<ToastInstance*> m_visible;
    std::bitset<TOAST_SLOT_COUNT> m_occupiedSlots;
    std::deque<Toast*> m_queued;
};