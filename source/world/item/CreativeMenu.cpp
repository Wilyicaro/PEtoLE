#include "CreativeMenu.hpp"
#include "Slot.hpp"
#include "world/tile/Sapling.hpp"
#include "world/tile/RecordPlayerTile.hpp"

CreativeMenu::CreativeMenu(Container* inventory, Container* container)
    : m_container(container)
{

	Tile* var2[] = {Tile::cobblestone, Tile::stone, Tile::diamondOre, Tile::goldOre, Tile::ironOre, Tile::coalOre, Tile::lapisOre, Tile::redstoneOre, Tile::clay, Tile::diamondBlock, Tile::goldBlock, Tile::ironBlock, Tile::bedrock, Tile::lapisBlock, Tile::brick, Tile::mossStone, Tile::stoneSlabHalf, Tile::stoneSlabHalf, Tile::stoneSlabHalf, Tile::stoneSlabHalf, Tile::obsidian, Tile::netherrack, Tile::soulSand, Tile::glowstone, Tile::treeTrunk, Tile::treeTrunk, Tile::treeTrunk, Tile::leaves, Tile::dirt, Tile::grass, Tile::sand, Tile::sandStone, Tile::gravel, Tile::web, Tile::wood, Tile::sapling, Tile::sapling, Tile::sapling, Tile::deadBush, Tile::sponge, Tile::ice, Tile::snow, Tile::flower, Tile::rose, Tile::mushroom1, Tile::mushroom2, Tile::reeds, Tile::cactus, Tile::pumpkin, Tile::pumpkinLantern, Tile::chest, Tile::craftingTable, Tile::glass, Tile::tnt, Tile::bookshelf, Tile::cloth, Tile::cloth, Tile::cloth, Tile::cloth, Tile::cloth, Tile::cloth, Tile::cloth, Tile::cloth, Tile::cloth, Tile::cloth, Tile::cloth, Tile::cloth, Tile::cloth, Tile::cloth, Tile::cloth, Tile::cloth, Tile::dispenser, Tile::furnace, Tile::musicBlock, Tile::recordPlayer, /*Tile::stickyPiston, Tile::piston,*/ Tile::fence, Tile::ladder, Tile::rail, Tile::poweredRail, Tile::detectorRail, Tile::torch, Tile::stairsWood, Tile::stairsStone, Tile::lever, Tile::stonePressurePlate, Tile::woodPressurePlate, Tile::redstoneTorchLit, Tile::button, Tile::cake, Tile::trapDoor};
	int var3 = 0;
	int var4 = 0;
	int var5 = 0;
	int var6 = 0;
	int var7 = 0;


	int var9;
	for (Tile* tile : var2)
	{
		var9 = 0;
		if (tile == Tile::cloth)
			var9 = var3++;
		else if (tile == Tile::stoneSlabHalf)
			var9 = var4++;
		else if (tile == Tile::treeTrunk)
			var9 = var5++;
		else if (tile == Tile::sapling)
			var9 = var6++;
		creativeItems.push_back(std::make_shared<ItemInstance>(tile, 1, var9));
	}

	for (int var8 = 256; var8 < C_MAX_ITEMS; ++var8) {
		if (Item::items[var8]) {
			creativeItems.push_back(std::make_shared<ItemInstance>(Item::items[var8]));
		}
	}

	for (int var8 = 1; var8 < 16; ++var8) {
		creativeItems.push_back(std::make_shared<ItemInstance>(Item::dyePowder->m_itemID, 1, var8));
	}

    int rows = m_container->getContainerSize() / 8;
    int verticalOffset = (rows - 4) * 18;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < 8; ++col) {
            int index = col + row * 8;
            int x = 8 + col * 18;
            int y = 18 + row * 18;
            addSlot(std::make_shared<Slot>(m_container, index, x, y));
        }
    }

    for (int col = 0; col < 9; ++col) {
        addSlot(std::make_shared<Slot>(inventory, col, 8 + col * 18, 184));
    }

	updateScroll(0.0f);
}

void CreativeMenu::updateScroll(float scroll)
{
	int var2 = creativeItems.size() / 8 - 8 + 1;
	int var3 = (int)((scroll * var2) + 0.5f);
	if (var3 < 0) {
		var3 = 0;
	}

	for (int var4 = 0; var4 < 9; ++var4) {
		for (int var5 = 0; var5 < 8; ++var5) {
			int var6 = var5 + (var4 + var3) * 8;
			if (var6 >= 0 && var6 < creativeItems.size()) {
				m_container->setItem(var5 + var4 * 8, creativeItems[var6]);
			}
			else {
				m_container->setItem(var5 + var4 * 8, nullptr);
			}
		}
	}
}

bool CreativeMenu::stillValid(Player* player) const
{
    return true;
}
