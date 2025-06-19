#pragma once

#include <vector>

#include "Recipe.hpp"
#include "world/item/CraftingContainer.hpp"

class ShapedRecipe : public Recipe
{
public:
    ShapedRecipe(int width, int height, std::shared_ptr<ItemInstance> result, std::vector<std::shared_ptr<ItemInstance>> ingredients);

    virtual bool matches(Container * container) override;
    virtual std::shared_ptr<ItemInstance> assemble(Container * container) override;
    virtual int size() override;

private:
    bool matches(CraftingContainer* container, int i, int j, bool lenient);

    bool isShaped() const override;

    int m_width, m_height;
    std::shared_ptr<ItemInstance> m_result;
    std::vector<std::shared_ptr<ItemInstance>> m_ingredients;
};