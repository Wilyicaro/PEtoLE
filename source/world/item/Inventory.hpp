#pragma once

#include <array>
#include "world/item/ItemInstance.hpp"
#include "world/entity/Player.hpp"
#include "Container.hpp"
#include "common/ListTag.hpp"
#include "common/CompoundTag.hpp"

class Entity;
class Player; // in case we're included from Player.hpp

#define C_MAX_HOTBAR_ITEMS (9)
#define C_NUM_SLOTS (36)
#define C_MAX_AMOUNT (64)

class Inventory : public Container
{
public:
	Inventory(Player*);
	void prepareCreativeInventory();
	void prepareSurvivalInventory();

	int getContainerSize() override;
	int getNumItems();

	void setItem(int index, std::shared_ptr<ItemInstance> item) override;
    void setSelectedItem(std::shared_ptr<ItemInstance> item)
    {
        setItem(m_selected, item);
    }
    std::shared_ptr<ItemInstance> removeItem(int index, int count) override;
    bool removeResource(int id);
	void addTestItem(int itemID, int amount = 1, int auxValue = 0);

	void clear();
	bool add(std::shared_ptr<ItemInstance> pInst);
    void tick();

    std::shared_ptr<ItemInstance> getArmor(int index)
    {
        return m_armor[index];
    }

    std::shared_ptr<ItemInstance> getItem(int index) override;
    std::shared_ptr<ItemInstance> getSelectedItem();
	int getSelectedItemId();
	void selectSlot(int slotNo);

	void selectItemById(int itemID, int maxHotBarSlot);

	int getAttackDamage(Entity*);

    int getArmorValue() const;

    void hurtArmor(int amount);

    void dropAll();

    int getSelectedSlotNo() const
    {
        return m_selected;
    }

    // v0.2.0 name alias
    std::shared_ptr<ItemInstance> getSelected() {
        return getSelectedItem();
    }

    std::string getName() override 
    {
        return "Inventory";
    }

    int getMaxStackSize() override
    {
        return C_MAX_AMOUNT;
    }

    void setChanged() override 
    {
        m_bChanged = true;
    }

    bool stillValid(Player* player) override;

    std::shared_ptr<ItemInstance> getCarried()
    {
        return m_carried;
    }

    void setCarried(std::shared_ptr<ItemInstance> carried);

    std::shared_ptr<ListTag> save(std::shared_ptr<ListTag> var1) {

        for (int i = 0; i < m_items.size(); i++) {
            std::shared_ptr<ItemInstance> item = m_items[i];
            if (item) {
                std::shared_ptr<CompoundTag> var3 = std::make_shared<CompoundTag>();
                var3->putByte("Slot", i);
                item->save(var3);
                var1->add(var3);
            }
        }

        for (int i = 0; i < m_armor.size(); i++) {
            std::shared_ptr<ItemInstance> armor = m_armor[i];
            if (armor) {
                std::shared_ptr<CompoundTag> var3 = std::make_shared<CompoundTag>();
                var3->putByte("Slot", i + 100);
                armor->save(var3);
                var1->add(var3);
            }
        }

        return var1;
    }

    void load(std::shared_ptr<ListTag> var1) {
        clear();

        for (auto v : var1->getValue()) {
            std::shared_ptr<CompoundTag> var3 = std::dynamic_pointer_cast<CompoundTag>(v);
            int var4 = var3->getByte("Slot") & 255;
            std::shared_ptr<ItemInstance> var5 = std::make_shared<ItemInstance>(var3);
            if (var5->getItem()) {
                if (var4 >= 0 && var4 < m_items.size()) {
                    m_items[var4] = var5;
                }

                if (var4 >= 100 && var4 < m_armor.size() + 100) {
                    m_armor[var4 - 100] = var5;
                }
            }
        }

    }

public:
	int m_selected;
    bool m_bChanged;
    Player* m_pPlayer;

private:
    int getSlotWithRemainingSpace(std::shared_ptr<ItemInstance> item);
    int getFreeSlot();
    int addResource(std::shared_ptr<ItemInstance> item);
    int getSlot(int id);


    std::shared_ptr<ItemInstance> m_carried = nullptr;
	std::array<std::shared_ptr<ItemInstance>, C_NUM_SLOTS> m_items;
    std::array<std::shared_ptr<ItemInstance>, 4> m_armor;
};
