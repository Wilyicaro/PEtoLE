#include "ShapedRecipe.hpp"

ShapedRecipe::ShapedRecipe(int width, int height, std::shared_ptr<ItemInstance> result, std::vector<std::shared_ptr<ItemInstance>> ingredients) : m_width(width), m_height(height), m_result(result), m_ingredients(ingredients)
{
}

bool ShapedRecipe::matches(Container * container)
{
    CraftingContainer* craftingContainer = (CraftingContainer*)container;
    if (!craftingContainer) return false;

    for (int var2 = 0; var2 <= 3 - m_width; ++var2) {
        for (int var3 = 0; var3 <= 3 - m_height; ++var3) {
            if (matches(craftingContainer, var2, var3, true)) {
                return true;
            }

            if (matches(craftingContainer, var2, var3, false)) {
                return true;
            }
        }
    }

    return false;
}

std::shared_ptr<ItemInstance> ShapedRecipe::assemble(Container* container)
{
	return m_result->copy();
}

int ShapedRecipe::size()
{
	return m_width * m_height;
}

bool ShapedRecipe::matches(CraftingContainer* container, int i, int j, bool lenient)
{
    for (int var5 = 0; var5 < 3; ++var5) {
        for (int var6 = 0; var6 < 3; ++var6) {
            int var7 = var5 - i;
            int var8 = var6 - j;
            std::shared_ptr<ItemInstance> var9 = nullptr;
            if (var7 >= 0 && var8 >= 0 && var7 < m_width && var8 < m_height) {
                if (lenient) {
                    var9 = m_ingredients[m_width - var7 - 1 + var8 * m_width];
                }
                else {
                    var9 = m_ingredients[var7 + var8 * m_width];
                }
            }

            auto var10 = container->getItem(var5, var6);
            if (var10 || var9) {
                if (!var10 && var9 || var10 && !var9) {
                    return false;
                }

                if (var9->m_itemID != var10->m_itemID) {
                    return false;
                }

                if (var9->getAuxValue() != -1 && var9->getAuxValue() != var10->getAuxValue()) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool ShapedRecipe::isShaped() const
{
    return false;
}