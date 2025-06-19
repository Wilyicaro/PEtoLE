#pragma once

#include <memory>
#include "ContainerMenu.hpp"
#include "Container.hpp"
#include "world/entity/Player.hpp"

class ChestMenu : public ContainerMenu {
private:
    Container* m_container;

public:
    ChestMenu(Container* inventory, Container* container);

    bool stillValid(Player* player) const override;
};
