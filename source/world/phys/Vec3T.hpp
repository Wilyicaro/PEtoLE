/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "common/Mth.hpp"
// Needed for when we're missing nullptr in multiple files
#include "compat/LegacyCPPCompatibility.hpp"


struct Vec3i;

enum class Axis {
	X, Y, Z
};

template<typename T>
class Vec3T
{
public:
	static const Vec3T<T> ZERO;

public:
	T x, y, z;

public:
	Vec3T() : x(0), y(0), z(0) {}
	Vec3T(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
	Vec3T(const Vec3i& vec3i);

	Vec3T interpolateTo(const Vec3T& to, T t) const 
	{
		T nx = x + (to.x - x) * t;
		T ny = y + (to.y - y) * t;
		T nz = z + (to.z - z) * t;
		return Vec3T(nx, ny, nz);
	}

	Vec3T vectorTo(const Vec3T& to) const 
	{
		return Vec3T(to.x - x, to.y - y, to.z - z);
	}

	Vec3T normalize() const 
	{
		T dist = length();
		if (dist < 0.0001)
			return ZERO;

		return Vec3T(x / dist, y / dist, z / dist);
	}

	T dot(const Vec3T& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	Vec3T cross(const Vec3T& other) const 
	{
		return Vec3T(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
	}

	Vec3T add(T x, T y, T z) const 
	{
		return Vec3T(this->x + x, this->y + y, this->z + z);
	}

	T distanceTo(const Vec3T& b) const
	{
		return Mth::sqrt(distanceToSqr(b));
	}

	T distanceToSqr(const Vec3T& b) const 
	{
		Vec3T d = *this - b;

		return d.x * d.x + d.y * d.y + d.z * d.z;
	}
    
	bool clipX(const Vec3T& a2, T a3, Vec3T& a4) const
	{
		T delta = a2.x - this->x;
		if (delta * delta < 0.000001f)
			return false;

		T ratio = (a3 - this->x) / delta;
		if (ratio < 0.0f || ratio > 1.0f)
			return false;

		a4.x = x + (a2.x - x) * ratio;
		a4.y = y + (a2.y - y) * ratio;
		a4.z = z + (a2.z - z) * ratio;
		return true;
	}

	bool clipY(const Vec3T& a2, T a3, Vec3T& a4) const
	{
		T delta = a2.y - this->y;
		if (delta * delta < 0.000001f)
			return false;

		T ratio = (a3 - this->y) / delta;
		if (ratio < 0.0f || ratio > 1.0f)
			return false;

		a4.x = x + (a2.x - x) * ratio;
		a4.y = y + (a2.y - y) * ratio;
		a4.z = z + (a2.z - z) * ratio;
		return true;
	}

	bool clipZ(const Vec3T& a2, T a3, Vec3T& a4) const
	{
		T delta = a2.z - this->z;
		if (delta * delta < 0.000001f)
			return false;

		T ratio = (a3 - this->z) / delta;
		if (ratio < 0.0f || ratio > 1.0f)
			return false;

		a4.x = x + (a2.x - x) * ratio;
		a4.y = y + (a2.y - y) * ratio;
		a4.z = z + (a2.z - z) * ratio;
		return true;
	}

	void xRot(float rot) {
		float cos = Mth::cos(rot);
		float sin = Mth::sin(rot);
		T y0 = y;
		y = (y * cos) + (z * sin);
		z = (z * cos) - (y0 * sin);
	}

	void yRot(float rot) {
		float cos = Mth::cos(rot);
		float sin = Mth::sin(rot);
		T x0 = x;
		x = (x * cos) + (z * sin);
		z = (z * cos) - (x0 * sin);
	}

	void zRot(float rot) {
		float cos = Mth::cos(rot);
		float sin = Mth::sin(rot);
		T x0 = x;
		x = (x * cos) + (y * sin);
		y = (y * cos) - (x0 * sin);
	}

	Vec3T operator+(const Vec3T& b) const
	{
		return Vec3T(x + b.x, y + b.y, z + b.z);
	}

	Vec3T operator+(T f) const
	{
		return *this + Vec3T(f, f, f);
	}

	Vec3T operator-(const Vec3T& b) const
	{
		return Vec3T(x - b.x, y - b.y, z - b.z);
	}

	void operator+=(const Vec3T& b)
	{
		x += b.x;
		y += b.y;
		z += b.z;
	}

	void operator-=(const Vec3T& b)
	{
		(*this) += -b;
	}

	void operator+=(T f)
	{
		x += f;
		y += f;
		z += f;
	}

	void operator-=(T f)
	{
		x -= f;
		y -= f;
		z -= f;
	}

	void operator*=(T f)
	{
		x *= f;
		y *= f;
		z *= f;
	}

	Vec3T operator-() const
	{
		return Vec3T(-x, -y, -z);
	}

	Vec3T operator-(T f) const
	{
		return Vec3T(x - f, y - f, z - f);
	}

	Vec3T operator*(T f) const
	{
		return Vec3T(x * f, y * f, z * f);
	}

	Vec3T operator/(T f) const
	{
		return Vec3T(x / f, y / f, z / f);
	}

	bool operator==(const Vec3T& b) const
	{
		return x == b.x &&
			   y == b.y &&
			   z == b.z;
	}

	Vec3T translate(T tx, T ty, T tz) const
	{
		return Vec3T(x + tx, y + ty, z + tz);
	}

	T lengthSqr() const
	{
		return x * x + y * y + z * z;
	}

	T length() const
	{
		return Mth::sqrt(lengthSqr());
	}

	Vec3T scale(T scale) const
	{
		return Vec3T(x * scale, y * scale, z * scale);
	}

	//@NOTE: LCE Vec3 methods, not sure if they were used at some point in JE
	Vec3T closestPointOnSegment(const Vec3T& segStart, const Vec3T& segEnd)
	{
		Vec3T v1 = *this - segStart;
		Vec3T v2 = segEnd - segStart;

		T dot = v1.dot(v2);
		if (dot <= 0.0)
			return segStart;

		T len2 = v2.dot(v2);
		if (len2 <= dot)
			return segEnd;

		T t = dot / len2;
		return segStart + v2 * t;
	}

	T distanceToSegment(const Vec3T& segStart, const Vec3T& segEnd)
	{
		Vec3T diff = (*this - closestPointOnSegment(segStart, segEnd));
		return diff.length();
	}
};

template<typename T>
const Vec3T<T> Vec3T<T>::ZERO = Vec3T<T>(0, 0, 0);

using real = double;

using Vec3 = Vec3T<real>;
using Vec3f = Vec3T<float>;
