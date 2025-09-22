/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once
#include <string>
#include <Windows.h>
#include <uuids.h>
#include <strmif.h>
#include <stdexcept>
#include <vector>

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <mmsystem.h>
#include <dsound.h>

#include "client/sound/SoundSystem.hpp"
#include "client/sound/SoundData.hpp"
#include "SoundStreamDS.hpp"

class SoundSystemDS : public SoundSystem
{
public:
	SoundSystemDS();
	~SoundSystemDS();
	virtual bool isAvailable();
	virtual void setListenerPos(const Vec3& pos) override;
	virtual void setListenerAngle(const Vec2& rot) override;
	virtual void load(const std::string& sound) override;
	virtual void play(const std::string& sound) override;
	virtual void pause(const std::string& sound) override;
	virtual void stop(const std::string& sound) override;
	virtual bool playing(const std::string& sound) override;
	virtual void playAt(const SoundDesc& sound, const Vec3& pos, float a, float b, bool is2D) override;
	virtual void update(float) override;
	virtual bool allowStreaming() override;
	virtual void playMusic(const std::string& soundPath) override;
	virtual bool isPlayingMusic() const override;
	virtual void stopMusic() override;
	virtual void pauseMusic(bool state) override;
private:

	struct BufferInfo
	{
		std::string name;
		LPDIRECTSOUNDBUFFER buffer;
		LPDIRECTSOUND3DBUFFER object3d;
	};

	bool m_available;
	IDirectSound* m_directsound;
	LPDIRECTSOUND3DLISTENER m_listener;
	std::vector<BufferInfo> m_buffers;
	SoundStreamDS* _musicStream;
};