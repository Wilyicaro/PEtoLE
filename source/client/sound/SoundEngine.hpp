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
#include "world/entity/Mob.hpp"
#include <vector>

class SoundEngine
{
private:
	float _getVolumeMult(const Vec3& pos);
public:
	SoundEngine(SoundSystem* soundSystem, float distance = 20.0f);
	void init(Options* options, AppPlatform*);
	void addLocalSound(SoundRepository& repo, std::string name, std::string path, bool streaming = false);
	void addSound(std::string name, std::string path, bool streaming = false);
	void addStreamSound(std::string name, std::string path);
	void addMusic(std::string name);
	void enable(bool b);
	void updateOptions();
	void mute();
	void unMute();
	void destroy();
	void play(const std::string& name, const Vec3& pos = Vec3::ZERO, float volume = 1.0f, float pitch = 1.0f);
	void playDesc(const SoundDesc& desc, const Vec3& pos = Vec3::ZERO, float volume = 1.0f, float pitch = 1.0f, bool isUI = false);
	void playUI(const std::string& name, float volume = 1.0f, float pitch = 1.0f);
	void playStreaming(const std::string& name, const Vec3& pos = Vec3::ZERO, float volume = 1.0f, float pitch = 1.0f);
	void playMusicTick();
	void update(const Mob*, float);

public:
	SoundSystem* m_pSoundSystem;
	Options* m_pOptions;
	AppPlatform* m_pPlatform;
	int field_40;
	Random m_random;
	SoundRepository m_sounds;
	SoundRepository m_songs;
	SoundRepository m_streamingSounds;
	int field_A1C;
	int field_A20;
	int m_bNoMusicDelay;
	bool m_muted;
	std::string m_lastStreaming;
	std::string m_lastMusic;
	Vec3 m_listenerPosition;
	Vec2 m_listenerOrientation;
	float m_soundDistance;
};

