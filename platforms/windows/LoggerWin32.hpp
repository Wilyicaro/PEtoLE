#pragma once

#include <string>
#include "common/Logger.hpp"

class LoggerWin32 : Logger
{
	void print(eLogLevel, const char* const str) override;
	void print(eLogLevel, std::string str) override;
};