#pragma once

#include <vector>

class Achievement;

class Achievements
{
public:
	static void initAchievements();

	static Achievement* openInventory;
	static Achievement* mineWood;
	static Achievement* buildWorkBench;
	static Achievement* buildPickaxe;
	static Achievement* buildFurnace;
	static Achievement* acquireIron;
	static Achievement* buildHoe;
	static Achievement* makeBread;
	static Achievement* bakeCake;
	static Achievement* buildBetterPickaxe;
	static Achievement* cookFish;
	static Achievement* onARail;
	static Achievement* buildSword;
	static Achievement* killEnemy;
	static Achievement* killCow;
	static Achievement* flyPig;

public:
	static std::vector<Achievement*> achievements;
	static int minX, minY, maxX, maxY;
};