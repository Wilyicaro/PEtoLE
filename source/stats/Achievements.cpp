#include "Achievement.hpp"

std::vector<Achievement*> Achievements::achievements;

Achievement* Achievements::openInventory;
Achievement* Achievements::mineWood;
Achievement* Achievements::buildWorkBench;
Achievement* Achievements::buildPickaxe;
Achievement* Achievements::buildFurnace;
Achievement* Achievements::acquireIron;
Achievement* Achievements::buildHoe;
Achievement* Achievements::makeBread;
Achievement* Achievements::bakeCake;
Achievement* Achievements::buildBetterPickaxe;
Achievement* Achievements::cookFish;
Achievement* Achievements::onARail;
Achievement* Achievements::buildSword;
Achievement* Achievements::killEnemy;
Achievement* Achievements::killCow;
Achievement* Achievements::flyPig;

int Achievements::minX = 0;
int Achievements::minY = 0;
int Achievements::maxX = 0;
int Achievements::maxY = 0;

void Achievements::initAchievements()
{
	openInventory = (new Achievement(0, "openInventory", 0, 0, Item::book, nullptr))->root()->registerAchievement();
	mineWood = (new Achievement(1, "mineWood", 2, 1, Tile::treeTrunk, openInventory))->registerAchievement();
	buildWorkBench = (new Achievement(2, "buildWorkBench", 4, -1, Tile::craftingTable, mineWood))->registerAchievement();
	buildPickaxe = (new Achievement(3, "buildPickaxe", 4, 2, Item::woodPickaxe, buildWorkBench))->registerAchievement();
	buildFurnace = (new Achievement(4, "buildFurnace", 3, 4, Tile::furnaceLit, buildPickaxe))->registerAchievement();
	acquireIron = (new Achievement(5, "acquireIron", 1, 4, Item::ironIngot, buildFurnace))->registerAchievement();
	buildHoe = (new Achievement(6, "buildHoe", 2, -3, Item::woodHoe, buildWorkBench))->registerAchievement();
	makeBread = (new Achievement(7, "makeBread", -1, -3, Item::bread, buildHoe))->registerAchievement();
	bakeCake = (new Achievement(8, "bakeCake", 0, -5, Item::cake, buildHoe))->registerAchievement();
	buildBetterPickaxe = (new Achievement(9, "buildBetterPickaxe", 6, 2, Item::stonePickaxe, buildPickaxe))->registerAchievement();
	cookFish = (new Achievement(10, "cookFish", 2, 6, Item::cookedFish, buildFurnace))->registerAchievement();
	onARail = (new Achievement(11, "onARail", 2, 3, Tile::rail, acquireIron))->challenge()->registerAchievement();
	buildSword = (new Achievement(12, "buildSword", 6, -1, Item::woodSword, buildWorkBench))->registerAchievement();
	killEnemy = (new Achievement(13, "killEnemy", 8, -1, Item::bone, buildSword))->registerAchievement();
	killCow = (new Achievement(14, "killCow", 7, -3, Item::leather, buildSword))->registerAchievement();
	flyPig = (new Achievement(15, "flyPig", 8, -4, Item::saddle, killCow))->challenge()->registerAchievement();

	LOG_I("%d achievements", achievements.size());
}
