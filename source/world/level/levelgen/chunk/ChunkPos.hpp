#pragma once

#include <functional>
#include "world/phys/Vec3T.hpp"
#ifndef __TILEPOS_HPP
struct Vec3i;
#endif

#define __CHUNKPOS_HPP

struct ChunkPos
{
	int x, z;
    
private:
    void _init(int _x, int _z);
    void _init(const Vec3& pos);
	void _init(const Vec3i& pos);
    
public:
	ChunkPos();
	ChunkPos(int _x, int _z);
	ChunkPos(int _x, int _y, int _z);
	ChunkPos(float _x, float _y, float _z);
	ChunkPos(const Vec3& pos) { _init(pos); }
	ChunkPos(const Vec3i& pos) { _init(pos); }

	bool operator<(const ChunkPos& b) const;
	bool operator>(const ChunkPos& b) const;
	bool operator<=(const ChunkPos& b) const;
	bool operator>=(const ChunkPos& b) const;
	ChunkPos operator+(const ChunkPos& b) const;
	ChunkPos operator+(int i) const;
	ChunkPos operator-(const ChunkPos& b) const;
	ChunkPos operator-(int i) const;
	void operator+=(const ChunkPos& b);
	void operator-=(const ChunkPos& b);
	void operator*=(int i);
	ChunkPos operator-() const;
	ChunkPos operator*(int i) const;
	ChunkPos operator/(int i) const;
	bool operator==(const ChunkPos& other) const;
	bool operator!=(const ChunkPos& other) const;

	operator Vec3i() const;

	static uint64_t createKey(int32_t x, int32_t z) noexcept {
		return (static_cast<uint64_t>(static_cast<uint32_t>(x)) << 32) | static_cast<uint32_t>(z);
	}

	uint64_t key() const noexcept {
		return createKey(x, z);
	}

	uint64_t regionKey() const noexcept {
		return createKey(x >> 5, z >> 5);
	}

	int regionIndex() const noexcept {
		return (x & 31) + (z & 31) * 32;
	}
};

namespace std {
	template <>
	struct hash<ChunkPos> {
		std::size_t operator()(const ChunkPos& pos) const noexcept {
			std::size_t h1 = std::hash<int>{}(pos.x);
			std::size_t h2 = std::hash<int>{}(pos.z);
			return h1 ^ (h2 << 1);
		}
	};
}