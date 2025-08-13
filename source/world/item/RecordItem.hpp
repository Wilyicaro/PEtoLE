#pragma once

#include "Item.hpp"

class RecordItem : public Item
{
public:
	RecordItem(int id, const std::string& record);

	virtual bool useOn(ItemInstance*, Player*, Level*, const TilePos& pos, Facing::Name face) override;
	const std::string& getStreamingMusic() override;

private:
	 std::string m_record;
};
