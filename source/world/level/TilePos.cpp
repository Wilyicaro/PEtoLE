#include "TilePos.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"

void TilePos::_init(int _x, int _y, int _z)
{
    x = _x;
    y = _y;
    z = _z;
}

TilePos::TilePos()
{
    _init(0, 0, 0);
}

TilePos::TilePos(int _x, int _y, int _z)
{
    _init(_x, _y, _z);
}

TilePos::TilePos(int* xyz)
{
	_init(xyz[0], xyz[1], xyz[2]);
}

TilePos::TilePos(float _x, float _y, float _z)
{
    _init(Mth::floor(_x), Mth::floor(_y), Mth::floor(_z));
}

TilePos::TilePos(double _x, double _y, double _z)
{
	_init(Mth::floor(_x), Mth::floor(_y), Mth::floor(_z));
}

TilePos::TilePos(const Vec3& pos)
{
    _init(Mth::floor(pos.x), Mth::floor(pos.y), Mth::floor(pos.z));
}

TilePos::TilePos(const ChunkPos& pos, int y)
{
    _init(pos.x * 16, y, pos.z * 16);
}

TilePos TilePos::relative(Facing::Name facing, int steps) const
{
	switch (facing)
	{
	case Facing::NORTH: return TilePos(x,         y,         z - steps);
	case Facing::SOUTH: return TilePos(x,         y,         z + steps);
	case Facing::UP:    return TilePos(x,         y + steps, z);
	case Facing::DOWN:  return TilePos(x,         y - steps, z);
	case Facing::WEST:  return TilePos(x - steps, y,         z);
	case Facing::EAST:  return TilePos(x + steps, y,         z);
	default:            return TilePos(*this);
	}
}


Vec3 TilePos::center() const {
	return Vec3(x + 0.5, y + 0.5, z + 0.5);
}

bool TilePos::operator<(const TilePos& b) const
{
	if (x != b.x)
		return x < b.x;
	if (y != b.y)
		return y < b.y;

	return z < b.z;
}

bool TilePos::operator>(const TilePos& b) const
{
	if (x != b.x)
		return x > b.x;
	if (y != b.y)
		return y > b.y;

	return z > b.z;
}

bool TilePos::operator<=(const TilePos& b) const
{
	return x <= b.x &&
		   y <= b.y &&
		   z <= b.z;
}

bool TilePos::operator>=(const TilePos& b) const
{
	return x >= b.x &&
		   y >= b.y &&
		   z >= b.z;
}

TilePos TilePos::operator+(const TilePos& b) const
{
	return TilePos(x + b.x, y + b.y, z + b.z);
}

TilePos TilePos::operator+(int i) const
{
	return TilePos(i + x, i + y, i + z);
}

Vec3 TilePos::operator+(float f) const
{
	return Vec3(*this) + f;
}

TilePos TilePos::operator-(const TilePos& b) const
{
	return TilePos(x - b.x, y - b.y, z - b.z);
}

TilePos TilePos::operator-(int i) const
{
	return TilePos(x - i, y - i, z - i);
}

Vec3 TilePos::operator-(float f) const
{
	return Vec3(*this) - f;
}

void TilePos::operator+=(const TilePos& b)
{
	x += b.x;
	y += b.y;
	z += b.z;
}

void TilePos::operator-=(const TilePos& b)
{
	(*this) += -b;
}

void TilePos::operator*=(int i)
{
	x *= i;
	y *= i;
	z *= i;
}

TilePos TilePos::operator-() const
{
	return TilePos(-x, -y, -z);
}

TilePos TilePos::operator*(int i) const
{
	return TilePos(x * i, y * i, z * i);
}

Vec3 TilePos::operator*(float f) const
{
	return Vec3(*this) * f;
}

TilePos TilePos::operator/(int i) const
{
	return TilePos(x / i, y / i, z / i);
}

Vec3 TilePos::operator/(float f) const
{
	return Vec3(*this) / f;
}

bool TilePos::operator==(const TilePos& b) const
{
	return x == b.x && y == b.y && z == b.z;
}

bool TilePos::operator!=(const TilePos& b) const
{
	// Dunno if the top is more optimal than the bottom when compiled
	//return x != b.x || z != b.z;
	return !(*this == b);
}

int TilePos::operator[](uint8_t index) const
{
	switch (index)
	{
	case 0: return x;
	case 1: return y;
	case 2: return z;
	default: throw std::out_of_range("TilePos index out of range");
	}
}

int& TilePos::operator[](uint8_t index)
{
	switch (index)
	{
	case 0: return x;
	case 1: return y;
	case 2: return z;
	default: throw std::out_of_range("TilePos index out of range");
	}
}
