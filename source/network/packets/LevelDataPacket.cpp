/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"

void LevelDataPacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void LevelDataPacket::write(RakNet::BitStream* pbs)
{
}

void LevelDataPacket::read(RakNet::BitStream* bs)
{
}
