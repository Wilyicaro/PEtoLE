#pragma once

#include <vector>

#include "Recipe.hpp"

class ShapelessRecipe : public Recipe
{
public:
    ShapelessRecipe(std::shared_ptr<ItemInstance> result, std::vector<std::shared_ptr<ItemInstance>> ingredients);

    virtual bool matches(Container * container) override;
    virtual std::shared_ptr<ItemInstance> assemble(Container * container) override;
    virtual int size() override;

private:
    std::shared_ptr<ItemInstance> m_result;
    std::vector<std::shared_ptr<ItemInstance>> m_ingredients;
};