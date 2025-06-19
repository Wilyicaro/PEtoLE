#pragma once

#include "Item.hpp"

class BucketItem : public Item
{
public:
	BucketItem(int id, int content);
	virtual std::shared_ptr<ItemInstance> use(std::shared_ptr<ItemInstance>, Level*, Player*);
protected:
	int m_content;
};
