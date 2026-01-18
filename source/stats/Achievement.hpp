#pragma once

#include "Stat.hpp"
#include "Achievements.hpp"
#include "world/item/ItemInstance.hpp"
#include "world/tile/Tile.hpp"

class Achievement : public Stat
{
public:
	Achievement(int id, const std::string name, int x, int y, Item*, Achievement*);
	Achievement(int id, const std::string name, int x, int y, Tile*, Achievement*);
	Achievement(int id, const std::string name, int x, int y, std::shared_ptr<ItemInstance>, Achievement*);

	bool isAchievement() const override
	{
		return true;
	}

	bool isChallenge() const
	{
		return m_bChallenge;
	}

	std::string getDescription()
	{
		return m_descriptionFormatter(m_description);
	}

	Achievement* root()
	{
		m_bRoot = true;
		return this;
	}

	Achievement* challenge()
	{
		m_bChallenge = true;
		return this;
	}

	Achievement* registerAchievement()
	{
		Stat::registerStat();
		Achievements::achievements.push_back(this);
		return this;
	}

	Achievement* descriptionFormatter(std::function<std::string(const std::string&)> formatter)
	{
		m_descriptionFormatter = formatter;
		return this;
	}

public:
	const int m_x;
	const int m_y;
	Achievement* m_pParent;
	const std::shared_ptr<ItemInstance> m_icon;


private:
	const std::string m_description;
	std::function<std::string(const std::string&)> m_descriptionFormatter;
	bool m_bChallenge;
};