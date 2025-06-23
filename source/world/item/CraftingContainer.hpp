#pragma once

#include <vector>
#include <memory>
#include "Container.hpp"
#include "ContainerMenu.hpp"

class ContainerMenu;
class Player;
class ItemInstance;

class CraftingContainer : public Container {
private:
    std::vector<std::shared_ptr<ItemInstance>> items;
    int width;
    ContainerMenu* menu;

public:
    CraftingContainer(ContainerMenu* menu, int width, int height);

    int getContainerSize() override;
    std::shared_ptr<ItemInstance> getItem(int index) override;
    std::shared_ptr<ItemInstance> getItem(int x, int y);

    const std::string& getName() override;

    std::shared_ptr<ItemInstance> removeItem(int index, int amount) override;
    void setItem(int index, std::shared_ptr<ItemInstance> item) override;

    int getMaxStackSize() override;
    void setChanged() override;
    bool stillValid(Player* player) override;
};