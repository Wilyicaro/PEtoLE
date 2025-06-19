#include "SingleInputRecipe.hpp"

SingleInputRecipe::SingleInputRecipe(std::shared_ptr<ItemInstance> ingredient, std::shared_ptr<ItemInstance> result) : m_ingredient(ingredient), m_result(result)
{
}

bool SingleInputRecipe::matches(Container* container)
{
    return matches(container->getItem(0));
}

bool SingleInputRecipe::matches(std::shared_ptr<ItemInstance> input)
{
    return input->m_itemID == m_ingredient->m_itemID && (m_ingredient->getAuxValue() == -1 || input->getAuxValue() == m_ingredient->getAuxValue());
}

std::shared_ptr<ItemInstance> SingleInputRecipe::assemble(Container* container)
{
    return m_result->copy();
}

int SingleInputRecipe::size()
{
	return 1;
}
