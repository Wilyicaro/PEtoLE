#include "ShapelessRecipe.hpp"
#include <world/item/CraftingContainer.hpp>

ShapelessRecipe::ShapelessRecipe(std::shared_ptr<ItemInstance> result, std::vector<std::shared_ptr<ItemInstance>> ingredients) : m_result(result), m_ingredients(ingredients)
{
}

bool ShapelessRecipe::matches(Container * container)
{
    CraftingContainer* craftingContainer = (CraftingContainer*)container;
    if (!craftingContainer) return false;

    std::vector<std::shared_ptr<ItemInstance>> var2 = m_ingredients;

    for (int var3 = 0; var3 < 3; ++var3) {
        for (int var4 = 0; var4 < 3; ++var4) {
            auto var5 = craftingContainer->getItem(var4, var3);
            if (var5) {
                bool var6 = false;

                for (auto it = var2.begin(); it != var2.end(); ++it) {
                    auto var8 = it->get();
                    if (var5->m_itemID == var8->m_itemID && (var8->getAuxValue() == -1 || var5->getAuxValue() == var8->getAuxValue())) {
                        var6 = true;
                        var2.erase(it);
                        break;
                    }
                }

                if (!var6) {
                    return false;
                }
            }
        }
    }

    return var2.empty();
}

std::shared_ptr<ItemInstance> ShapelessRecipe::assemble(Container* container)
{
    return m_result->copy();
}

int ShapelessRecipe::size()
{
	return m_ingredients.size();
}
