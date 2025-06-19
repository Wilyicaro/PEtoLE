#include "FurnaceRecipes.hpp"

FurnaceRecipes::FurnaceRecipes()
{
    addFurnaceRecipe(Tile::ironOre, std::make_shared<ItemInstance>(Item::ironIngot));
    addFurnaceRecipe(Tile::goldOre, std::make_shared<ItemInstance>(Item::goldIngot));
    addFurnaceRecipe(Tile::diamondOre, std::make_shared<ItemInstance>(Item::diamond));
    addFurnaceRecipe(Tile::coalOre, std::make_shared<ItemInstance>(Item::coal));
    addFurnaceRecipe(Tile::redstoneOre, std::make_shared<ItemInstance>(Item::redStone, 4));
    addFurnaceRecipe(Tile::lapisOre, std::make_shared<ItemInstance>(Item::dyePowder, 4, 4));
    addFurnaceRecipe(Tile::sand, std::make_shared<ItemInstance>(Tile::glass));
    addFurnaceRecipe(Item::rawPorkchop, std::make_shared<ItemInstance>(Item::cookedPorkchop));
    addFurnaceRecipe(Item::rawFish, std::make_shared<ItemInstance>(Item::cookedFish));
    addFurnaceRecipe(Tile::cobblestone, std::make_shared<ItemInstance>(Tile::stone));
    addFurnaceRecipe(Item::clay, std::make_shared<ItemInstance>(Item::brick));
    addFurnaceRecipe(Tile::cactus, std::make_shared<ItemInstance>(Item::dyePowder, 1, 2));
    addFurnaceRecipe(Tile::treeTrunk, std::make_shared<ItemInstance>(Item::coal, 1, 1));

    LOG_I("%d furnace recipes loaded", m_furnaceRecipes.size());

    addFurnaceFuel(Material::wood, 300);
    addFurnaceFuel(Item::stick, 100);
    addFurnaceFuel(Item::coal, 1600);
    addFurnaceFuel(Item::lavaBucket, 20000);
}
