#pragma once

#include "client/sound/SoundSystem.hpp"

#define SOUND_SYSTEM NoSoundSystem

class NoSoundSystem : public SoundSystem
{
public:
	bool isAvailable() override;
};