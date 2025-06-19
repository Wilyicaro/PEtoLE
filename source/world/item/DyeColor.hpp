#pragma once
#include <string>

class DyeColor
{
public:
	enum Name 
	{
		BLACK, RED, GREEN, BROWN, BLUE, PURPLE, CYAN, SILVER, GRAY, PINK, LIME, YELLOW, LIGHT_BLUE, MAGENTA, ORANGE, WHITE
	};

	static std::string getId(Name name)
	{
		return IDS[name];
	}

	static constexpr const char* IDS[16] = { "black", "red", "green", "brown", "blue", "purple", "cyan", "silver", "gray", "pink", "lime", "yellow", "lightblue", "magenta", "orange", "white" };
	static constexpr const int RGBS[16] = { 1973019, 11743532, 3887386, 5320730, 2437522, 8073150, 2651799, 2651799, 4408131, 14188952, 4312372, 14602026, 6719955, 12801229, 15435844, 15790320 };
};
