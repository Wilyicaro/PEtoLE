/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "SoundData.hpp"
#include "common/Random.hpp"

class SoundRepository
{
public:
	void add(const std::string& name, const SoundDesc& sd);
	bool get(const std::string& name, SoundDesc& sd);

	const SoundDesc& any();

public:
	std::unordered_map<std::string, std::vector<SoundDesc>> m_repo;
	std::vector<SoundDesc> m_all;

private:
	Random m_random;
};

