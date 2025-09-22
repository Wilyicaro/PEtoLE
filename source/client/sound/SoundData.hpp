/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <stdint.h>
#include "common/Utils.hpp"

struct PCMSoundHeader
{
	int m_channels;
	int m_bytes_per_sample;
	int m_sample_rate;
	int m_samples;
};

struct SoundBuffer
{
	void* m_pData;
	int m_dataSize;
};

struct SoundDesc
{
	std::string m_name;
	std::string m_path;
	short* m_pData;
	int m_size;
	PCMSoundHeader m_header;
	bool m_bIsStream;

	SoundDesc()
	{
		m_pData = nullptr;
		m_size = 0;
	}

	SoundDesc(std::string name, std::string path) : SoundDesc(name, path, {0, 0, 0, 0}, nullptr)
	{
		m_bIsStream = true;
	}

	SoundDesc(std::string name, std::string path, const PCMSoundHeader& header, short* data)
	{
		m_name = name;
		m_path = path;
		m_header = header;
		m_pData = data;
		m_size = header.m_channels * header.m_samples * header.m_bytes_per_sample;
		m_bIsStream = false;
	}
};
