#pragma once

#include "StatFormatter.hpp"
#include "AchievementMap.hpp"
#include "Stats.hpp"
#include <functional>

class Stat
{
public:
	//Use a static instance for the formatter!
	Stat(int id, const std::string&, const StatFormatter& = NumberFormatter::INSTANCE);

	virtual std::string format(int value) const
	{
		return m_pFormatter->format(value);
	}

	std::string getName() const
	{
		return m_nameFormatter(m_name);
	}

	const std::string& getUUID()
	{
		return AchievementMap::getInstance()->get(m_id);
	}

	virtual bool isAchievement() const
	{
		return false;
	}

	Stat* root()
	{
		m_bRoot = true;
		return this;
	}

	virtual Stat* registerStat()
	{
		Stats::stats.push_back(this);
		Stats::statById[m_id] = this;
		return this;
	}

	Stat* setNameFormatter(std::function<std::string(const std::string&)> formatter)
	{
		m_nameFormatter = formatter;
		return this;
	}

public:
	const int m_id;
	//For some weird reason, "root" stats are controlled by the client, not sure if this really should be named like that
	bool m_bRoot;

private:
	const std::string m_name;
	const StatFormatter* m_pFormatter;
	std::function<std::string(const std::string&)> m_nameFormatter;
};

class IdStat : public Stat
{
public:
	//Use a static instance for the formatter!
	IdStat(int id, const std::string&, int typeId, const StatFormatter & = NumberFormatter::INSTANCE);

	int getTypeId() const;

private:
	const int m_typeId;
};