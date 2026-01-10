/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundEngine.hpp"
#include "SoundDefs.hpp"
#include "client/app/Minecraft.hpp"
#include "common/Mth.hpp"

SoundEngine::SoundEngine(SoundSystem* soundSystem, float distance) : m_lastStreaming(Util::EMPTY_STRING), m_lastMusic(Util::EMPTY_STRING)
{
	m_pSoundSystem = soundSystem;
	m_pOptions = nullptr;
	field_40 = 0;
	field_A1C = 0;
	field_A20 = 0;
	m_muted = false;
	m_soundDistance = 1.0f / distance;
	m_bNoMusicDelay = m_random.nextInt(12000);
}

float SoundEngine::_getVolumeMult(const Vec3& pos)
{
	float distance = 1.1f - (pos.distanceTo(m_listenerPosition) * m_soundDistance);
	return Mth::clamp(distance, -1.0f, 1.0f);
}

void SoundEngine::init(Options* options, AppPlatform* plat)
{
	this->m_pOptions = options;
	this->m_pPlatform = plat;


	addSound("step.grass", "sound/step/grass1.ogg");
	addSound("step.grass", "sound/step/grass2.ogg");
	addSound("step.grass", "sound/step/grass3.ogg");
	addSound("step.grass", "sound/step/grass4.ogg");

	addSound("step.stone", "sound/step/stone1.ogg");
	addSound("step.stone", "sound/step/stone2.ogg");
	addSound("step.stone", "sound/step/stone3.ogg");
	addSound("step.stone", "sound/step/stone4.ogg");

	addSound("step.gravel", "sound/step/gravel1.ogg");
	addSound("step.gravel", "sound/step/gravel2.ogg");
	addSound("step.gravel", "sound/step/gravel3.ogg");
	addSound("step.gravel", "sound/step/gravel4.ogg");

	addSound("step.sand", "sound/step/sand1.ogg");
	addSound("step.sand", "sound/step/sand2.ogg");
	addSound("step.sand", "sound/step/sand3.ogg");
	addSound("step.sand", "sound/step/sand4.ogg");

	addSound("step.cloth", "sound/step/cloth1.ogg");
	addSound("step.cloth", "sound/step/cloth2.ogg");
	addSound("step.cloth", "sound/step/cloth3.ogg");
	addSound("step.cloth", "sound/step/cloth4.ogg");

	addSound("step.wood", "sound/step/wood1.ogg");
	addSound("step.wood", "sound/step/wood2.ogg");
	addSound("step.wood", "sound/step/wood3.ogg");
	addSound("step.wood", "sound/step/wood4.ogg");

	addSound("random.glass", "newsound/random/glass1.ogg");
	addSound("random.glass", "newsound/random/glass2.ogg");
	addSound("random.glass", "newsound/random/glass3.ogg");

	addSound("random.splash", "newsound/random/splash.ogg");
	addSound("random.explode", "newsound/random/explode.ogg");
	addSound("random.fuse", "newsound/random/fuse.ogg");
	addSound("random.click", "newsound/random/click.ogg");
	addSound("random.pop", "newsound/random/pop.ogg");
	addSound("random.door_close", "newsound/random/door_close.ogg");
	addSound("random.door_open", "newsound/random/door_open.ogg");
	addSound("random.drr", "newsound/random/drr.ogg");
	addSound("random.bow", "newsound/random/bow.ogg");
	addSound("random.bowhit", "newsound/random/bowhit1.ogg");
	addSound("random.bowhit", "newsound/random/bowhit2.ogg");
	addSound("random.bowhit", "newsound/random/bowhit3.ogg");
	addSound("random.bowhit", "newsound/random/bowhit4.ogg");
	addSound("random.hurt", "newsound/random/hurt.ogg");
	addSound("random.fizz", "newsound/random/fizz.ogg");

	addSound("damage.fallbig", "newsound/damage/fallbig1.ogg");
	addSound("damage.fallbig", "newsound/damage/fallbig2.ogg");
	addSound("damage.fallsmall", "newsound/damage/fallsmall.ogg");

	addSound("liquid.water", "newsound/liquid/water.ogg");

	addSound("ambient.weather.rain", "newsound/ambient/weather/rain1.ogg");
	addSound("ambient.weather.rain", "newsound/ambient/weather/rain2.ogg");
	addSound("ambient.weather.rain", "newsound/ambient/weather/rain3.ogg");
	addSound("ambient.weather.rain", "newsound/ambient/weather/rain4.ogg");
	addSound("ambient.weather.thunder", "newsound/ambient/weather/thunder1.ogg");
	addSound("ambient.weather.thunder", "newsound/ambient/weather/thunder2.ogg");
	addSound("ambient.weather.thunder", "newsound/ambient/weather/thunder3.ogg");
	addSound("ambient.cave.cave", "newsound/ambient/cave/cave1.ogg");
	addSound("ambient.cave.cave", "newsound/ambient/cave/cave2.ogg");
	addSound("ambient.cave.cave", "newsound/ambient/cave/cave3.ogg");
	addSound("ambient.cave.cave", "newsound/ambient/cave/cave4.ogg");
	addSound("ambient.cave.cave", "newsound/ambient/cave/cave5.ogg");
	addSound("ambient.cave.cave", "newsound/ambient/cave/cave6.ogg");
	addSound("ambient.cave.cave", "newsound/ambient/cave/cave7.ogg");
	addSound("ambient.cave.cave", "newsound/ambient/cave/cave8.ogg");
	addSound("ambient.cave.cave", "newsound/ambient/cave/cave9.ogg");
	addSound("ambient.cave.cave", "newsound/ambient/cave/cave10.ogg");
	addSound("ambient.cave.cave", "newsound/ambient/cave/cave11.ogg");
	addSound("ambient.cave.cave", "newsound/ambient/cave/cave12.ogg");
	addSound("ambient.cave.cave", "newsound/ambient/cave/cave13.ogg");

	addSound("mob.cow", "newsound/mob/cow1.ogg");
	addSound("mob.cow", "newsound/mob/cow2.ogg");
	addSound("mob.cow", "newsound/mob/cow3.ogg");
	addSound("mob.cow", "newsound/mob/cow4.ogg");
	addSound("mob.cowhurt", "newsound/mob/cowhurt1.ogg");
	addSound("mob.cowhurt", "newsound/mob/cowhurt2.ogg");
	addSound("mob.cowhurt", "newsound/mob/cowhurt3.ogg");

	addSound("mob.chicken", "newsound/mob/chicken1.ogg");
	addSound("mob.chicken", "newsound/mob/chicken2.ogg");
	addSound("mob.chicken", "newsound/mob/chicken3.ogg");
	addSound("mob.chickenhurt", "newsound/mob/chickenhurt1.ogg");
	addSound("mob.chickenhurt", "newsound/mob/chickenhurt2.ogg");
	addSound("mob.chickenplop", "newsound/mob/chickenplop.ogg");

	addSound("mob.sheep", "newsound/mob/sheep1.ogg");
	addSound("mob.sheep", "newsound/mob/sheep2.ogg");
	addSound("mob.sheep", "newsound/mob/sheep3.ogg");

	addSound("mob.pig", "newsound/mob/pig1.ogg");
	addSound("mob.pig", "newsound/mob/pig2.ogg");
	addSound("mob.pig", "newsound/mob/pig3.ogg");
	addSound("mob.pigdeath", "newsound/mob/pigdeath.ogg");

	addSound("mob.creeper", "newsound/mob/creeper1.ogg");
	addSound("mob.creeper", "newsound/mob/creeper2.ogg");
	addSound("mob.creeper", "newsound/mob/creeper3.ogg");
	addSound("mob.creeper", "newsound/mob/creeper4.ogg");
	addSound("mob.creeperdeath", "newsound/mob/creeperdeath.ogg");

	addSound("mob.slime", "newsound/mob/slime1.ogg");
	addSound("mob.slime", "newsound/mob/slime2.ogg");
	addSound("mob.slime", "newsound/mob/slime3.ogg");
	addSound("mob.slime", "newsound/mob/slime4.ogg");
	addSound("mob.slime", "newsound/mob/slime5.ogg");
	addSound("mob.slimeattack", "newsound/mob/slimeattack1.ogg");
	addSound("mob.slimeattack", "newsound/mob/slimeattack2.ogg");

	addSound("mob.ghast.moan", "newsound/mob/ghast/moan1.ogg");
	addSound("mob.ghast.moan", "newsound/mob/ghast/moan2.ogg");
	addSound("mob.ghast.moan", "newsound/mob/ghast/moan3.ogg");
	addSound("mob.ghast.moan", "newsound/mob/ghast/moan4.ogg");
	addSound("mob.ghast.moan", "newsound/mob/ghast/moan5.ogg");
	addSound("mob.ghast.moan", "newsound/mob/ghast/moan6.ogg");
	addSound("mob.ghast.moan", "newsound/mob/ghast/moan7.ogg");
	addSound("mob.ghast.scream", "newsound/mob/ghast/scream1.ogg");
	addSound("mob.ghast.scream", "newsound/mob/ghast/scream2.ogg");
	addSound("mob.ghast.scream", "newsound/mob/ghast/scream3.ogg");
	addSound("mob.ghast.scream", "newsound/mob/ghast/scream4.ogg");
	addSound("mob.ghast.scream", "newsound/mob/ghast/scream5.ogg");
	addSound("mob.ghast.death", "newsound/mob/ghast/death.ogg");
	addSound("mob.ghast.charge", "newsound/mob/ghast/charge.ogg");
	addSound("mob.ghast.fireball", "newsound/mob/ghast/fireball4.ogg");

	addSound("mob.wolf.growl", "newsound/mob/wolf/growl1.ogg");
	addSound("mob.wolf.growl", "newsound/mob/wolf/growl2.ogg");
	addSound("mob.wolf.growl", "newsound/mob/wolf/growl3.ogg");
	addSound("mob.wolf.howl", "newsound/mob/wolf/howl1.ogg");
	addSound("mob.wolf.howl", "newsound/mob/wolf/howl2.ogg");
	addSound("mob.wolf.whine", "newsound/mob/wolf/whine.ogg");
	addSound("mob.wolf.panting", "newsound/mob/wolf/panting.ogg");
	addSound("mob.wolf.bark", "newsound/mob/wolf/bark1.ogg");
	addSound("mob.wolf.bark", "newsound/mob/wolf/bark2.ogg");
	addSound("mob.wolf.bark", "newsound/mob/wolf/bark3.ogg");
	addSound("mob.wolf.shake", "newsound/mob/wolf/shake.ogg");
	addSound("mob.wolf.hurt", "newsound/mob/wolf/hurt1.ogg");
	addSound("mob.wolf.hurt", "newsound/mob/wolf/hurt2.ogg");
	addSound("mob.wolf.hurt", "newsound/mob/wolf/hurt3.ogg");
	addSound("mob.wolf.death", "newsound/mob/wolf/death.ogg");

	addSound("mob.zombie", "newsound/mob/zombie1.ogg");
	addSound("mob.zombie", "newsound/mob/zombie2.ogg");
	addSound("mob.zombie", "newsound/mob/zombie3.ogg");
	addSound("mob.zombiehurt", "newsound/mob/zombiehurt1.ogg");
	addSound("mob.zombiehurt", "newsound/mob/zombiehurt2.ogg");
	addSound("mob.zombiedeath", "newsound/mob/zombiedeath.ogg");

	addSound("mob.skeleton", "newsound/mob/skeleton1.ogg");
	addSound("mob.skeleton", "newsound/mob/skeleton2.ogg");
	addSound("mob.skeleton", "newsound/mob/skeleton3.ogg");
	addSound("mob.skeletonhurt", "newsound/mob/skeletonhurt1.ogg");
	addSound("mob.skeletonhurt", "newsound/mob/skeletonhurt2.ogg");
	addSound("mob.skeletonhurt", "newsound/mob/skeletonhurt3.ogg");
	addSound("mob.skeletonhurt", "newsound/mob/skeletonhurt4.ogg");
	addSound("mob.skeletondeath", "newsound/mob/skeletondeath.ogg");

	addSound("mob.spider", "newsound/mob/spider1.ogg");
	addSound("mob.spider", "newsound/mob/spider2.ogg");
	addSound("mob.spider", "newsound/mob/spider3.ogg");
	addSound("mob.spider", "newsound/mob/spider4.ogg");
	addSound("mob.spiderdeath", "newsound/mob/spiderdeath.ogg");

	addSound("mob.zombiepig.zpig", "newsound/mob/zombiepig/zpig1.ogg");
	addSound("mob.zombiepig.zpig", "newsound/mob/zombiepig/zpig2.ogg");
	addSound("mob.zombiepig.zpig", "newsound/mob/zombiepig/zpig3.ogg");
	addSound("mob.zombiepig.zpig", "newsound/mob/zombiepig/zpig4.ogg");
	addSound("mob.zombiepig.zpigangry", "newsound/mob/zombiepig/zpigangry1.ogg");
	addSound("mob.zombiepig.zpigangry", "newsound/mob/zombiepig/zpigangry2.ogg");
	addSound("mob.zombiepig.zpigangry", "newsound/mob/zombiepig/zpigangry3.ogg");
	addSound("mob.zombiepig.zpigangry", "newsound/mob/zombiepig/zpigangry4.ogg");
	addSound("mob.zombiepig.zpighurt", "newsound/mob/zombiepig/zpighurt1.ogg");
	addSound("mob.zombiepig.zpighurt", "newsound/mob/zombiepig/zpighurt2.ogg");
	addSound("mob.zombiepig.zpigdeath", "newsound/mob/zombiepig/zpigdeath.ogg");

	addSound("fire.fire", "newsound/fire/fire.ogg");
	addSound("fire.ignite", "newsound/fire/ignite.ogg");

	addSound("note.bd", "newsound/note/bd.ogg");
	addSound("note.snare", "newsound/note/snare.ogg");
	addSound("note.hat", "newsound/note/hat.ogg");
	addSound("note.harp", "newsound/note/harp.ogg");
	addSound("note.bassattack", "newsound/note/bassattack.ogg");

	addSound("portal.portal", "newsound/portal/portal.ogg");
	addSound("portal.travel", "newsound/portal/travel.ogg");
	addSound("portal.trigger", "newsound/portal/trigger.ogg");

	addSound("tile.piston.in", "newsound/tile/piston/in.ogg");
	addSound("tile.piston.out", "newsound/tile/piston/out.ogg");

	addStreamSound("13", "streaming/13.ogg");
	addStreamSound("cat", "streaming/cat.ogg");

	addMusic("calm1");
	addMusic("calm2");
	addMusic("calm3");
	addMusic("hal1");
	addMusic("hal2");
	addMusic("hal3");
	addMusic("hal4");
	addMusic("nuance1");
	addMusic("nuance2");
	addMusic("piano1");
	addMusic("piano2");
	addMusic("piano3");
}

void SoundEngine::addLocalSound(SoundRepository& repo, std::string name, std::string path, bool streaming)
{
	if (streaming && m_pSoundSystem->allowStreaming())
	{
		std::string asset = m_pPlatform->getAssetPath(path);
		std::string loadPath = asset;

#ifdef ANDROID
		//Copying streaming audios to the external directory, as the streaming with stb_vorbis doesn't have integration with SDL or Android Asset Manager

		loadPath = m_pOptions->m_pMinecraft->m_externalStorageDir + '/' + asset;
		std::vector<uint8_t> data = m_pPlatform->loadFile(path);

		createFoldersIfNotExists(loadPath.c_str());

		FILE* pFile = fopen(loadPath.c_str(), "wb");
		if (pFile)
		{
			fwrite(data.data(), 1, data.size(), pFile);
			fclose(pFile);
		}
#endif

		repo.add(name, SoundDesc(name, loadPath));
	}
	else
	{
		int channels, sample_rate;
		short* output;
		int samples = m_pPlatform->loadOgg(path, &channels, &sample_rate, &output);

		if (samples > 0)
		{
			const PCMSoundHeader& pcm = { channels, sizeof(short), sample_rate, samples };
			const SoundDesc& sound = SoundDesc(name, m_pPlatform->getAssetPath(path), pcm, output);
			repo.add(name, sound);
		}
	}
}

void SoundEngine::addSound(std::string name, std::string path, bool streaming)
{
	addLocalSound(m_sounds, name, path);
}

void SoundEngine::addStreamSound(std::string name, std::string path)
{
	addLocalSound(m_streamingSounds, name, path, true);

	SoundDesc sd;
	if (m_streamingSounds.get(name, sd))
		LOG_I("Adding %s streaming", sd.m_name.c_str());
}

void SoundEngine::addMusic(std::string name)
{
	addLocalSound(m_songs, name, "newmusic/" + name + ".ogg", true);
	addLocalSound(m_songs, name, "music/" + name + ".ogg", true);

	SoundDesc sd;
	if (m_songs.get(name, sd))
		LOG_I("Adding %s music", sd.m_name.c_str());
}

void SoundEngine::enable(bool b)
{
}

void SoundEngine::updateOptions()
{
}

void SoundEngine::mute()
{
	m_muted = true;
}

void SoundEngine::unMute()
{
	m_muted = false;
}

void SoundEngine::destroy()
{
}

void SoundEngine::play(const std::string& name, const Vec3& pos, float volume, float pitch)
{
	float vol = m_pOptions->m_fMasterVolume.get() * volume;
	if (vol <= 0.0f)
		return;

	float cVolume = Mth::clamp(_getVolumeMult(pos) * vol, 0.0f, 1.0f);
	SoundDesc sd;

	if (m_sounds.get(name, sd))
		playDesc(sd, pos, volume, pitch);
}

void SoundEngine::playDesc(const SoundDesc& desc, const Vec3& pos, float volume, float pitch, bool isUI)
{
	if (desc.m_bIsStream)
	{
		m_pSoundSystem->playMusic(desc.m_path);
		m_pSoundSystem->setMusicVolume(volume);
	}
	else
	{
		m_pSoundSystem->playAt(desc, pos, volume, pitch, desc.m_bIsStream);
	}
}

void SoundEngine::playUI(const std::string& name, float volume, float pitch)
{
	float vol = m_pOptions->m_fMasterVolume.get() * volume * 0.25f;
	if (vol <= 0.0f)
		return;

	SoundDesc sd;

	if (m_sounds.get(name, sd))
	{
		float cVolume = Mth::clamp(vol, 0.0f, 1.0f);
		float cPitch = Mth::clamp(pitch, -1.0f, 1.0f);
		m_pSoundSystem->playAt(sd, Vec3::ZERO, cVolume, cPitch, true);
	}
}

void SoundEngine::playStreaming(const std::string& name, const Vec3& pos, float volume, float pitch)
{
	if (!m_lastStreaming.empty())
	{
		if (m_pSoundSystem->isPlayingMusic())
			m_pSoundSystem->stopMusic();
		else
			m_pSoundSystem->stop(m_lastStreaming);
		m_lastStreaming = Util::EMPTY_STRING;
	}

	SoundDesc sd;
	if (!name.empty() && m_streamingSounds.get(name, sd))
	{
		playDesc(sd, pos, volume, pitch);
		m_lastStreaming = name;
	}
}

void SoundEngine::playMusicTick()
{
	if (m_songs.m_all.empty()) return;

	if (m_pOptions->m_fMusicVolume.get() > 0.0f)
	{
		if ((m_lastStreaming.empty() || !m_pSoundSystem->playing(m_lastStreaming)) && !m_pSoundSystem->isPlayingMusic()) {
			if (m_bNoMusicDelay > 0)
			{
				--m_bNoMusicDelay;
				return;
			}

			const SoundDesc& song = m_songs.any();
			LOG_I("Playing %s music", song.m_name.c_str());
			m_bNoMusicDelay = m_random.nextInt(12000) + 12000;
			m_lastStreaming = m_lastMusic = song.m_name;
			playDesc(song, Vec3::ZERO, m_pOptions->m_fMusicVolume.get(), 1.0f, true);
		}
	}
}

void SoundEngine::update(const Mob* mob, float partialTicks)
{
	if (m_pOptions->m_fMasterVolume.get() > 0.0f)
	{
		if (mob)
		{
			Vec3 pos = mob->getPos(partialTicks);
			m_listenerPosition = pos;
			m_pSoundSystem->setListenerPos(pos);

			Vec2 rot = mob->getRot(partialTicks);
			m_listenerOrientation = rot;
			m_pSoundSystem->setListenerAngle(rot);
		}
	}
}
