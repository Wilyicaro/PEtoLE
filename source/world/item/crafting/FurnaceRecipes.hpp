#include <unordered_map>

#include "SingleInputRecipe.hpp"
#include "world/tile/Tile.hpp"

class FurnaceRecipes {
public:
    static FurnaceRecipes& getInstance()
    {
        static FurnaceRecipes instance;
        return instance;
    }

    void addFurnaceRecipe(Tile* tile, std::shared_ptr<ItemInstance> result) {
        addFurnaceRecipe(std::make_shared<ItemInstance>(tile), result);
    }

    void addFurnaceRecipe(Item* item, std::shared_ptr<ItemInstance> result) {
        addFurnaceRecipe(std::make_shared<ItemInstance>(item), result);
    }

    void addFurnaceRecipe(std::shared_ptr<ItemInstance> ingredient, std::shared_ptr<ItemInstance> result) {
        m_furnaceRecipes.insert({ingredient->m_itemID, std::make_unique<SingleInputRecipe>(ingredient, result)});
    }

    void add(std::unique_ptr<SingleInputRecipe> recipe) {
        m_furnaceRecipes.insert({recipe->m_ingredient->m_itemID, std::move(recipe)});
    }

    std::shared_ptr<ItemInstance> getItemFor(Container* container) {
        auto testItem = container->getItem(0);
        auto matches = m_furnaceRecipes.equal_range(testItem->m_itemID);
        for (auto& it = matches.first; it != matches.second; ++it) {
            if (it->second->matches(container))
                return it->second->assemble(container);
        }
        return nullptr;
    }

    bool isFurnaceItem(std::shared_ptr<ItemInstance> item)
    {
        auto matches = m_furnaceRecipes.equal_range(item->m_itemID);
        for (auto& it = matches.first; it != matches.second; ++it) {
            if (it->second->matches(item))
                return true;
        }
        return false;
    }

    int getBurnDuration(std::shared_ptr<ItemInstance> item)
    {
        if (!item) return 0;

        auto it = m_burnDuration.find(item->m_itemID);
        if (it != m_burnDuration.end())
            return it->second;
        if (item->m_itemID < 256)
        {
            auto it = m_materialBurnDuration.find(Tile::tiles[item->m_itemID]->m_pMaterial);
            if (it != m_materialBurnDuration.end())
                return it->second;
        }
        return 0;
    }

    void addFurnaceFuel(Tile* tile, int burnDuration) {
        m_burnDuration[tile->m_ID] = burnDuration;
    }

    void addFurnaceFuel(Item* item, int burnDuration) {
        m_burnDuration[item->m_itemID] = burnDuration;
    }

    void addFurnaceFuel(Material* material, int burnDuration) {
        m_materialBurnDuration[material] = burnDuration;
    }

private:
    FurnaceRecipes();

    std::unordered_multimap<int, std::unique_ptr<SingleInputRecipe>> m_furnaceRecipes;
    std::unordered_map<int, int> m_burnDuration;
    std::unordered_map<Material*, int> m_materialBurnDuration;
};



