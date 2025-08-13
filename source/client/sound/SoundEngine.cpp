/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundEngine.hpp"
#include "SoundDefs.hpp"
#include "common/Mth.hpp"

SoundEngine::SoundEngine(SoundSystem* soundSystem) : m_lastStreaming(Util::EMPTY_STRING), m_lastMusic(Util::EMPTY_STRING)
{
	m_pSoundSystem = soundSystem;
	m_pOptions = nullptr;
	field_40 = 0;
	field_A1C = 0;
	field_A20 = 0;
	m_muted = false;
	m_bNoMusicDelay = m_random.nextInt(12000);
}

float SoundEngine::_getVolumeMult(const Vec3& pos)
{
	return 1.0f;
}

void SoundEngine::init(Options* options, AppPlatform* plat)
{
	this->m_pOptions = options;
	this->m_pPlatform = plat;


	addLocalSound("step.grass", "newsound/step/grass1.ogg");
	addLocalSound("step.grass", "newsound/step/grass2.ogg");
	addLocalSound("step.grass", "newsound/step/grass3.ogg");
	addLocalSound("step.grass", "newsound/step/grass4.ogg");

	addLocalSound("step.stone", "newsound/step/stone1.ogg");
	addLocalSound("step.stone", "newsound/step/stone2.ogg");
	addLocalSound("step.stone", "newsound/step/stone3.ogg");
	addLocalSound("step.stone", "newsound/step/stone4.ogg");

	addLocalSound("step.gravel", "newsound/step/gravel1.ogg");
	addLocalSound("step.gravel", "newsound/step/gravel2.ogg");
	addLocalSound("step.gravel", "newsound/step/gravel3.ogg");
	addLocalSound("step.gravel", "newsound/step/gravel4.ogg");

	addLocalSound("step.sand", "newsound/step/sand1.ogg");
	addLocalSound("step.sand", "newsound/step/sand2.ogg");
	addLocalSound("step.sand", "newsound/step/sand3.ogg");
	addLocalSound("step.sand", "newsound/step/sand4.ogg");

	addLocalSound("step.cloth", "newsound/step/cloth1.ogg");
	addLocalSound("step.cloth", "newsound/step/cloth2.ogg");
	addLocalSound("step.cloth", "newsound/step/cloth3.ogg");
	addLocalSound("step.cloth", "newsound/step/cloth4.ogg");

	addLocalSound("step.wood", "newsound/step/wood1.ogg");
	addLocalSound("step.wood", "newsound/step/wood2.ogg");
	addLocalSound("step.wood", "newsound/step/wood3.ogg");
	addLocalSound("step.wood", "newsound/step/wood4.ogg");

	addLocalSound("random.glass", "newsound/random/glass1.ogg");
	addLocalSound("random.glass", "newsound/random/glass2.ogg");
	addLocalSound("random.glass", "newsound/random/glass3.ogg");

	addLocalSound("random.splash", "newsound/random/splash.ogg");
	addLocalSound("random.explode", "newsound/random/explode.ogg");
	addLocalSound("random.fuse", "newsound/random/fuse.ogg");
	addLocalSound("random.click", "newsound/random/click.ogg");
	addLocalSound("random.pop", "newsound/random/pop.ogg");
	addLocalSound("random.door_close", "newsound/random/door_close.ogg");
	addLocalSound("random.door_open", "newsound/random/door_open.ogg");
	addLocalSound("random.drr", "newsound/random/drr.ogg");
	addLocalSound("random.bow", "newsound/random/bow.ogg");
	addLocalSound("random.bowhit", "newsound/random/bowhit1.ogg");
	addLocalSound("random.bowhit", "newsound/random/bowhit2.ogg");
	addLocalSound("random.bowhit", "newsound/random/bowhit3.ogg");
	addLocalSound("random.bowhit", "newsound/random/bowhit4.ogg");
	addLocalSound("random.hurt", "newsound/random/hurt.ogg");
	addLocalSound("random.fizz", "newsound/random/fizz.ogg");

	addLocalSound("liquid.water", "newsound/liquid/water.ogg");

	addLocalSound("ambient.weather.rain", "newsound/ambient/weather/rain1.ogg");
	addLocalSound("ambient.weather.rain", "newsound/ambient/weather/rain2.ogg");
	addLocalSound("ambient.weather.rain", "newsound/ambient/weather/rain3.ogg");
	addLocalSound("ambient.weather.rain", "newsound/ambient/weather/rain4.ogg");
	addLocalSound("ambient.weather.thunder", "newsound/ambient/weather/thunder1.ogg");
	addLocalSound("ambient.weather.thunder", "newsound/ambient/weather/thunder2.ogg");
	addLocalSound("ambient.weather.thunder", "newsound/ambient/weather/thunder3.ogg");
	addLocalSound("ambient.cave.cave", "newsound/ambient/cave/cave1.ogg");
	addLocalSound("ambient.cave.cave", "newsound/ambient/cave/cave2.ogg");
	addLocalSound("ambient.cave.cave", "newsound/ambient/cave/cave3.ogg");
	addLocalSound("ambient.cave.cave", "newsound/ambient/cave/cave4.ogg");
	addLocalSound("ambient.cave.cave", "newsound/ambient/cave/cave5.ogg");
	addLocalSound("ambient.cave.cave", "newsound/ambient/cave/cave6.ogg");
	addLocalSound("ambient.cave.cave", "newsound/ambient/cave/cave7.ogg");
	addLocalSound("ambient.cave.cave", "newsound/ambient/cave/cave8.ogg");
	addLocalSound("ambient.cave.cave", "newsound/ambient/cave/cave9.ogg");
	addLocalSound("ambient.cave.cave", "newsound/ambient/cave/cave10.ogg");
	addLocalSound("ambient.cave.cave", "newsound/ambient/cave/cave11.ogg");
	addLocalSound("ambient.cave.cave", "newsound/ambient/cave/cave12.ogg");
	addLocalSound("ambient.cave.cave", "newsound/ambient/cave/cave13.ogg");

	addLocalSound("mob.cow", "newsound/mob/cow1.ogg");
	addLocalSound("mob.cow", "newsound/mob/cow2.ogg");
	addLocalSound("mob.cow", "newsound/mob/cow3.ogg");
	addLocalSound("mob.cow", "newsound/mob/cow4.ogg");
	addLocalSound("mob.cowhurt", "newsound/mob/cowhurt1.ogg");
	addLocalSound("mob.cowhurt", "newsound/mob/cowhurt2.ogg");
	addLocalSound("mob.cowhurt", "newsound/mob/cowhurt3.ogg");
	addLocalSound("mob.cowhurt", "newsound/mob/cowhurt4.ogg");

	addLocalSound("mob.chicken", "newsound/mob/chicken1.ogg");
	addLocalSound("mob.chicken", "newsound/mob/chicken2.ogg");
	addLocalSound("mob.chicken", "newsound/mob/chicken3.ogg");
	addLocalSound("mob.chickenhurt", "newsound/mob/chickenhurt1.ogg");
	addLocalSound("mob.chickenhurt", "newsound/mob/chickenhurt2.ogg");
	addLocalSound("mob.chickenplop", "newsound/mob/chickenplop.ogg");

	addLocalSound("mob.sheep", "newsound/mob/sheep1.ogg");
	addLocalSound("mob.sheep", "newsound/mob/sheep2.ogg");
	addLocalSound("mob.sheep", "newsound/mob/sheep3.ogg");

	addLocalSound("mob.pig", "newsound/mob/pig1.ogg");
	addLocalSound("mob.pig", "newsound/mob/pig2.ogg");
	addLocalSound("mob.pig", "newsound/mob/pig3.ogg");
	addLocalSound("mob.pigdeath", "newsound/mob/pigdeath.ogg");

	addLocalSound("mob.creeper", "newsound/mob/creeper1.ogg");
	addLocalSound("mob.creeper", "newsound/mob/creeper2.ogg");
	addLocalSound("mob.creeper", "newsound/mob/creeper3.ogg");
	addLocalSound("mob.creeper", "newsound/mob/creeper4.ogg");
	addLocalSound("mob.creeperdeath", "newsound/mob/creeperdeath.ogg");

	addLocalSound("mob.slime", "newsound/mob/slime1.ogg");
	addLocalSound("mob.slime", "newsound/mob/slime2.ogg");
	addLocalSound("mob.slime", "newsound/mob/slime3.ogg");
	addLocalSound("mob.slime", "newsound/mob/slime4.ogg");
	addLocalSound("mob.slime", "newsound/mob/slime5.ogg");
	addLocalSound("mob.slimeattack", "newsound/mob/slimeattack1.ogg");
	addLocalSound("mob.slimeattack", "newsound/mob/slimeattack2.ogg");

	addLocalSound("mob.ghast.moan", "newsound/mob/ghast/moan1.ogg");
	addLocalSound("mob.ghast.moan", "newsound/mob/ghast/moan2.ogg");
	addLocalSound("mob.ghast.moan", "newsound/mob/ghast/moan3.ogg");
	addLocalSound("mob.ghast.moan", "newsound/mob/ghast/moan4.ogg");
	addLocalSound("mob.ghast.moan", "newsound/mob/ghast/moan5.ogg");
	addLocalSound("mob.ghast.moan", "newsound/mob/ghast/moan6.ogg");
	addLocalSound("mob.ghast.moan", "newsound/mob/ghast/moan7.ogg");
	addLocalSound("mob.ghast.scream", "newsound/mob/ghast/scream1.ogg");
	addLocalSound("mob.ghast.scream", "newsound/mob/ghast/scream2.ogg");
	addLocalSound("mob.ghast.scream", "newsound/mob/ghast/scream3.ogg");
	addLocalSound("mob.ghast.scream", "newsound/mob/ghast/scream4.ogg");
	addLocalSound("mob.ghast.scream", "newsound/mob/ghast/scream5.ogg");
	addLocalSound("mob.ghast.death", "newsound/mob/ghast/death.ogg");
	addLocalSound("mob.ghast.charge", "newsound/mob/ghast/charge.ogg");
	addLocalSound("mob.ghast.fireball", "newsound/mob/ghast/fireball4.ogg");

	addLocalSound("mob.wolf.growl", "newsound/mob/wolf/growl1.ogg");
	addLocalSound("mob.wolf.growl", "newsound/mob/wolf/growl2.ogg");
	addLocalSound("mob.wolf.growl", "newsound/mob/wolf/growl3.ogg");
	addLocalSound("mob.wolf.howl", "newsound/mob/wolf/howl1.ogg");
	addLocalSound("mob.wolf.howl", "newsound/mob/wolf/howl2.ogg");
	addLocalSound("mob.wolf.whine", "newsound/mob/wolf/whine.ogg");
	addLocalSound("mob.wolf.panting", "newsound/mob/wolf/panting.ogg");
	addLocalSound("mob.wolf.bark", "newsound/mob/wolf/bark1.ogg");
	addLocalSound("mob.wolf.bark", "newsound/mob/wolf/bark2.ogg");
	addLocalSound("mob.wolf.bark", "newsound/mob/wolf/bark3.ogg");
	addLocalSound("mob.wolf.shake", "newsound/mob/wolf/shake.ogg");
	addLocalSound("mob.wolf.hurt", "newsound/mob/wolf/hurt1.ogg");
	addLocalSound("mob.wolf.hurt", "newsound/mob/wolf/hurt2.ogg");
	addLocalSound("mob.wolf.hurt", "newsound/mob/wolf/hurt3.ogg");
	addLocalSound("mob.wolf.death", "newsound/mob/wolf/death.ogg");

	addLocalSound("mob.zombie", "newsound/mob/zombie1.ogg");
	addLocalSound("mob.zombie", "newsound/mob/zombie2.ogg");
	addLocalSound("mob.zombie", "newsound/mob/zombie3.ogg");
	addLocalSound("mob.zombiehurt", "newsound/mob/zombiehurt1.ogg");
	addLocalSound("mob.zombiehurt", "newsound/mob/zombiehurt2.ogg");
	addLocalSound("mob.zombiedeath", "newsound/mob/zombiedeath.ogg");

	addLocalSound("mob.skeleton", "newsound/mob/skeleton1.ogg");
	addLocalSound("mob.skeleton", "newsound/mob/skeleton2.ogg");
	addLocalSound("mob.skeleton", "newsound/mob/skeleton3.ogg");
	addLocalSound("mob.skeletonhurt", "newsound/mob/skeletonhurt1.ogg");
	addLocalSound("mob.skeletonhurt", "newsound/mob/skeletonhurt2.ogg");
	addLocalSound("mob.skeletonhurt", "newsound/mob/skeletonhurt3.ogg");
	addLocalSound("mob.skeletonhurt", "newsound/mob/skeletonhurt4.ogg");
	addLocalSound("mob.skeletondeath", "newsound/mob/skeletondeath.ogg");

	addLocalSound("mob.spider", "newsound/mob/spider1.ogg");
	addLocalSound("mob.spider", "newsound/mob/spider2.ogg");
	addLocalSound("mob.spider", "newsound/mob/spider3.ogg");
	addLocalSound("mob.spider", "newsound/mob/spider4.ogg");
	addLocalSound("mob.spiderdeath", "newsound/mob/spiderdeath.ogg");

	addLocalSound("mob.zombiepig.zpig", "newsound/mob/zombiepig/zpig1.ogg");
	addLocalSound("mob.zombiepig.zpig", "newsound/mob/zombiepig/zpig2.ogg");
	addLocalSound("mob.zombiepig.zpig", "newsound/mob/zombiepig/zpig3.ogg");
	addLocalSound("mob.zombiepig.zpig", "newsound/mob/zombiepig/zpig4.ogg");
	addLocalSound("mob.zombiepig.zpigangry", "newsound/mob/zombiepig/zpigangry1.ogg");
	addLocalSound("mob.zombiepig.zpigangry", "newsound/mob/zombiepig/zpigangry2.ogg");
	addLocalSound("mob.zombiepig.zpigangry", "newsound/mob/zombiepig/zpigangry3.ogg");
	addLocalSound("mob.zombiepig.zpigangry", "newsound/mob/zombiepig/zpigangry4.ogg");
	addLocalSound("mob.zombiepig.zpighurt", "newsound/mob/zombiepig/zpighurt1.ogg");
	addLocalSound("mob.zombiepig.zpighurt", "newsound/mob/zombiepig/zpighurt2.ogg");
	addLocalSound("mob.zombiepig.zpigdeath", "newsound/mob/zombiepig/zpigdeath.ogg");

	addLocalSound("fire.fire", "newsound/fire/fire.ogg");
	addLocalSound("fire.ignite", "newsound/fire/ignite.ogg");

	addLocalSound("note.bd", "newsound/note/bd.ogg");
	addLocalSound("note.snare", "newsound/note/snare.ogg");
	addLocalSound("note.hat", "newsound/note/hat.ogg");
	addLocalSound("note.harp", "newsound/note/harp.ogg");
	addLocalSound("note.bassattack", "newsound/note/bassattack.ogg");

	addLocalSound("portal.portal", "newsound/portal/portal.ogg");
	addLocalSound("portal.travel", "newsound/portal/travel.ogg");
	addLocalSound("portal.trigger", "newsound/portal/trigger.ogg");

	addLocalSound("13", "streaming/13.ogg");
	addLocalSound("cat", "streaming/cat.ogg");

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

void SoundEngine::addLocalSound(std::string name, std::string path)
{
	int channels, sample_rate;
	short* output;
	int samples = m_pPlatform->loadOgg(path, &channels, &sample_rate, &output);

	if (samples > 0) {
		const PCMSoundHeader& pcm = { channels, sizeof(short), sample_rate, samples };
		const SoundDesc& sound = SoundDesc(name, pcm, output);
		m_repository.add(name, sound);
	}
}

void SoundEngine::addMusic(std::string name)
{
	addLocalSound(name, "newmusic/" + name + ".ogg");

	SoundDesc sd;
	if (m_repository.get(name, sd)) {
		m_musics.push_back(sd);
		LOG_I("Adding %s music", sd.m_name.c_str());
	}
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
	float vol = m_pOptions->m_fMasterVolume * volume;
	if (vol <= 0.0f)
		return;

	float cVolume = Mth::clamp(_getVolumeMult(pos) * vol, 0.0f, 1.0f);
	float cPitch = Mth::clamp(pitch, -1.0f, 1.0f);
	SoundDesc sd;

	if (m_repository.get(name, sd)) {
		m_pSoundSystem->playAt(sd, pos.x, pos.y, pos.z, cVolume, pitch);
	}
}

//@NOTE: As you can see, this is not really a streaming, it's just trying to have the same behavior. I don't know if I'll really implement the streaming
void SoundEngine::playStreaming(const std::string& name, const Vec3& pos, float volume, float pitch)
{
	if (!m_lastStreaming.empty())
	{
		m_pSoundSystem->stop(m_lastStreaming);
		m_lastStreaming = Util::EMPTY_STRING;
	}

	if (!name.empty())
	{
		play(name, pos, volume, pitch);
		m_lastStreaming = name;
	}
}

void SoundEngine::playMusicTick()
{
	if (m_musics.empty()) return;

	if (m_pOptions->m_fMusicVolume != 0.0F) {
		if ((m_lastMusic.empty() || !m_pSoundSystem->playing(m_lastMusic)) && (m_lastStreaming.empty() || !m_pSoundSystem->playing(m_lastStreaming))) {
			if (m_bNoMusicDelay > 0) {
				--m_bNoMusicDelay;
				return;
			}

			SoundDesc song = m_musics[m_random.nextInt(m_musics.size())];
			LOG_I("Playing %s music", song.m_name.c_str());
			m_bNoMusicDelay = m_random.nextInt(12000) + 12000;
			m_lastStreaming = m_lastMusic = song.m_name;
			m_pSoundSystem->playAt(song, 0, 0, 0, m_pOptions->m_fMusicVolume, 1.0f);
		}
	}
}
