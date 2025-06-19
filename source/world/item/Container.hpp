#pragma once

#include "ItemInstance.hpp"

class Container
{
public:
	static constexpr int LARGE_MAX_STACK_SIZE = 64;

	virtual int getContainerSize() = 0;

	virtual std::shared_ptr<ItemInstance> getItem(int index) = 0;

	virtual std::shared_ptr<ItemInstance> removeItem(int index, int count) = 0;

	virtual void setItem(int index, std::shared_ptr<ItemInstance> item) = 0;

	virtual std::string getName() = 0;

	virtual int getMaxStackSize() = 0;

	virtual void setChanged() = 0;

	virtual bool stillValid(Player* player) = 0;
};


