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

struct SoundDesc
{
	short* m_pData;
	int field_4;
	PCMSoundHeader m_header;

	SoundDesc()
	{
		m_pData = nullptr;
		field_4 = 0;
	}
	SoundDesc(const PCMSoundHeader& header, short* data)
	{
		m_header = header;
		m_pData = data;
		field_4 = header.m_channels * header.m_samples * header.m_bytes_per_sample;
	}
};
