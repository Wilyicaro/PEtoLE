#pragma once

#ifdef USE_OPENAL

#include "thirdparty/OpenAL.h"

#include <string>
#include <vector>
#include <unordered_map>

#include "client/sound/SoundSystem.hpp"
#include "world/phys/Vec3T.hpp"
#include "SoundStreamAL.hpp"

#define MAX_IDLE_SOURCES 50
#define MAX_DISTANCE 16.0f

class SoundSystemAL : public SoundSystem
{
public:
	SoundSystemAL();
	~SoundSystemAL();
	virtual bool isAvailable() override;
	virtual void update(float) override;
	virtual void stop(const std::string& sound) override;
	virtual bool playing(const std::string& sound) override;
	virtual void playAt(const SoundDesc& sound, const Vec3& pos, float volume, float pitch, bool isUI) override;

	virtual bool allowStreaming() override;

	virtual void playMusic(const std::string& soundPath) override;
	virtual bool isPlayingMusic() const override;
	virtual void stopMusic() override;
	virtual void pauseMusic(bool state) override;

	virtual void setListenerPos(const Vec3& pos) override;
	virtual void setListenerAngle(const Vec2& rot) override;
	virtual void setMusicVolume(float vol) override;

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
	SoundStreamAL* _musicStream;

	Vec3 _lastListenerPos;
    float _listenerVolume;
	float _listenerYaw;
};

#endif
