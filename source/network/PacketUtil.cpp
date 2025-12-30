#include "PacketUtil.hpp"

int8_t PacketUtil::packRot(float degrees)
{
	return (degrees * 256.0f) / 360.0f;
}

float PacketUtil::unpackRot(int8_t charValue)
{
	return (charValue * 360.0f) / 256.0f;
}

Vec3i PacketUtil::packPos(const Vec3& pos)
{
	return pos * 32;
}

Vec3 PacketUtil::unpackPos(const Vec3i& pos)
{
	return pos / 32.0f;
}


Vec3i PacketUtil::packMotion(const Vec3& vel)
{
	const constexpr real maxVel = 3.9;
	return Vec3i(int(Mth::clamp(vel.x, -maxVel, maxVel) * 8000), int(Mth::clamp(vel.y, -maxVel, maxVel) * 8000), int(Mth::clamp(vel.z, -maxVel, maxVel) * 8000));
}

Vec3 PacketUtil::unpackMotion(const Vec3i& vel)
{
	return vel / 8000.0f;
}

void PacketUtil::Rot_entityToChar(const Entity* entity, char& yawChar, char& pitchChar)
{
	yawChar = packRot(entity->m_rot.y);
	pitchChar = packRot(entity->m_rot.x);
}

void PacketUtil::Rot_charToEntity(Entity* entity, char yawChar, char pitchChar)
{
	float pitch = PacketUtil::unpackRot(pitchChar);
	entity->m_rotPrev.x = pitch;
	entity->m_rot.x = pitch;

	float rot = PacketUtil::unpackRot(yawChar);
	entity->m_rotPrev.y = rot;
	entity->m_rot.y = rot;
}