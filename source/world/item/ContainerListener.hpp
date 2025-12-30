#pragma once

#include <memory>
#include <vector>

class ContainerMenu;
class ItemInstance;

class ContainerListener {
public:
    virtual ~ContainerListener() = default;

    virtual void refreshContainer(ContainerMenu* menu, const std::vector<std::shared_ptr<ItemInstance>>& items) = 0;
    virtual void refreshContainerItems(ContainerMenu* menu);
    virtual void slotChanged(ContainerMenu* menu, int index, std::shared_ptr<ItemInstance> item) = 0;
    virtual void setContainerData(ContainerMenu* menu, int id, int value) = 0;
};