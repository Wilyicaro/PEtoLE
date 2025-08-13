#include "ContainerMenu.hpp"
#include "Slot.hpp"
#include "ItemInstance.hpp"
#include "Inventory.hpp"
#include "Container.hpp"
#include "ContainerListener.hpp"

void ContainerMenu::addSlot(std::shared_ptr<Slot> slot) {
    slot->index = slots.size();
    slots.push_back(slot);
    lastSlots.emplace_back();
}

void ContainerMenu::addSlotListener(std::shared_ptr<ContainerListener> listener) {
    listeners.push_back(listener);

    std::vector<std::shared_ptr<ItemInstance>> snapshot;
    for (auto& slot : slots) {
        snapshot.push_back(slot->getItem());
    }

    listener->refreshContainer(this, snapshot);
    broadcastChanges();
}

void ContainerMenu::sendData(int id, int value) {
    for (auto& listener : listeners) {
        listener->setContainerData(this, id, value);
    }
}

void ContainerMenu::broadcastChanges() {
    for (size_t i = 0; i < slots.size(); ++i) {
        std::shared_ptr<ItemInstance> current = slots[i]->getItem();
        if (!ItemInstance::matches(lastSlots[i], current)) {
            lastSlots[i] = current;
            for (auto& listener : listeners) {
                listener->slotChanged(this, i, lastSlots[i]);
            }
        }
    }
}

void ContainerMenu::removed(Player* player) {
    Inventory* inv = player->m_pInventory;
    if (inv->getCarried()) {
        player->drop(inv->getCarried());
        inv->setCarried(nullptr);
    }
}

void ContainerMenu::slotsChanged(Container*) {
    broadcastChanges();
}

Slot* ContainerMenu::getSlotFor(Container* container, int index) {
    for (auto& slot : slots) {
        if (slot->isAt(container, index)) {
            return slot.get();
        }
    }
    return nullptr;
}

Slot* ContainerMenu::getSlot(int index) {
    return slots[index].get();
}

std::shared_ptr<ItemInstance> ContainerMenu::clicked(int slotIndex, int mouseButton, Player* player) {
    std::shared_ptr<ItemInstance> result = nullptr;

    if (mouseButton == 0 || mouseButton == 1) {
        Inventory* inv = player->m_pInventory;

        if (slotIndex == -999) {
            if (inv->getCarried()) {
                if (mouseButton == 0) {
                    player->drop(inv->getCarried());
                    inv->setCarried(nullptr);
                }
                else if (mouseButton == 1) {
                    std::shared_ptr<ItemInstance> single = inv->getCarried()->remove(1);
                    player->drop(single);
                    if (!inv->getCarried()->m_count) {
                        inv->setCarried(nullptr);
                    }
                }
            }
        }
        else {
            Slot* slot = getSlot(slotIndex);
            if (slot) {
                slot->setChanged();
                std::shared_ptr<ItemInstance> slotItem = slot->getItem();
                if (slotItem) {
                    result = slotItem->copy();
                }

                std::shared_ptr<ItemInstance> carried = inv->getCarried();

                if (slotItem || carried) {
                    int count;
                    if (slotItem && !carried) {
                        count = (mouseButton == 0) ? slotItem->m_count : (slotItem->m_count + 1) / 2;
                        inv->setCarried(slot->remove(count));
                        if (!slotItem->m_count) {
                            slot->set(nullptr);
                        }
                        slot->onTake(inv->getCarried());
                    }
                    else if (!slotItem && carried && slot->mayPlace(carried)) {
                        count = (mouseButton == 0) ? carried->m_count : 1;
                        if (count > slot->getMaxStackSize()) {
                            count = slot->getMaxStackSize();
                        }

                        slot->set(carried->remove(count));
                        if (!carried->m_count) {
                            inv->setCarried(nullptr);
                        }
                    }
                    else if (slotItem && carried) {
                        if (slot->mayPlace(carried)) {
                            if (slotItem->m_itemID != carried->m_itemID || (slotItem->isStackedByData() && slotItem->getAuxValue() != carried->getAuxValue())) {
                                if (carried->m_count <= slot->getMaxStackSize()) {
                                    slot->set(carried);
                                    inv->setCarried(slotItem);
                                }
                            }
                            else if (slotItem->m_itemID == carried->m_itemID) {
                                if (mouseButton == 0) {
                                    count = carried->m_count;
                                    int space = slot->getMaxStackSize() - slotItem->m_count;
                                    if (count > space) count = space;

                                    int maxStack = carried->getMaxStackSize() - slotItem->m_count;
                                    if (count > maxStack) count = maxStack;

                                    carried->remove(count);
                                    if (!carried->m_count) {
                                        inv->setCarried(nullptr);
                                    }
                                    slotItem->m_count += count;
                                }
                                else if (mouseButton == 1) {
                                    count = 1;
                                    int space = slot->getMaxStackSize() - slotItem->m_count;
                                    if (count > space) count = space;

                                    int maxStack = carried->getMaxStackSize() - slotItem->m_count;
                                    if (count > maxStack) count = maxStack;

                                    carried->remove(count);
                                    if (carried->m_count == 0) {
                                        inv->setCarried(nullptr);
                                    }
                                    slotItem->m_count += count;
                                }
                            }
                        }
                        else {
                            if (slotItem->m_itemID == carried->m_itemID && carried->getMaxStackSize() > 1 &&
                                (!slotItem->isStackedByData() || slotItem->getAuxValue() == carried->getAuxValue())) {
                                int slotCount = slotItem->m_count;
                                if (slotCount > 0 && (slotCount + carried->m_count) <= carried->getMaxStackSize()) {
                                    carried->m_count += slotCount;
                                    slotItem->remove(slotCount);
                                    if (slotItem->m_count == 0) {
                                        slot->set(nullptr);
                                    }
                                    slot->onTake(carried);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

void ContainerMenu::setItem(int index, std::shared_ptr<ItemInstance> item) {
    slots[index]->set(item);
}

void ContainerMenu::setAll(const std::vector<std::shared_ptr<ItemInstance>>& items) {
    for (size_t i = 0; i < items.size(); ++i) {
        slots[i]->set(items[i]);
    }
}

void ContainerMenu::setData(int id, int value) {
}

uint16_t ContainerMenu::backup(Inventory*) {
    return ++changeUid;
}

void ContainerMenu::deleteBackup(uint16_t) {
}

void ContainerMenu::rollbackToBackup(uint16_t) {
}

bool ContainerMenu::isSynched(Player* player) const {
    return unsynchedPlayers.find(player) == unsynchedPlayers.end();
}

void ContainerMenu::setSynched(Player* player, bool isSynched) {
    if (isSynched) {
        unsynchedPlayers.erase(player);
    }
    else {
        unsynchedPlayers.insert(player);
    }
}
