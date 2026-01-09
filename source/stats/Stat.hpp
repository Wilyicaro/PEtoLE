#pragma once

#include "StatFormatter.hpp"

class Stat
{
public:
	//Use a static instance for the formatter!
	Stat(int id, const std::string&, const StatFormatter& = NumberFormatter::INSTANCE);

	virtual std::string format(int value) const
	{
		return m_pFormatter->format(value);
	}

	const std::string& getName() const
	{
		return m_name;
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

	Stat* registerStat()
	{
		return this;
	}

public:
	const int m_id;
	bool m_bRoot;

private:
	const std::string m_name;
	const StatFormatter* m_pFormatter;
};