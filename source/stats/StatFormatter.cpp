#include "StatFormatter.hpp"
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>

std::string DistanceFormatter::format(int value) const
{
	double meters = value / 100.0;
	double km = meters / 1000.0;
	return km > 0.5 ? DecimalFormatter::INSTANCE.format(km) + " km" : (meters > 0.5 ? DecimalFormatter::INSTANCE.format(meters) + " m" : std::to_string(value) + " cm");
}

const DistanceFormatter DistanceFormatter::INSTANCE;

std::string DateFormatter::format(int value) const
{
	double secs = value / 20.0;
	double mins = secs / 60.0;
	double hours = mins / 60.0;
	double days = hours / 24.0;
	double years = days / 365.0;
	return years > 0.5 ? DecimalFormatter::INSTANCE.format(years) + " y" : (days > 0.5 ? DecimalFormatter::INSTANCE.format(days) + " d" : (hours > 0.5 ? DecimalFormatter::INSTANCE.format(hours) + " h" : (mins > 0.5 ? DecimalFormatter::INSTANCE.format(mins) + " m" : std::to_string(secs) + " s")));
}

const DateFormatter DateFormatter::INSTANCE;

std::string NumberFormatter::format(int value) const
{
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << value;
	return ss.str();
}

const NumberFormatter NumberFormatter::INSTANCE;

DecimalFormatter::DecimalFormatter(int precision) : m_precision(precision)
{
}

std::string DecimalFormatter::format(double value) const
{
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << std::fixed << std::setprecision(m_precision) << value;
	return ss.str();
}

const DecimalFormatter DecimalFormatter::INSTANCE;
