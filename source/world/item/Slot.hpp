#pragma once

#include <memory>
#include "Container.hpp"

class ItemInstance;

class Slot {
public:
    int index;
    int x;
    int y;
    Container* container;

    Slot(Container* container, int slot, int x, int y);

    virtual void swap(Slot& other);
    virtual void onTake(std::shared_ptr<ItemInstance>) {
        setChanged();
    }

    virtual bool mayPlace(std::shared_ptr<ItemInstance> item) {
        return true;
    }

    virtual std::shared_ptr<ItemInstance> getItem() {
        return container->getItem(slot);
    }

    virtual bool hasItem() {
        return getItem() != nullptr;
    }

    virtual void set(std::shared_ptr<ItemInstance> item) {
        container->setItem(slot, item);
        setChanged();
    }

    virtual void setChanged() {
        container->setChanged();
    }

    virtual int getMaxStackSize() {
        return container->getMaxStackSize();
    }

    virtual int getNoItemIcon() {
        return -1;
    }

    virtual std::shared_ptr<ItemInstance> remove(int count) {
        return container->removeItem(slot, count);
    }

    virtual bool isAt(Container* cont, int s) {
        return cont == container && s == slot;
    }


protected:
    int slot;
};