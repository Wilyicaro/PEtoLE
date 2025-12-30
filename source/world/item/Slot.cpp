#include "Slot.hpp"
#include "ItemInstance.hpp"

Slot::Slot(Container* container, int slot, int x, int y)
    : container(container), slot(slot), x(x), y(y) {
}

void Slot::swap(Slot& other) {
    auto var2 = this->container->getItem(this->slot);
    auto var3 = other.container->getItem(other.slot);

    if (var2 && var2->m_count > other.getMaxStackSize()) {
        if (var3) return;
        var3 = remove(var2->m_count - other.getMaxStackSize());
    }

    if (var3 && var3->m_count > this->getMaxStackSize()) {
        if (var2) return;
        var2 = var3->remove(var3->m_count - this->getMaxStackSize());
    }

    other.container->setItem(other.slot, var2);
    this->container->setItem(this->slot, var3);
    this->setChanged();
}

bool Slot::canSync()
{
    return true;
}
