#pragma once

#include "Recipe.hpp"

class SingleInputRecipe : public Recipe
{
public:
    SingleInputRecipe(std::shared_ptr<ItemInstance> ingredient, std::shared_ptr<ItemInstance> result);

    virtual bool matches(Container* container) override;
    virtual bool matches(std::shared_ptr<ItemInstance> input);
    virtual std::shared_ptr<ItemInstance> assemble(Container * container) override;
    virtual int size() override;

    std::shared_ptr<ItemInstance> m_ingredient;

private:
    std::shared_ptr<ItemInstance> m_result;
};