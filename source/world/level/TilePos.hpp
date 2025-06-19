#pragma once

#include <stdint.h>
#include "world/Facing.hpp"
#include "world/phys/Vec3T.hpp"

#ifndef __CHUNKPOS_HPP
struct ChunkPos;
#endif

#define __TILEPOS_HPP

struct TilePos
{
	int x;
	int y;
	int z;

private:
	void _init(int _x, int _y, int _z);

public:
	TilePos();
	TilePos(int _x, int _y, int _z);
	TilePos(int* xyz);
	TilePos(float _x, float _y, float _z);
	TilePos(double _x, double _y, double _z);
	TilePos(const Vec3& pos);
	TilePos(const ChunkPos& pos, int y);

	TilePos relative(Facing::Name facing, int steps = 1) const;

	TilePos above(int steps = 1) const { return relative(Facing::UP, steps); }
	TilePos below(int steps = 1) const { return relative(Facing::DOWN, steps); }
	TilePos north(int steps = 1) const { return relative(Facing::NORTH, steps); }
	TilePos south(int steps = 1) const { return relative(Facing::SOUTH, steps); }
	TilePos west(int steps = 1) const { return relative(Facing::WEST, steps); }
	TilePos east(int steps = 1) const { return relative(Facing::EAST, steps); }
	TilePos offset(int _x, int _y = 0, int _z = 0) const { return TilePos(x + _x, y + _y, z + _z); }

	Vec3 center() const;

	bool operator<(const TilePos& b) const;
	bool operator>(const TilePos& b) const;
	bool operator<=(const TilePos& b) const;
	bool operator>=(const TilePos& b) const;
	TilePos operator+(const TilePos& b) const;
	TilePos operator+(int i) const;
	Vec3 operator+(float f) const;
	TilePos operator-(const TilePos& b) const;
	TilePos operator-(int i) const;
	Vec3 operator-(float f) const;
	void operator+=(const TilePos& b);
	void operator-=(const TilePos& b);
	void operator*=(int i);
	TilePos operator-() const;
	TilePos operator*(int i) const;
	Vec3 operator*(float f) const;
	TilePos operator/(int i) const;
	Vec3 operator/(float f) const;
	bool operator==(const TilePos& b) const;
	bool operator!=(const TilePos& b) const;
	int operator[](uint8_t index) const;
	int& operator[](uint8_t index);
};

template<>
inline Vec3T<real>::Vec3T(const TilePos& tilePos)
	: x(tilePos.x), y(tilePos.y), z(tilePos.z) {
}

template<>
inline Vec3T<float>::Vec3T(const TilePos& tilePos)
	: x(tilePos.x), y(tilePos.y), z(tilePos.z) {
}