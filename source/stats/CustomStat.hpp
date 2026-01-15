#pragma once

#include "Stat.hpp"

class CustomStat : public Stat
{
public:
	CustomStat(int id, const std::string&, const StatFormatter& = NumberFormatter::INSTANCE);

	CustomStat* registerStat()
	{
		Stat::registerStat();
		Stats::customStats.push_back(this);
		return this;
	}
};