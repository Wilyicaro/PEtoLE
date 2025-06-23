#pragma once

#include <memory>
#include "ContainerMenu.hpp"
#include "Container.hpp"
#include "world/entity/Player.hpp"

class CreativeMenu : public ContainerMenu {
private:
    Container* m_container;

public:
    CreativeMenu(Container* inventory, Container* container);

    void updateScroll(float scroll);

    bool stillValid(Player* player) const override;

public:
    std::vector<std::shared_ptr<ItemInstance>> creativeItems;
};
