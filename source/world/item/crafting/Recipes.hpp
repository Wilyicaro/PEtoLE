#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include "Recipe.hpp"
#include "world/item/CraftingContainer.hpp"
#include "ShapedRecipe.hpp"
#include "ShapelessRecipe.hpp"
#include "world/tile/Tile.hpp"

class ItemInstance;

class RecipeBuilder {
public:
    virtual std::unique_ptr<Recipe> build() = 0;
};

class ShapelessRecipeBuilder : public RecipeBuilder {
public:
    ShapelessRecipeBuilder(std::shared_ptr<ItemInstance> result) : m_result(result)
    {
    }

    ShapelessRecipeBuilder& add(Item* item)
    {
        return add(std::make_shared<ItemInstance>(item));
    }

    ShapelessRecipeBuilder& add(Tile* tile)
    {
        return add(std::make_shared<ItemInstance>(tile));
    }

    ShapelessRecipeBuilder& add(std::shared_ptr<ItemInstance> ingredient)
    {
        m_ingredients.push_back(ingredient);
        return *this;
    }

    std::unique_ptr<Recipe> build()
    {
        return std::make_unique<ShapelessRecipe>(m_result, m_ingredients);
    }

private:
    std::shared_ptr<ItemInstance> m_result;
    std::vector<std::shared_ptr<ItemInstance>> m_ingredients;
};

class ShapedRecipeBuilder : public RecipeBuilder {
public:
    ShapedRecipeBuilder(std::vector<std::string> pattern, std::shared_ptr<ItemInstance> result) : m_pattern(pattern), m_result(result)
    {
    }

    ShapedRecipeBuilder& add(char character, Item* item)
    {
        return add(character, std::make_shared<ItemInstance>(item));
    }

    ShapedRecipeBuilder& add(char character, Tile* tile)
    {
        return add(character, std::make_shared<ItemInstance>(tile, 1, -1));
    }

    ShapedRecipeBuilder& add(char character, std::shared_ptr<ItemInstance> ingredient)
    {
        m_ingredients[character] = ingredient;
        return *this;
    }

    std::unique_ptr<Recipe> build()
    {
        std::vector<std::shared_ptr<ItemInstance>> ingredients;

        int width = 0;
        int height = static_cast<int>(m_pattern.size());

        for (const std::string& row : m_pattern) {
            if (static_cast<int>(row.size()) > width) {
                width = static_cast<int>(row.size());
            }
        }

        for (const std::string& row : m_pattern) {
            for (char character : row) {
                if (character == ' ') {
                    ingredients.push_back(nullptr);
                }
                else if (m_ingredients.find(character) != m_ingredients.end()) {
                    ingredients.push_back(m_ingredients[character]);
                }
                else {
                    ingredients.push_back(nullptr);
                }
            }
            for (size_t i = row.size(); i < static_cast<size_t>(width); i++) {
                ingredients.push_back(nullptr);
            }
        }
        return std::make_unique<ShapedRecipe>(width, height, m_result, ingredients);
    }

private:
    std::vector<std::string> m_pattern;
    std::shared_ptr<ItemInstance> m_result;
    std::unordered_map<char, std::shared_ptr<ItemInstance>> m_ingredients;
};

class Recipes {
public:
    static Recipes& getInstance()
    {
        static Recipes instance;
        return instance;
    }

    void add(std::unique_ptr<Recipe> recipe) {
        m_recipes.push_back(std::move(recipe));
    }

    void add(RecipeBuilder& recipe) {
        m_recipes.push_back(recipe.build());
    }

    std::shared_ptr<ItemInstance> getItemFor(CraftingContainer* container) {
        for (auto& recipe : m_recipes) {
            if (recipe->matches(container)) {
                return recipe->assemble(container);
            }
        }
        return nullptr;
    }
private:
    Recipes();

    std::vector<std::unique_ptr<Recipe>> m_recipes;
};



