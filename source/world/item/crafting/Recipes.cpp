#include "Recipes.hpp"
#include "world/tile/ClothTile.hpp"
#include "world/tile/RecordPlayerTile.hpp"

void Recipes::addTools(std::shared_ptr<ItemInstance> material, Item* sword, Item* pickaxe, Item* axe, Item* shovel, Item* hoe)
{
    add(ShapedRecipeBuilder({ "X", "X", "#" }, std::make_shared<ItemInstance>(sword)).add('X', material).add('#', Item::stick));
    add(ShapedRecipeBuilder({ "XXX", " # ", " # " }, std::make_shared<ItemInstance>(pickaxe)).add('X', material).add('#', Item::stick));
    add(ShapedRecipeBuilder({ "XX", "X#", " #" }, std::make_shared<ItemInstance>(axe)).add('X', material).add('#', Item::stick));
    add(ShapedRecipeBuilder({ "X", "#", "#" }, std::make_shared<ItemInstance>(shovel)).add('X', material).add('#', Item::stick));
    add(ShapedRecipeBuilder({ "XX", " #", " #" }, std::make_shared<ItemInstance>(hoe)).add('X', material).add('#', Item::stick));
}

void Recipes::addArmour(std::shared_ptr<ItemInstance> material, Item* helmet, Item* chestplate, Item* leggings, Item* boots)
{
    add(ShapedRecipeBuilder({ "XXX", "X X" }, std::make_shared<ItemInstance>(helmet)).add('X', material));
    add(ShapedRecipeBuilder({ "X X", "XXX", "XXX"}, std::make_shared<ItemInstance>(chestplate)).add('X', material));
    add(ShapedRecipeBuilder({ "XXX", "X X", "X X" }, std::make_shared<ItemInstance>(leggings)).add('X', material));
    add(ShapedRecipeBuilder({ "X X", "X X" }, std::make_shared<ItemInstance>(boots)).add('X', material));
}

Recipes::Recipes()
{
    //Weapon/ToolRecipes
    addTools(std::make_shared<ItemInstance>(Tile::wood), Item::woodSword, Item::woodPickaxe, Item::woodAxe, Item::woodShovel, Item::woodHoe);
    addTools(std::make_shared<ItemInstance>(Tile::cobblestone), Item::stoneSword, Item::stonePickaxe, Item::stoneAxe, Item::stoneShovel, Item::stoneHoe);
    addTools(std::make_shared<ItemInstance>(Item::ironIngot), Item::ironSword, Item::ironPickaxe, Item::ironAxe, Item::ironShovel, Item::ironHoe);
    addTools(std::make_shared<ItemInstance>(Item::goldIngot), Item::goldSword, Item::goldPickaxe, Item::goldAxe, Item::goldShovel, Item::goldHoe);
    addTools(std::make_shared<ItemInstance>(Item::diamond), Item::diamondSword, Item::diamondPickaxe, Item::diamondAxe, Item::diamondShovel, Item::diamondHoe);
    add(ShapedRecipeBuilder({ " #X", "# X", " #X" }, std::make_shared<ItemInstance>(Item::bow)).add('X', Item::string).add('#', Item::stick));
    add(ShapedRecipeBuilder({ "X", "#", "Y" }, std::make_shared<ItemInstance>(Item::arrow, 4)).add('Y', Item::feather).add('X', Item::flint).add('#', Item::stick));

    //ArmorRecipes
    addArmour(std::make_shared<ItemInstance>(Item::leather), Item::clothHelmet, Item::clothChestplate, Item::clothLeggings, Item::clothBoots);
    addArmour(std::make_shared<ItemInstance>(Tile::fire), Item::chainHelmet, Item::chainChestplate, Item::chainLeggings, Item::chainBoots);
    addArmour(std::make_shared<ItemInstance>(Item::ironIngot), Item::ironHelmet, Item::ironChestplate, Item::ironLeggings, Item::ironBoots);
    addArmour(std::make_shared<ItemInstance>(Item::goldIngot), Item::goldHelmet, Item::goldChestplate, Item::goldLeggings, Item::goldBoots);
    addArmour(std::make_shared<ItemInstance>(Item::diamond), Item::diamondHelmet, Item::diamondChestplate, Item::diamondLeggings, Item::diamondBoots);

    //StructureRecipes
    add(ShapedRecipeBuilder({ "###", "# #", "###" }, std::make_shared<ItemInstance>(Tile::chest)).add('#', Tile::wood));
    add(ShapedRecipeBuilder({ "###", "# #", "###" }, std::make_shared<ItemInstance>(Tile::furnace)).add('#', Tile::cobblestone));
    add(ShapedRecipeBuilder({ "##", "##" }, std::make_shared<ItemInstance>(Tile::craftingTable)).add('#', Tile::wood));
    add(ShapedRecipeBuilder({ "##", "##" }, std::make_shared<ItemInstance>(Tile::sandStone)).add('#', Tile::sand));
    add(ShapedRecipeBuilder({ "##", "##" }, std::make_shared<ItemInstance>(Tile::glowstone)).add('#', Item::yellowDust));

    add(ShapedRecipeBuilder({ "###" }, std::make_shared<ItemInstance>(Item::paper, 3))
        .add('#', Item::reeds));

    add(ShapedRecipeBuilder({ "#", "#", "#" }, std::make_shared<ItemInstance>(Item::book, 1))
        .add('#', Item::paper));

    add(ShapedRecipeBuilder({ "###", "###" }, std::make_shared<ItemInstance>(Tile::fence, 2))
        .add('#', Item::stick));

    add(ShapedRecipeBuilder({ "###", "#X#", "###" }, std::make_shared<ItemInstance>(Tile::recordPlayer, 1))
        .add('#', Tile::wood)
        .add('X', Item::diamond));

    add(ShapedRecipeBuilder({ "###", "#X#", "###" }, std::make_shared<ItemInstance>(Tile::musicBlock, 1))
        .add('#', Tile::wood)
        .add('X', Item::redStone));

    add(ShapedRecipeBuilder({ "###", "XXX", "###" }, std::make_shared<ItemInstance>(Tile::bookshelf, 1))
        .add('#', Tile::wood)
        .add('X', Item::book));

    add(ShapedRecipeBuilder({ "##", "##" }, std::make_shared<ItemInstance>(Tile::snow, 1))
        .add('#', Item::snowBall));

    add(ShapedRecipeBuilder({ "##", "##" }, std::make_shared<ItemInstance>(Tile::clay, 1))
        .add('#', Item::clay));

    add(ShapedRecipeBuilder({ "##", "##" }, std::make_shared<ItemInstance>(Tile::brick, 1))
        .add('#', Item::brick));

    add(ShapedRecipeBuilder({ "###", "###", "###" }, std::make_shared<ItemInstance>(Tile::glowstone, 1))
        .add('#', Item::yellowDust));

    add(ShapedRecipeBuilder({ "###", "###", "###" }, std::make_shared<ItemInstance>(Tile::cloth, 1))
        .add('#', Item::string));

    add(ShapedRecipeBuilder({ "X#X", "#X#", "X#X" }, std::make_shared<ItemInstance>(Tile::tnt, 1))
        .add('X', Item::sulphur)
        .add('#', Tile::sand));

    add(ShapedRecipeBuilder({ "###" }, std::make_shared<ItemInstance>(Tile::stoneSlabHalf, 3))
        .add('#', Tile::stone));

    add(ShapedRecipeBuilder({ "###" }, std::make_shared<ItemInstance>(Tile::stoneSlabHalf, 3, 1))
        .add('#', Tile::sandStone));

    add(ShapedRecipeBuilder({ "###" }, std::make_shared<ItemInstance>(Tile::stoneSlabHalf, 3, 2))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder({ "###" }, std::make_shared<ItemInstance>(Tile::stoneSlabHalf, 3, 3))
        .add('#', Tile::cobblestone));

    add(ShapedRecipeBuilder({ "###", "###" }, std::make_shared<ItemInstance>(Tile::trapDoor, 2))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder({ "# #", "###", "# #" }, std::make_shared<ItemInstance>(Tile::ladder, 2))
        .add('#', Item::stick));

    add(ShapedRecipeBuilder({ "##", "##", "##" }, std::make_shared<ItemInstance>(Item::woodDoor, 1))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder({ "##", "##", "##" }, std::make_shared<ItemInstance>(Item::ironDoor, 1))
        .add('#', Item::ironIngot));

    add(ShapedRecipeBuilder({ "###", "###", " X " }, std::make_shared<ItemInstance>(Item::sign, 1))
        .add('#', Tile::wood)
        .add('X', Item::stick));

    add(ShapedRecipeBuilder({ "AAA", "BEB", "CCC" }, std::make_shared<ItemInstance>(Item::cake, 1))
        .add('A', Item::milk)
        .add('B', Item::sugar)
        .add('C', Item::wheat)
        .add('E', Item::egg));

    add(ShapedRecipeBuilder({ "#" }, std::make_shared<ItemInstance>(Item::sugar, 1))
        .add('#', Item::reeds));

    add(ShapedRecipeBuilder({ "#" }, std::make_shared<ItemInstance>(Tile::wood, 4))
        .add('#', Tile::treeTrunk));

    add(ShapedRecipeBuilder({ "#", "#" }, std::make_shared<ItemInstance>(Item::stick, 4))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder({ "X", "#" }, std::make_shared<ItemInstance>(Tile::torch, 4))
        .add('X', Item::coal)
        .add('#', Item::stick));

    add(ShapedRecipeBuilder({ "X", "#" }, std::make_shared<ItemInstance>(Tile::torch, 4))
        .add('X', std::make_shared<ItemInstance>(Item::coal, 1, 1))
        .add('#', Item::stick));

    add(ShapedRecipeBuilder({ "# #", " # " }, std::make_shared<ItemInstance>(Item::bowl, 4))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder({ "X X", "X#X", "X X" }, std::make_shared<ItemInstance>(Tile::rail, 16))
        .add('X', Item::ironIngot)
        .add('#', Item::stick));

    add(ShapedRecipeBuilder({ "# #", "###" }, std::make_shared<ItemInstance>(Item::minecart, 1))
        .add('#', Item::ironIngot));

    add(ShapedRecipeBuilder({ "A", "B" }, std::make_shared<ItemInstance>(Tile::pumpkinLantern, 1))
        .add('A', Tile::pumpkin)
        .add('B', Tile::torch));

    add(ShapedRecipeBuilder({"A", "B"}, std::make_shared<ItemInstance>(Item::chestMinecart, 1))
        .add('A', Tile::chest)
        .add('B', Item::minecart));

    add(ShapedRecipeBuilder({"A", "B"}, std::make_shared<ItemInstance>(Item::furnaceMinecart, 1))
        .add('A', Tile::furnace)
        .add('B', Item::minecart));

    add(ShapedRecipeBuilder({ "# #", "###" }, std::make_shared<ItemInstance>(Item::boat, 1))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder({ "# #", " # " }, std::make_shared<ItemInstance>(Item::emptyBucket, 1))
        .add('#', Item::ironIngot));

    add(ShapedRecipeBuilder({ "A ", " B" }, std::make_shared<ItemInstance>(Item::flintAndSteel, 1))
        .add('A', Item::ironIngot)
        .add('B', Item::flint));

    add(ShapedRecipeBuilder({ "###" }, std::make_shared<ItemInstance>(Item::bread, 1))
        .add('#', Item::wheat));

    add(ShapedRecipeBuilder({ "#  ", "## ", "###" }, std::make_shared<ItemInstance>(Tile::stairsWood, 4))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder({ "  #", " #X", "# X" }, std::make_shared<ItemInstance>(Item::fishingRod, 1))
        .add('#', Item::stick)
        .add('X', Item::string));

    add(ShapedRecipeBuilder({ "#  ", "## ", "###" }, std::make_shared<ItemInstance>(Tile::stairsStone, 4))
        .add('#', Tile::cobblestone));

    add(ShapedRecipeBuilder({ "###", "#X#", "###" }, std::make_shared<ItemInstance>(Item::painting, 1))
        .add('#', Item::stick)
        .add('X', Tile::cloth));

    add(ShapedRecipeBuilder({ "###", "#X#", "###" }, std::make_shared<ItemInstance>(Item::goldApple, 1))
        .add('#', Tile::goldBlock)
        .add('X', Item::apple));

    add(ShapedRecipeBuilder({ "X", "#" }, std::make_shared<ItemInstance>(Tile::lever, 1))
        .add('X', Item::stick)
        .add('#', Tile::cobblestone));

    add(ShapedRecipeBuilder({ "X", "#" }, std::make_shared<ItemInstance>(Tile::redstoneTorchLit, 1))
        .add('X', Item::redStone)
        .add('#', Item::stick));

    add(ShapedRecipeBuilder({ "#X#", "III" }, std::make_shared<ItemInstance>(Item::diode, 1))
        .add('#', Tile::redstoneTorchLit)
        .add('X', Item::redStone)
        .add('I', Tile::stone));

    add(ShapedRecipeBuilder({ " # ", "#X#", " # " }, std::make_shared<ItemInstance>(Item::clock, 1))
        .add('#', Item::goldIngot)
        .add('X', Item::redStone));

    add(ShapedRecipeBuilder({ " # ", "#X#", " # " }, std::make_shared<ItemInstance>(Item::compass, 1))
        .add('#', Item::ironIngot)
        .add('X', Item::redStone));

    add(ShapedRecipeBuilder({ "#" }, std::make_shared<ItemInstance>(Tile::button, 1))
        .add('#', Tile::stone));

    add(ShapedRecipeBuilder({"###"}, std::make_shared<ItemInstance>(Tile::stonePressurePlate, 1))
        .add('#', Tile::stone));

    add(ShapedRecipeBuilder({"###"}, std::make_shared<ItemInstance>(Tile::woodPressurePlate, 1))
        .add('#', Tile::wood));

    add(ShapedRecipeBuilder({"###", "#X#", "#R#"}, std::make_shared<ItemInstance>(Tile::dispenser, 1))
        .add('#', Tile::cobblestone)
        .add('X', Item::bow)
        .add('R', Item::redStone));

    add(ShapedRecipeBuilder({ "###", "XXX" }, std::make_shared<ItemInstance>(Item::bed))
        .add('#', Tile::cloth)
        .add('X', Tile::wood));

    for (int i = 0; i < 16; ++i)
    {
        add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Tile::cloth, 1, ClothTile::getColorFromData(i)))
            .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, i))
            .add(std::make_shared<ItemInstance>(Tile::cloth, 1, 0)));
    }

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 2, 11))
        .add(Tile::flower));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 2, 1))
        .add(Tile::rose));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 3, 15))
        .add(Item::bone));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 2, 9))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 1))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 15)));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 2, 14))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 1))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 11)));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 2, 10))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 2))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 15)));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 2, 8))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 0))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 15)));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 2, 7))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 8))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 15)));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 3, 7))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 0))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 15))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 15)));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 2, 12))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 4))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 15)));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 2, 6))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 4))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 2)));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 2, 5))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 4))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 1)));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 2, 13))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 5))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 9)));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 3, 13))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 4))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 1))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 9)));

    add(ShapelessRecipeBuilder(std::make_shared<ItemInstance>(Item::dyePowder, 4, 13))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 4))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 1))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 1))
        .add(std::make_shared<ItemInstance>(Item::dyePowder, 1, 15)));

    std::sort(m_recipes.begin(), m_recipes.end(), [](const std::unique_ptr<Recipe>& a, const std::unique_ptr<Recipe>& b)
        {
            if (!b->isShaped() && b->isShaped()) {
                return false;
            }
            if (a->isShaped() && !b->isShaped()) {
                return true;
            }
            return a->size() > b->size();
        });

    LOG_I("%d recipes loaded", m_recipes.size());
}
