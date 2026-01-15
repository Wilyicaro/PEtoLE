#pragma once

#include <stdint.h>
#include "world/Facing.hpp"
#include "world/phys/Vec3T.hpp"

#ifndef __CHUNKPOS_HPP
struct ChunkPos;
#endif

#define __TILEPOS_HPP

struct Vec3i
{
	int x;
	int y;
	int z;

private:
	void _init(int _x, int _y, int _z);

public:
	Vec3i();
	Vec3i(int _x, int _y, int _z);
	Vec3i(const int* xyz);
	Vec3i(float _x, float _y, float _z);
	Vec3i(double _x, double _y, double _z);
	Vec3i(const Vec3& pos);
	Vec3i(const ChunkPos& pos, int y);

	Vec3i relative(Facing::Name facing, int steps = 1) const;

	Vec3i above(int steps = 1) const { return relative(Facing::UP, steps); }
	Vec3i below(int steps = 1) const { return relative(Facing::DOWN, steps); }
	Vec3i north(int steps = 1) const { return relative(Facing::NORTH, steps); }
	Vec3i south(int steps = 1) const { return relative(Facing::SOUTH, steps); }
	Vec3i west(int steps = 1) const { return relative(Facing::WEST, steps); }
	Vec3i east(int steps = 1) const { return relative(Facing::EAST, steps); }
	Vec3i offset(int _x, int _y = 0, int _z = 0) const { return Vec3i(x + _x, y + _y, z + _z); }

	Vec3 center() const;

	real distanceTo(const Vec3i&) const;

	bool operator<(const Vec3i& b) const;
	bool operator>(const Vec3i& b) const;
	bool operator<=(const Vec3i& b) const;
	bool operator>=(const Vec3i& b) const;
	Vec3i operator+(const Vec3i& b) const;
	Vec3i operator+(int i) const;
	Vec3 operator+(float f) const;
	Vec3i operator-(const Vec3i& b) const;
	Vec3i operator-(int i) const;
	Vec3 operator-(float f) const;
	void operator+=(const Vec3i& b);
	void operator-=(const Vec3i& b);
	void operator*=(int i);
	Vec3i operator-() const;
	Vec3i operator*(int i) const;
	Vec3 operator*(float f) const;
	Vec3i operator/(int i) const;
	Vec3 operator/(float f) const;
	bool operator==(const Vec3i& b) const;
	bool operator!=(const Vec3i& b) const;
	int operator[](uint8_t index) const;
	int& operator[](uint8_t index);
};

using TilePos = Vec3i;

template<>
inline Vec3T<real>::Vec3T(const Vec3i& tilePos)
	: x(tilePos.x), y(tilePos.y), z(tilePos.z) {
}

template<>
inline Vec3T<float>::Vec3T(const Vec3i& tilePos)
	: x(tilePos.x), y(tilePos.y), z(tilePos.z) {
}

namespace std {
	template <>
	struct hash<Vec3i> {
		std::size_t operator()(const Vec3i& pos) const noexcept {
			return pos.x + (pos.y << 8) + (pos.z << 16);
		}
	};
}