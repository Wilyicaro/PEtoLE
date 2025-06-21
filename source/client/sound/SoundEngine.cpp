/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "SoundEngine.hpp"
#include "SoundDefs.hpp"
#include "common/Mth.hpp"

SoundEngine::SoundEngine(SoundSystem* soundSystem)
{
	m_pSoundSystem = soundSystem;
	m_pOptions = nullptr;
	field_40 = 0;
	field_A1C = 0;
	field_A20 = 0;
	m_muted = false;
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

	addLocalSound("fire.fire", "newsound/fire/fire.ogg");
	addLocalSound("fire.ignite", "newsound/fire/ignite.ogg");

}

void SoundEngine::addLocalSound(std::string name, std::string path)
{
	int channels, sample_rate;
	short* output;
	int samples = m_pPlatform->loadOgg(path, &channels, &sample_rate, &output);

	if (samples > 0) {
		const PCMSoundHeader& pcm = { channels, sizeof(short), sample_rate, samples };
		const SoundDesc& sound = SoundDesc(pcm, output);
		m_repository.add(name, sound);
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
