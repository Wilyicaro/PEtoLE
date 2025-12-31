#include "CustomSoundSystem.hpp"

#include "common/Utils.hpp"

SoundSystemAL::SoundSystemAL()
{
	_initialized = false;
    _listenerVolume = 1.0f;
	_musicStream = nullptr;
	_lastListenerPos = Vec3::ZERO;
	_listenerYaw = 0.0f;
    
    startEngine();
}

SoundSystemAL::~SoundSystemAL()
{
    stopEngine();
}

// Delete Sources
void SoundSystemAL::delete_sources()
{
	if (_initialized)
	{
		for (std::vector<ALuint>::iterator source = m_sources_idle.begin(); source != m_sources_idle.end(); source++)
		{
			alDeleteSources(1, &*source);
			AL_ERROR_CHECK();
		}
		for (auto source = m_sources.begin(); source != m_sources.end(); source++)
		{
			alDeleteSources(1, &source->m_id);
			AL_ERROR_CHECK();
		}
	}
	m_sources_idle.clear();
	m_sources.clear();
}

// Clear Finished Sources
void SoundSystemAL::_cleanSources()
{
	std::vector<SoundSource>::iterator it = m_sources.begin();
	while (it != m_sources.end())
	{
		ALuint source = it->m_id;
		bool remove = false;
		// Check
		if (source && alIsSource(source))
		{
			// Is Valid Source
			ALint source_state;
			alGetSourcei(source, AL_SOURCE_STATE, &source_state);
			AL_ERROR_CHECK();
			if (source_state != AL_PLAYING)
			{
				// Finished Playing
				remove = true;
				m_sources_idle.push_back(source);

				// Reset playback state of source to prevent buffer ghosting on legacy Mac OS X and Windows.
				// see: https://stackoverflow.com/questions/6960731/openal-problem-changing-gain-of-source
				alSourceStop(source);
				AL_ERROR_CHECK();
				alSourceRewind(source);
				AL_ERROR_CHECK();
			}
		}
		else
		{
			// Not A Source
			remove = true;
		}
		// Remove If Needed
		if (remove)
		{
			it = m_sources.erase(it);
		}
		else
		{
			++it;
		}
	}
}

// Delete Buffers
void SoundSystemAL::delete_buffers()
{
	if (_initialized)
	{
		for (auto it = m_buffers.begin(); it != m_buffers.end(); it++)
		{
			if (it->second && alIsBuffer(it->second))
			{
				alDeleteBuffers(1, &it->second);
				AL_ERROR_CHECK();
			}
		}
	}
	m_buffers.clear();
}


ALuint SoundSystemAL::_getIdleSource()
{
	ALuint al_source = AL_NONE;

	if (m_sources_idle.size() > 0)
	{
		// Use Idle Source
		al_source = m_sources_idle.back();
		m_sources_idle.pop_back();
	}

	return al_source;
}

bool SoundSystemAL::_hasMaxSources() const
{
	return m_sources.size() + m_sources_idle.size() >= MAX_IDLE_SOURCES;
}

ALuint SoundSystemAL::_getSource(bool& isNew, bool tryClean)
{
	isNew = false;
	ALuint al_source = _getIdleSource(); // Try to fetch pre-existing idle source
	if (!al_source)
	{
		// Could not find pre-existing idle source

		if (_hasMaxSources())
		{
			if (tryClean)
			{
				// Clean finished sources since no idle ones are available
				_cleanSources();

				// Did some cleaning, lets try again...
				return _getSource(isNew, false);
			}
			else
			{
				// Too many sources already exist and they're all in-use, sucks to suck...
				return AL_NONE;
			}
		}

		// Create Source
		alGenSources(1, &al_source);
		// Special Out-Of-Memory Handling
		{
			ALenum err = alGetError();
			if (err == AL_OUT_OF_MEMORY)
			{
				return AL_NONE;
			}
			else
			{
				AL_ERROR_CHECK_MANUAL(err);
			}
		}
		isNew = true;
	}

	return al_source;
}

// Get Buffer
ALuint SoundSystemAL::get_buffer(const SoundDesc& sound)
{
	auto it = m_buffers.find(sound.m_pData);
	if (it != m_buffers.end())
	{
		return it->second;
	}
	else
	{
		// Sound Format
		ALenum format = AL_NONE;
		if (sound.m_header.m_channels == 1)
		{
			format = sound.m_header.m_bytes_per_sample == 2 ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8;
		}
		else if (sound.m_header.m_channels == 2)
		{
			format = sound.m_header.m_bytes_per_sample == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8;
		}

		// Sound Data Size
		int size = sound.m_header.m_channels * sound.m_header.m_samples * sound.m_header.m_bytes_per_sample;

		// Create Buffer
		ALuint buffer;
		alGenBuffers(1, &buffer);
		AL_ERROR_CHECK();
		alBufferData(buffer, format, sound.m_pData, size, sound.m_header.m_sample_rate);
		AL_ERROR_CHECK();

		// Store
		m_buffers[sound.m_pData] = buffer;
		return buffer;
	}
}

bool SoundSystemAL::isAvailable()
{
	return _initialized;
}

void SoundSystemAL::update(float)
{
	_musicStream->update();
}

void SoundSystemAL::setListenerPos(const Vec3& pos)
{
	if (_lastListenerPos == pos)
		return;

	// Update Listener Position
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
	AL_ERROR_CHECK();
	_lastListenerPos = pos;

	// Update Listener Volume
	alListenerf(AL_GAIN, _listenerVolume);
	AL_ERROR_CHECK();
}

void SoundSystemAL::setListenerAngle(const Vec2& rot)
{
	if (_listenerYaw == rot.y)
		return; // No need to waste time doing math and talking to OpenAL
	// Update Listener Orientation
	float radian_yaw = rot.y * (M_PI / 180);
	ALfloat orientation[] = { -sinf(radian_yaw), 0.0f, cosf(radian_yaw), 0.0f, 1.0f, 0.0f };
	alListenerfv(AL_ORIENTATION, orientation);
	AL_ERROR_CHECK();
	_listenerYaw = rot.y;
}

void SoundSystemAL::setMusicVolume(float vol)
{
	assert(_musicStream != nullptr);

	_musicStream->setVolume(vol);
}

void SoundSystemAL::stop(const std::string& sound)
{
	for (auto& it : m_sources)
	{
		if (it.m_name == sound)
		{
			alSourceStop(it.m_id);
			AL_ERROR_CHECK();
		}
	}
}

bool SoundSystemAL::playing(const std::string& sound)
{
	for (auto& it : m_sources)
	{
		if (it.m_name == sound)
		{
			ALint source_state;
			alGetSourcei(it.m_id, AL_SOURCE_STATE, &source_state);
			AL_ERROR_CHECK();
			if (source_state == AL_PLAYING)
				return true;
		}
	}
	return false;
}

void SoundSystemAL::playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch, bool isUI)
{
	// Check
	if (!_initialized)
	{
		return;
	}

	if (volume <= 0.0f)
		return;

	float distance = 0.0f;
	if (pos.x != 0 || pos.y != 0 || pos.z != 0)
	{
		distance = pos.distanceTo(_lastListenerPos);
		if (distance >= MAX_DISTANCE)
			return;
	}

	// Load Sound
	ALuint buffer = get_buffer(sound);
	if (!buffer)
		return;
	
	// Get Source
	bool isNew;
	ALuint al_source = _getSource(isNew);
	if (!al_source)
	{
		// Couldn't get a source, just give up.
		return;
	}

	// Set Properties
	alSourcef(al_source, AL_PITCH, pitch);
	AL_ERROR_CHECK();
	alSourcef(al_source, AL_GAIN, volume);
	AL_ERROR_CHECK();
	alSource3f(al_source, AL_POSITION, pos.x, pos.y, pos.z);
	AL_ERROR_CHECK();
	alSourcei(al_source, AL_SOURCE_RELATIVE, isUI);
	AL_ERROR_CHECK();

	// Only set constant parameters if source isn't reused
	if (isNew)
	{
		// Set Attenuation
		alSourcef(al_source, AL_MAX_DISTANCE, MAX_DISTANCE);
		AL_ERROR_CHECK();
		alSourcef(al_source, AL_ROLLOFF_FACTOR, 0.9f); // 0.9f is audibly on-par with b1.2_02's rolloff factor. So you probably shouldn't change it. 0.03f is default value for Paulscode.
		AL_ERROR_CHECK();
		alSourcef(al_source, AL_REFERENCE_DISTANCE, 5.0f); // Sounds the same regardless of being set. Paulscode doesn't set this.
		AL_ERROR_CHECK();

		alSource3f(al_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
		AL_ERROR_CHECK();
		alSourcei(al_source, AL_LOOPING, AL_FALSE);
		AL_ERROR_CHECK();
	}
	else
	{
		// Detach all of the buffers from the source
		alSourcei(al_source, AL_BUFFER, AL_NONE);
		AL_ERROR_CHECK();
	}

	// Set Buffer
	alSourcei(al_source, AL_BUFFER, buffer);
	AL_ERROR_CHECK();

	// Play
	alSourcePlay(al_source);
	AL_ERROR_CHECK();
	m_sources.push_back(SoundSource(al_source, sound.m_name));
}

bool SoundSystemAL::allowStreaming()
{
	return true;
}

void SoundSystemAL::playMusic(const std::string& soundPath)
{
	_musicStream->open(soundPath);
}

bool SoundSystemAL::isPlayingMusic() const
{
	return _musicStream->isPlaying();
}

void SoundSystemAL::stopMusic()
{
	_musicStream->close();
}

void SoundSystemAL::pauseMusic(bool state)
{
	_musicStream->setPausedState(state);
}

void SoundSystemAL::startEngine()
{
    if (_initialized) return;
    
    _device = alcOpenDevice(NULL);
	if (!_device)
	{
		LOG_E("Unable To Load Audio Engine");
		return;
	}
    
	// Create Context
	_context = alcCreateContext(_device, NULL);
	ALCenum err = alcGetError(_device);
	if (err != ALC_NO_ERROR)
	{
		LOG_E("Unable To Open Audio Context: %s", alcGetString(_device, err));
		return;
	}
    
	// Select Context
	alcMakeContextCurrent(_context);
	err = alcGetError(_device);
	if (err != ALC_NO_ERROR)
	{
		LOG_E("Unable To Select Audio Context: %s", alcGetString(_device, err));
		return;
	}
    
	// Set Distance Model
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

	_musicStream = new SoundStreamAL();
    
	// Mark As loaded
	_initialized = true;
}

void SoundSystemAL::stopEngine()
{
    if (!_initialized) return;

	delete _musicStream;
    
	// Delete Audio Sources
	delete_sources();
    
	// Delete Audio Buffers
	delete_buffers();
    
	// Deselect Context
	alcMakeContextCurrent(NULL);
	ALCenum err = alcGetError(_device);
	if (err != ALC_NO_ERROR)
	{
		LOG_E("Unable To Deselect Audio Context: %s", alcGetString(_device, err));
	}
    
	// Destroy Context
	alcDestroyContext(_context);
	err = alcGetError(_device);
	if (err != ALC_NO_ERROR)
	{
		LOG_E("Unable To Destroy Audio Context: %s", alcGetString(_device, err));
	}
    
	// Close Device
	alcCloseDevice(_device);
	// Can't check for error because _device is closed
	/*err = alcGetError(_device);
     if (err != ALC_NO_ERROR)
     {
     LOG_E("Unable To Close Audio Device: %s", alcGetString(_device, err));
     }*/
    
    // Mark as unloaded
    _initialized = false;
}
