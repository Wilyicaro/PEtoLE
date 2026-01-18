/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "../Packet.hpp"
#include "world/level/levelgen/chunk/LevelChunk.hpp"

BlockRegionUpdatePacket::BlockRegionUpdatePacket(Level* level, const TilePos& minPos, int xs, int ys, int zs) : m_pos(minPos), m_xs(xs), m_ys(ys), m_zs(zs)
{
	uint8_t* rawData = level->getBlocksAndData(minPos, xs, ys, zs);
	size_t compressedSize = 0;
	uint8_t* compressed = ZlibDeflateToMemory(rawData, xs * ys * zs * 5 / 2, &compressedSize);
	m_size = compressedSize;
	m_data.WriteAlignedBytes(compressed, m_size);
	delete[] compressed;
}

void BlockRegionUpdatePacket::handle(const RakNet::RakNetGUID& guid, NetEventCallback* pCallback)
{
	pCallback->handle(guid, this);
}

void BlockRegionUpdatePacket::write(RakNet::BitStream* bs)
{
	bs->Write((unsigned char)PACKET_BLOCK_REGION_UPDATE);
	bs->Write(m_pos.x);
	bs->Write(m_pos.y);
	bs->Write(m_pos.z);
	bs->Write(m_xs);
	bs->Write(m_ys);
	bs->Write(m_zs);
	bs->Write(m_size);
	bs->WriteAlignedBytes(m_data.GetData(), m_size);
}

void BlockRegionUpdatePacket::read(RakNet::BitStream* bs)
{
	bs->Read(m_pos.x);
	bs->Read(m_pos.y);
	bs->Read(m_pos.z);
	bs->Read(m_xs);
	bs->Read(m_ys);
	bs->Read(m_zs);
	bs->Read(m_size);

	uint8_t* compressed = new uint8_t[m_size];
	bs->ReadAlignedBytes(compressed, m_size);

	uint8_t* decompressed = ZlibInflateToMemory(compressed, m_size, m_xs * m_ys * m_zs * 5 / 2);
	if (decompressed)
	{
		m_data.Reset();
		m_data.WriteAlignedBytes(decompressed, m_xs * m_ys * m_zs * 5 / 2);
		m_data.ResetReadPointer();
	}

	delete[] compressed;
	SAFE_DELETE_ARRAY(decompressed);
}
