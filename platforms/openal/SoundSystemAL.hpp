#pragma once

#ifdef USE_OPENAL

#ifdef _WIN32
#include <AL/al.h>
#include <AL/alc.h>
#pragma comment( lib, "OpenAL32.lib" )
#elif defined(__APPLE__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <string>
#include <vector>
#include <unordered_map>

#include "client/sound/SoundSystem.hpp"
#include "world/phys/Vec3T.hpp"

#define MAX_IDLE_SOURCES 50
#define MAX_DISTANCE 16.0f

class SoundSystemAL : public SoundSystem
{
public:
	SoundSystemAL();
	~SoundSystemAL();
	virtual bool isAvailable() override;
	void update();
	virtual void stop(const std::string& sound) override;
	virtual bool playing(const std::string& sound) override;
	virtual void playAt(const SoundDesc& sound, float x, float y, float z, float volume, float pitch) override;

	virtual void setListenerPos(float x, float y, float z);
	virtual void setListenerAngle(float yaw, float pitch);
    
    virtual void startEngine();
    virtual void stopEngine();
    
private:
	struct SoundSource {
		ALuint m_id;
		std::string m_name;
		SoundSource(ALuint id, std::string name) : m_id(id), m_name(name)
		{
		}
	};

	void delete_sources();
	void delete_buffers();
	ALuint get_buffer(const SoundDesc& sound);

	ALCdevice *_device;
	ALCcontext *_context;
	bool _initialized;
	std::vector<SoundSource> m_sources;
	std::vector<ALuint> m_sources_idle;
	std::unordered_map<void*, ALuint> m_buffers;

	Vec3 _lastListenerPos;
    float _listenerVolume;
};

#endif
