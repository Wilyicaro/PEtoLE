#include "Vec3i.hpp"
#include "world/level/levelgen/chunk/ChunkPos.hpp"

void Vec3i::_init(int _x, int _y, int _z)
{
    x = _x;
    y = _y;
    z = _z;
}

Vec3i::Vec3i()
{
    _init(0, 0, 0);
}

Vec3i::Vec3i(int _x, int _y, int _z)
{
    _init(_x, _y, _z);
}

Vec3i::Vec3i(const int* xyz)
{
	_init(xyz[0], xyz[1], xyz[2]);
}

Vec3i::Vec3i(float _x, float _y, float _z)
{
    _init(Mth::floor(_x), Mth::floor(_y), Mth::floor(_z));
}

Vec3i::Vec3i(double _x, double _y, double _z)
{
	_init(Mth::floor(_x), Mth::floor(_y), Mth::floor(_z));
}

Vec3i::Vec3i(const Vec3& pos)
{
    _init(Mth::floor(pos.x), Mth::floor(pos.y), Mth::floor(pos.z));
}

Vec3i::Vec3i(const ChunkPos& pos, int y)
{
    _init(pos.x * 16, y, pos.z * 16);
}

Vec3i Vec3i::relative(Facing::Name facing, int steps) const
{
	switch (facing)
	{
	case Facing::NORTH: return Vec3i(x,         y,         z - steps);
	case Facing::SOUTH: return Vec3i(x,         y,         z + steps);
	case Facing::UP:    return Vec3i(x,         y + steps, z);
	case Facing::DOWN:  return Vec3i(x,         y - steps, z);
	case Facing::WEST:  return Vec3i(x - steps, y,         z);
	case Facing::EAST:  return Vec3i(x + steps, y,         z);
	default:            return Vec3i(*this);
	}
}


Vec3 Vec3i::center() const {
	return Vec3(x + 0.5, y + 0.5, z + 0.5);
}

bool Vec3i::operator<(const Vec3i& b) const
{
	if (x != b.x)
		return x < b.x;
	if (y != b.y)
		return y < b.y;

	return z < b.z;
}

bool Vec3i::operator>(const Vec3i& b) const
{
	if (x != b.x)
		return x > b.x;
	if (y != b.y)
		return y > b.y;

	return z > b.z;
}

bool Vec3i::operator<=(const Vec3i& b) const
{
	return x <= b.x &&
		   y <= b.y &&
		   z <= b.z;
}

bool Vec3i::operator>=(const Vec3i& b) const
{
	return x >= b.x &&
		   y >= b.y &&
		   z >= b.z;
}

Vec3i Vec3i::operator+(const Vec3i& b) const
{
	return Vec3i(x + b.x, y + b.y, z + b.z);
}

Vec3i Vec3i::operator+(int i) const
{
	return Vec3i(i + x, i + y, i + z);
}

Vec3 Vec3i::operator+(float f) const
{
	return Vec3(*this) + f;
}

Vec3i Vec3i::operator-(const Vec3i& b) const
{
	return Vec3i(x - b.x, y - b.y, z - b.z);
}

Vec3i Vec3i::operator-(int i) const
{
	return Vec3i(x - i, y - i, z - i);
}

Vec3 Vec3i::operator-(float f) const
{
	return Vec3(*this) - f;
}

void Vec3i::operator+=(const Vec3i& b)
{
	x += b.x;
	y += b.y;
	z += b.z;
}

void Vec3i::operator-=(const Vec3i& b)
{
	(*this) += -b;
}

void Vec3i::operator*=(int i)
{
	x *= i;
	y *= i;
	z *= i;
}

Vec3i Vec3i::operator-() const
{
	return Vec3i(-x, -y, -z);
}

Vec3i Vec3i::operator*(int i) const
{
	return Vec3i(x * i, y * i, z * i);
}

Vec3 Vec3i::operator*(float f) const
{
	return Vec3(*this) * f;
}

Vec3i Vec3i::operator/(int i) const
{
	return Vec3i(x / i, y / i, z / i);
}

Vec3 Vec3i::operator/(float f) const
{
	return Vec3(*this) / f;
}

bool Vec3i::operator==(const Vec3i& b) const
{
	return x == b.x && y == b.y && z == b.z;
}

bool Vec3i::operator!=(const Vec3i& b) const
{
	// Dunno if the top is more optimal than the bottom when compiled
	//return x != b.x || z != b.z;
	return !(*this == b);
}

int Vec3i::operator[](uint8_t index) const
{
	switch (index)
	{
	case 0: return x;
	case 1: return y;
	case 2: return z;
	default: throw std::out_of_range("TilePos index out of range");
	}
}

int& Vec3i::operator[](uint8_t index)
{
	switch (index)
	{
	case 0: return x;
	case 1: return y;
	case 2: return z;
	default: throw std::out_of_range("TilePos index out of range");
	}
}
