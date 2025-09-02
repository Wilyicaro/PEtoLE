#pragma once

#include <vector>
#include <unordered_set>
#include <memory>
#include <cstdint>

class Player;
class ItemInstance;
class Inventory;
class Slot;
class Container;
class ContainerListener;

class ContainerMenu {
protected:
    std::vector<std::shared_ptr<ItemInstance>> lastSlots;
    uint16_t changeUid = 0;
    std::vector<std::shared_ptr<ContainerListener>> listeners;
    std::unordered_set<Player*> unsynchedPlayers;

public:
    int containerId = 0;
    std::vector<std::shared_ptr<Slot>> slots;
    virtual ~ContainerMenu() = default;

    void addSlot(std::shared_ptr<Slot> slot);
    void addSlotListener(std::shared_ptr<ContainerListener> listener);
    void sendData(int id, int value);
    virtual void broadcastChanges();
    virtual void removed(Player* player);
    virtual void slotsChanged(Container* container);

    Slot* getSlotFor(Container* container, int index);
    Slot* getSlot(int index);
    std::shared_ptr<ItemInstance> clicked(int slotIndex, int mouseButton, Player* player);

    void setItem(int slotIndex, std::shared_ptr<ItemInstance> item);
    void setAll(const std::vector<std::shared_ptr<ItemInstance>>& items);
    void setData(int id, int value);

    uint16_t backup(Inventory* inventory);
    void deleteBackup(uint16_t id);
    void rollbackToBackup(uint16_t id);

    bool isSynched(Player* player) const;
    void setSynched(Player* player, bool isSynched);

    virtual bool stillValid(Player* player) const = 0;
    virtual bool isPauseScreen() const { return false; }
};
