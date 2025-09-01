#include "FishingRodItem.hpp"
#include "world/level/Level.hpp"
#include <world/entity/projectile/FishingHook.hpp>

FishingRodItem::FishingRodItem(int id) : Item(id)
{
    m_maxDamage = 64;
    m_maxStackSize = 1;
}

std::shared_ptr<ItemInstance> FishingRodItem::use(std::shared_ptr<ItemInstance> item, Level* level, Player* player)
{
    if (player->m_fishing)
    {
        item->hurtAndBreak(player->m_fishing->retrieve(), player);
        player->swing();
    }
    else
    {
        level->playSound(player, "random.bow", 0.5F, 0.4F / (itemRand.nextFloat() * 0.4F + 0.8F));
        if (!level->m_bIsOnline)
        {
            auto hook = std::make_shared<FishingHook>(level, std::dynamic_pointer_cast<Player>(player->shared_from_this()));
            player->m_fishing = hook;
            level->addEntity(hook);
        }

        player->swing();
    }
    return item;
}

bool FishingRodItem::isHandEquipped()
{
    return true;
}

bool FishingRodItem::isMirroredArt()
{
    return true;
}
