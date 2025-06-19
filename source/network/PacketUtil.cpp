#include "PacketUtil.hpp"

char PacketUtil::Rot_degreesToChar(float degrees)
{
	return (degrees / 360.0f) * 256.0f;
}

float PacketUtil::Rot_charToDegrees(char charValue)
{
	return (charValue * 0.0039062f) * 360.0f;
}

void PacketUtil::Rot_entityToChar(const Entity* entity, char& yawChar, char& pitchChar)
{
	yawChar = Rot_degreesToChar(entity->m_rot.y);
	pitchChar = Rot_degreesToChar(entity->m_rot.x);
}

void PacketUtil::Rot_charToEntity(Entity* entity, char yawChar, char pitchChar)
{
	float pitch = PacketUtil::Rot_charToDegrees(pitchChar);
	entity->m_rotPrev.x = pitch;
	entity->m_rot.x = pitch;

	float yaw = PacketUtil::Rot_charToDegrees(yawChar);
	entity->m_rotPrev.y = yaw;
	entity->m_rot.y = yaw;
}