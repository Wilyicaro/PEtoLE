#pragma once

#include "world/entity/Entity.hpp"

class PacketUtil
{
public:
	static int8_t packRot(float degrees);
	static float unpackRot(int8_t charValue);
	static Vec3i packPos(const Vec3&);
	static Vec3 unpackPos(const Vec3i&);
	static Vec3i packMotion(const Vec3&);
	static Vec3 unpackMotion(const Vec3i&);
	static void Rot_entityToChar(const Entity* entity, char& yawChar, char& pitchChar);
	static void Rot_charToEntity(Entity* entity, char yawChar, char pitchChar);
};

