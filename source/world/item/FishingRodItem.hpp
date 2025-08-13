#pragma once

#include "Item.hpp"

class FishingRodItem : public Item
{
public:
	FishingRodItem(int id);

	std::shared_ptr<ItemInstance> use(std::shared_ptr<ItemInstance>, Level*, Player*) override;
	bool isHandEquipped() override;
	bool isMirroredArt() override;
};
