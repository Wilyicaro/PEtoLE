/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundSystem.hpp"
#include "SoundEngine.hpp"

SoundSystem::~SoundSystem()
{
}

bool SoundSystem::isAvailable()
{
	return false;
}

void SoundSystem::setListenerPos(const Vec3& pos)
{
}

void SoundSystem::setListenerAngle(const Vec2& rot)
{
}

void SoundSystem::update(float)
{
}

void SoundSystem::setMusicVolume(float vol)
{
}

void SoundSystem::load(const std::string& sound)
{
}

void SoundSystem::play(const std::string& sound)
{
}

void SoundSystem::pause(const std::string& sound)
{
}

void SoundSystem::stop(const std::string& sound)
{
}

bool SoundSystem::playing(const std::string& sound)
{
	return false;
}

void SoundSystem::playAt(const SoundDesc& sound, const Vec3& pos, float a, float b, bool is2D)
{
}

bool SoundSystem::allowStreaming()
{
	return false;
}


void SoundSystem::playMusic(const std::string& soundPath)
{
}

bool SoundSystem::isPlayingMusic() const
{
	return false;
}

void SoundSystem::stopMusic()
{
}

void SoundSystem::pauseMusic(bool state)
{
}

void SoundSystem::startEngine()
{
}

void SoundSystem::stopEngine()
{
}

void SoundSystem::muteAudio()
{
}

void SoundSystem::unMuteAudio()
{
}