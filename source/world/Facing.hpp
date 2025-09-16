#pragma once

#include "world/phys/Vec3T.hpp"

class Facing
{
public:
	enum Name
	{
		DOWN,  // -Y
		UP,    // +Y
		NORTH, // -Z
		SOUTH, // +Z
		WEST,  // -X
		EAST,  // +X
	};

	static constexpr const Name ALL[6] = { DOWN, UP, NORTH, SOUTH, WEST, EAST };

	static constexpr const Name OPPOSITE[6] = { UP, DOWN, SOUTH, NORTH, EAST, WEST };

	static constexpr const Name CLOCKWISE[6] = { UP, DOWN, NORTH, EAST, WEST, SOUTH };

	static constexpr const Name HORIZONTAL[4] = { NORTH, SOUTH, WEST, EAST };

	static constexpr const Name VERTICAL[4] = { DOWN, UP };

	static bool isVertical(Name facing)
	{
		return facing == DOWN || facing == UP;
	}

	static bool isHorizontal(Name facing)
	{
		return !isVertical(facing);
	}

	static bool isPositive(Name facing)
	{
		return facing == UP || facing == SOUTH || facing == EAST;
	}

	static constexpr const int CORNERS[8][3] = {
		{0, 1, 2},
		{3, 1, 2},
		{3, 4, 2},
		{0, 4, 2},
		{0, 1, 5},
		{3, 1, 5},
		{3, 4, 5},
		{0, 4, 5},
	};

	static constexpr const int VERTICES[6][4] = {
		{4, 0, 1, 5},
		{6, 2, 3, 7},
		{3, 2, 1, 0},
		{7, 4, 5, 6},
		{7, 3, 0, 4},
		{5, 1, 2, 6}
	};

	static constexpr const int NORMALS[6][3] = {
		{0, -1,  0},
		{0,  1,  0},
		{0,  0, -1},
		{0,  0,  1},
		{-1,  0,  0},
		{1,  0,  0}
	};

	static constexpr const int UVS[6][4][2] = {
		{ {0, 1}, {0, 0}, {1, 0}, {1, 1} },
		{ {1, 1}, {1, 0}, {0, 0}, {0, 1} },
		{ {1, 0}, {0, 0}, {0, 1}, {1, 1} },
		{ {0, 0}, {0, 1}, {1, 1}, {1, 0} },
		{ {1, 0}, {0, 0}, {0, 1}, {1, 1} },
		{ {0, 1}, {1, 1}, {1, 0}, {0, 0} }
	};

	static constexpr const int UV_AXES[3][4] = {
		{0, 3, 2, 5},
		{0, 3, 1, 4},
		{2, 5, 1, 4}
	};

	static constexpr const int ROTATED_INDEX[4][4] = {
		{0, 1, 2, 3}, // 0°
		{1, 2, 3, 0}, // 90°
		{3, 0, 1, 2},  // 270°
		{2, 3, 0, 1} // 180°
	};

	static constexpr const float LIGHT[] = { 0.5f, 1.0f, 0.8f, 0.8f, 0.6f, 0.6f };
};