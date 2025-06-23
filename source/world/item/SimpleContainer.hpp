#pragma once

#include "Container.hpp"
#include <vector>

class SimpleContainer : public Container
{
public:
	SimpleContainer(int size, const std::string& name);

	virtual int getContainerSize() override;

	virtual std::shared_ptr<ItemInstance> getItem(int index) override;

	virtual std::shared_ptr<ItemInstance> removeItem(int index, int count) override;

	virtual void setItem(int index, std::shared_ptr<ItemInstance> item) override;

	virtual const std::string& getName() override;

	virtual int getMaxStackSize() override;

	virtual void setChanged() override;

	virtual bool stillValid(Player* player) override;

	virtual void clear();

	virtual void load(std::shared_ptr<CompoundTag> tag);
	virtual void save(std::shared_ptr<CompoundTag> tag);

private:
	std::vector<std::shared_ptr<ItemInstance>> m_items;

	std::string m_name;
};


