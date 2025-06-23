#pragma once

#include "Container.hpp"

class Player;
class ItemInstance;

class ResultContainer : public Container {
private:
    std::shared_ptr<ItemInstance> items[1];

public:
    ResultContainer();

    int getContainerSize() override;
    std::shared_ptr<ItemInstance> getItem(int index) override;

    const std::string& getName() override;

    std::shared_ptr<ItemInstance> removeItem(int index, int amount) override;
    void setItem(int index, std::shared_ptr<ItemInstance> item) override;

    int getMaxStackSize() override;
    void setChanged() override;
    bool stillValid(Player* player) override;
};
