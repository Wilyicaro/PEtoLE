#pragma once

#include "ComplexItem.hpp"
#include <world/level/saveddata/MapItemSavedData.hpp>

class MapItem : public ComplexItem
{
public:
    MapItem(int);

    static std::shared_ptr<MapItemSavedData> createSavedData(short, Level*);
    virtual std::shared_ptr<MapItemSavedData> getMapSavedData(const std::shared_ptr<ItemInstance>&, Level*);
    virtual void inventoryTick(const std::shared_ptr<ItemInstance>&, Level*, Entity*, int, bool) override;
    virtual void update(Level*, Entity*, const std::shared_ptr<MapItemSavedData>&);
    virtual void onCraftedBy(const std::shared_ptr<ItemInstance>&, Player*, Level*) override;
    virtual Packet* getUpdatePacket(const std::shared_ptr<ItemInstance>&, Level*, const std::shared_ptr<Player>&) override;
};