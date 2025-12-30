#include "Inventory.hpp"
#include "Item.hpp"
#include "world/gamemode/GameMode.hpp"

Inventory::Inventory(Player* pPlayer) : Container()
{
	m_pPlayer = pPlayer;
	m_selected = 0;
	clear();
}

void Inventory::prepareCreativeInventory()
{
	addTestItem(Tile::stone->m_ID);
	addTestItem(Tile::cobblestone->m_ID);
	addTestItem(Tile::brick->m_ID);
	addTestItem(Tile::dirt->m_ID);
	addTestItem(Tile::wood->m_ID);
	addTestItem(Tile::treeTrunk->m_ID);
	addTestItem(Tile::leaves->m_ID);
	addTestItem(Tile::torch->m_ID);
	addTestItem(Tile::stoneSlabHalf->m_ID);
}

void Inventory::prepareSurvivalInventory()
{
	// Add some items for testing
	addTestItem(Item::ironSword->m_itemID);
	addTestItem(Item::ironPickaxe->m_itemID);
	addTestItem(Item::ironAxe->m_itemID);
	addTestItem(Item::ironShovel->m_itemID);
	addTestItem(Item::ironHoe->m_itemID);
	addTestItem(Item::stick->m_itemID, 64);
	addTestItem(Item::wheat->m_itemID, 64);
	addTestItem(Item::sugar->m_itemID, 64);
	addTestItem(Tile::ladder->m_ID, 64);
	addTestItem(Tile::obsidian->m_ID, 64);
	addTestItem(Tile::fire->m_ID, 64);
	addTestItem(Item::woodDoor->m_itemID);
	addTestItem(Item::ironDoor->m_itemID);
	addTestItem(Tile::fence->m_ID, 64);
	addTestItem(Tile::lever->m_ID, 64);
	addTestItem(Item::redStone->m_itemID, 64);
	addTestItem(Tile::redstoneTorchLit->m_ID, 64);
}

int Inventory::getContainerSize()
{
	return C_NUM_SLOTS + 4;
}

int Inventory::getNumItems()
{
	int items;
	for (items = 0; items < getContainerSize(); items++) {
		if (m_items[items]) items++;
	}
	return items;
}


void Inventory::clear()
{
	m_items.fill(nullptr);
	m_armor.fill(nullptr);
}

bool Inventory::add(std::shared_ptr<ItemInstance> pInst)
{
	if (!pInst->isDamaged()) {
		pInst->m_count = addResource(pInst);
		if (!pInst->m_count) {
			return true;
		}
	}

	int var2 = getFreeSlot();
	if (var2 >= 0) {
		m_items[var2] = pInst;
		m_items[var2]->m_popTime = 5;
		return true;
	}
	else {
		return false;
	}
}

bool Inventory::contains(const std::shared_ptr<ItemInstance>& item)
{
	for (auto& armor : m_armor) {
		if (armor && armor->sameItem(item)) {
			return true;
		}
	}

	for (auto& i : m_items) {
		if (i && i->sameItem(item)) {
			return true;
		}
	}

	return false;
}

int Inventory::getSlotWithRemainingSpace(std::shared_ptr<ItemInstance> item) {
	for (int index = 0; index < m_items.size(); index++) {
		std::shared_ptr<ItemInstance> i = m_items[index];
		if (i && i->m_itemID == item->m_itemID && i->isStackable() && i->m_count < i->getMaxStackSize() && i->m_count < getMaxStackSize() && (!i->isStackedByData() || i->getAuxValue() == item->getAuxValue())) {
			return index;
		}
	}

	return -1;
}

int Inventory::getFreeSlot() {
	for (int var1 = 0; var1 < m_items.size(); var1++) {
		if (!m_items[var1]) {
			return var1;
		}
	}

	return -1;
}

int Inventory::addResource(std::shared_ptr<ItemInstance> item) {
	int var2 = item->m_itemID;
	int var3 = item->m_count;
	int var4 = getSlotWithRemainingSpace(item);
	if (var4 < 0) var4 = getFreeSlot();
	

	if (var4 < 0) {
		return var3;
	}
	else {
		if (!m_items[var4]) {
			m_items[var4] = std::make_shared<ItemInstance>(var2, 0, item->getAuxValue());
		}

		int var5 = var3;
		if (var3 > m_items[var4]->getMaxStackSize() - m_items[var4]->m_count) {
			var5 = m_items[var4]->getMaxStackSize() - m_items[var4]->m_count;
		}

		if (var5 > getMaxStackSize() - m_items[var4]->m_count) {
			var5 = getMaxStackSize() - m_items[var4]->m_count;
		}

		if (!var5) 
		{
			return var3;
		}
		else 
		{
			var3 -= var5;
			m_items[var4]->m_count += var5;
			m_items[var4]->m_popTime = 5;
			return var3;
		}
	}
}

void Inventory::tick()
{
	for (int i = 0; i < m_items.size(); i++)
	{
		auto& v = m_items[i];
		if (v) 
		{
			if (v->m_popTime > 0) v->m_popTime--;
			v->getItem()->inventoryTick(v, m_pPlayer->m_pLevel, m_pPlayer, i, i == m_selected);
		}
	}
}

void Inventory::setItem(int index, std::shared_ptr<ItemInstance> item)
{
	if (index >= m_items.size()) setArmor(index - m_items.size(), item);
	else m_items[index] = item;
}

void Inventory::setArmor(int index, std::shared_ptr<ItemInstance> item)
{
	m_armor[index] = item;
}

std::shared_ptr<ItemInstance> Inventory::removeItem(int index, int count)
{
	std::shared_ptr<ItemInstance> item = getItem(index);

	if (item) {
		if (item->m_count <= count) {
			setItem(index, nullptr);
			return item;
		}
		else {
			item = item->remove(count);
			if (item->m_count == 0) {
				setItem(index, nullptr);
			}

			return item;
		}
	}
	else {
		return nullptr;
	}
}

int Inventory::getSlot(int id) {
	for (int i = 0; i < m_items.size(); ++i) {
		if (m_items[i] && m_items[i]->m_itemID == id) {
			return i;
		}
	}

	return -1;
}

bool Inventory::removeResource(int id)
{
	int var2 = getSlot(id);
	if (var2 < 0) {
		return false;
	}
	else {
		if (--m_items[var2]->m_count <= 0) {
			m_items[var2] = nullptr;
		}

		return true;
	}
}

void Inventory::addTestItem(int itemID, int amount, int auxValue)
{
	std::shared_ptr<ItemInstance> inst = std::make_shared<ItemInstance>(itemID, amount, auxValue);
	add(inst);

	if (inst->m_count != 0)
	{
		LOG_I("AddTestItem: Couldn't add all %d of %s, only gave %d",
			amount, Item::items[itemID]->m_DescriptionID.c_str(), amount - inst->m_count);
	}
}

std::shared_ptr<ItemInstance> Inventory::getItem(int index)
{
	if (index >= m_items.size()) return m_armor[index - m_items.size()];
	else return m_items[index];
}

std::shared_ptr<ItemInstance> Inventory::getSelectedItem()
{
	return getItem(m_selected);
}

int Inventory::getSelectedItemId()
{
	std::shared_ptr<ItemInstance> item = getSelectedItem();
	return item ? item->m_itemID : -1;
}

void Inventory::selectSlot(int slotNo)
{
	if (slotNo < 0 || slotNo >= C_MAX_HOTBAR_ITEMS)
		return;
	if (m_selected != slotNo)
	{
		m_selected = slotNo;
		m_pPlayer->m_pGameMode->sendCarriedItem(slotNo);
	}
}

void Inventory::selectItem(int itemID, int data, int maxHotBarSlot)
{
	Item* selectItem = Item::items[itemID];

	if (!selectItem) return;

	for (int i = 0; i < m_items.size(); i++)
	{
		if (!m_items[i] || m_items[i]->m_itemID != itemID || m_items[i]->getAuxValue() != data)
			continue;

		if (i < maxHotBarSlot) m_selected = i;
		else 
		{
			auto picked = m_items[i];
			auto old = getSelectedItem();
			setItem(m_selected, picked);
			setItem(i, old);
		}
		return;
	}

	if (m_pPlayer->isCreative())
	{
		auto oldSelected = getSelected();
		setItem(m_selected, std::make_shared<ItemInstance>(selectItem, 1, data));
		if (oldSelected) addResource(oldSelected);
	}
}

int Inventory::getAttackDamage(Entity* pEnt)
{
	std::shared_ptr<ItemInstance> pInst = getSelected();
	if (!pInst)
		return 1;

	return pInst->getAttackDamage(pEnt);
}

int Inventory::getArmorValue() const
{
	int var1 = 0;
	int var2 = 0;
	int var3 = 0;

	for (auto& armor : m_armor)
	{
		if (!armor || armor->getItem()->getDefense() <= 0) continue;
		int var5 = armor->getMaxDamage();
		int var6 = armor->getDamageValue();
		int var7 = var5 - var6;
		var2 += var7;
		var3 += var5;
		var1 += armor->getItem()->getDefense();
	}

	if (var3 == 0) {
		return 0;
	}
	else {
		return (var1 - 1) * var2 / var3 + 1;
	}
}

void Inventory::hurtArmor(int amount)
{
	for (int i = 0; i < m_armor.size(); i++)
	{
		std::shared_ptr<ItemInstance> armor = m_armor[i];
		if (!armor || armor->getItem()->getDefense() <= 0) continue;
		armor->hurtAndBreak(amount, m_pPlayer);
		if (!armor->m_count)
		{
			armor->snap(m_pPlayer);
			m_armor[i] = nullptr;
		}
	}
}

void Inventory::dropAll()
{
	for (int i = 0; i < m_items.size(); i++)
	{
		std::shared_ptr<ItemInstance> item = m_items[i];
		if (item)
		{
			m_pPlayer->drop(item, true);
			m_items[i] = nullptr;
		}
	}

	for (int i = 0; i < m_armor.size(); i++)
	{
		std::shared_ptr<ItemInstance> item = m_armor[i];
		if (item)
		{
			m_pPlayer->drop(item, true);
			m_armor[i] = nullptr;
		}
	}
}

bool Inventory::stillValid(Player* player)
{
	if (m_pPlayer->m_bRemoved) {
		return false;
	}
	else {
		return !(player->distanceToSqr(m_pPlayer) > 64.0);
	}
}

void Inventory::setCarried(std::shared_ptr<ItemInstance> carried)
{
	m_carried = carried;
	m_pPlayer->carriedChanged(carried);
}
