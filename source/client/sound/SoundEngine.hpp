/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "client/options/Options.hpp"
#include "client/app/AppPlatform.hpp"
#include "common/Random.hpp"
#include "world/phys/Vec3T.hpp"
#include "SoundSystem.hpp"
#include "SoundRepository.hpp"

class SoundEngine
{
private:
	float _getVolumeMult(const Vec3& pos);
public:
	SoundEngine(SoundSystem* soundSystem);
	void init(Options* options, AppPlatform*);
	void addLocalSound(std::string name, std::string path);
	void enable(bool b);
	void updateOptions();
	void mute();
	void unMute();
	void destroy();
	void play(const std::string& name, const Vec3& pos = Vec3::ZERO, float volume = 1.0f, float pitch = 1.0f);

public:
	SoundSystem* m_pSoundSystem;
	Options* m_pOptions;
	AppPlatform* m_pPlatform;
	int field_40;
	//Random m_random;
	SoundRepository m_repository;
	int field_A1C;
	int field_A20;
	bool m_muted;
};

