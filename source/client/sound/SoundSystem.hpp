/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <string>
#include "SoundData.hpp"
#include <world/phys/Vec3T.hpp>
#include <world/phys/Vec2.hpp>
#include "SoundRepository.hpp"

class SoundSystem
{
public:
	virtual ~SoundSystem();

	virtual bool isAvailable();

	virtual void setListenerPos(const Vec3& pos);
	virtual void setListenerAngle(const Vec2& rot);
	virtual void update(float);
	virtual void setMusicVolume(float vol);
	virtual void load(const std::string& sound);
	virtual void play(const std::string& sound);
	virtual void pause(const std::string& sound);
	virtual void stop(const std::string& sound);
	virtual bool playing(const std::string& sound);
	virtual void playAt(const SoundDesc& sound, const Vec3& pos, float a, float b, bool isUI = false);

	virtual bool allowStreaming();

	virtual void playMusic(const std::string& soundPath);
	virtual bool isPlayingMusic() const;
	virtual void stopMusic();
	virtual void pauseMusic(bool state);
    
    // Be prepared for these to be called regardless of engine state
    virtual void startEngine();
    virtual void stopEngine();
    
    virtual void muteAudio();
    virtual void unMuteAudio();
};

