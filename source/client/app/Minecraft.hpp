/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "App.hpp"
#include <functional>
#include <thread>
#include "common/Mth.hpp"
#include "common/Timer.hpp"
#include "client/gui/Gui.hpp"
#include "client/gui/Screen.hpp"
#include "network/RakNetInstance.hpp"
#include "network/NetEventCallback.hpp"
#include "client/player/input/IInputHolder.hpp"
#include "client/player/input/MouseHandler.hpp"
#include "client/player/input/BuildActionIntention.hpp"
#include "client/renderer/GameRenderer.hpp"
#include "client/renderer/LevelRenderer.hpp"
#include "client/renderer/entity/EntityRenderDispatcher.hpp"
#include "client/sound/SoundEngine.hpp"
#include "world/level/Level.hpp"
#include "world/entity/LocalPlayer.hpp"
#include "world/gamemode/GameMode.hpp"
#include "world/gamemode/GameType.hpp"
#include "world/particle/ParticleEngine.hpp"
#include <stdint.h>

class Screen; // in case we're included from Screen.hpp


class AsyncTask {
	std::thread t;
	std::atomic<bool> finished;

public:
	AsyncTask(std::function<void()> func) : finished(false) {
		t = std::thread([this, func]() {
			func();
			finished = true;
			});
	}

	~AsyncTask() {
		if (t.joinable())
			t.join();
	}

	bool isFinished() const {
		return finished.load();
	}

	AsyncTask(AsyncTask&& other) noexcept : t(std::move(other.t)), finished(other.finished.load()) {
		other.finished = true;
	}

	AsyncTask& operator=(AsyncTask&& other) noexcept {
		if (this != &other) {
			if (t.joinable())
				t.join();

			t = std::move(other.t);
			finished = other.finished.load();
			other.finished = true;
		}
		return *this;
	}
};

class Minecraft : public App
{
public:
	Minecraft();
	virtual ~Minecraft();

	int getLicenseId();
	void setScreen(Screen * pScreen);
	void releaseMouse();
	void grabMouse();
	void tick();
	void tickInput();
	void saveOptions();
	void handleBuildAction(const BuildActionIntention& action);
	bool isLevelReady() const;
	void changeDimension(int dim = -1);
	void selectLevel(const std::string&, std::function<void(LevelData&)> = {});
	void setLevel(Level*, const std::string&, std::shared_ptr<LocalPlayer>);
	void toggleDimension(int dim = -1);
	void changeLevel(Level*);
	bool pauseGame();
	bool resumeGame();
	void leaveGame();
	void hostMultiplayer();
	void joinMultiplayer(const PingedCompatibleServer& serverInfo);
	void cancelLocateMultiplayer();
	void locateMultiplayer();
	void tickMouse();
	void handleCharInput(char chr);
	void resetInput();
	void sendMessage(const std::string& message);
	void resetPlayer(std::shared_ptr<Player> player);
	void respawnPlayer(std::shared_ptr<Player> player);
	std::string getVersionString() const;
	bool isTouchscreen() const;
	bool useSplitControls() const;
	bool useController() const;

	virtual void update() override;
	virtual void init() override;
	virtual void initAssets();
	virtual void onGraphicsReset();
	virtual void sizeUpdate(int newWidth, int newHeight) override;
	virtual void resizeDisplay(int guiScale, int newWidth = Minecraft::width, int newHeight = Minecraft::height);
	virtual int getFpsIntlCounter();

	float getBestScaleForThisScreenSize(int guiScale, int width, int height);
	void prepareLevel(const std::string& text);
	bool isOnline() const;
	bool isOnlineClient() const;
	bool isGamePaused() const { return m_bIsGamePaused; }

	LevelStorageSource* getLevelSource();
	std::shared_ptr<ItemInstance> getSelectedItem();
	Options* getOptions() const { return m_options; }
	
private:
	void _reloadInput();
	void _levelGenerated();

private:
    // Value provided by the OS
    static float _renderScaleMultiplier;
public:
    static float getRenderScaleMultiplier() { return _renderScaleMultiplier; }
    static void setRenderScaleMultiplier(float value) { _renderScaleMultiplier = value; }
    
public:
	static int width, height;
	static bool useAmbientOcclusion;
	static const char* progressMessages[];
	static const bool DEADMAU5_CAMERA_CHEATS;
	static int customDebugId;

private:
	Logger *m_Logger;
	Options *m_options;

public:
	bool m_bLocateMultiplayer;
	bool m_bIsGamePaused;
	LevelRenderer* m_pLevelRenderer;
	GameRenderer* m_pGameRenderer;
	ParticleEngine* m_pParticleEngine;
	SoundEngine* m_pSoundEngine;
	GameMode* m_pGameMode;
	Textures* m_pTextures;
	Font* m_pFont;
	RakNetInstance* m_pRakNetInstance;
	NetEventCallback* m_pNetEventCallback;
	int field_2B0;
	int field_2B4;
	int field_2B8;
	User* m_pUser;
	Level* m_pLevel;
	MinecraftServer* m_pMinecraftServer;
	std::shared_ptr<LocalPlayer> m_pPlayer;
	std::shared_ptr<Mob> m_pMobPersp;
	Gui m_gui;
	int field_D0C;
	std::vector<std::function<void()>> m_delayed;
	std::vector<AsyncTask> m_async;
	Screen* m_pScreen;
	int field_D18;
	IInputHolder* m_pInputHolder;
	MouseHandler m_mouseHandler;
	bool m_bGrabbedMouse;
	bool m_bIsTouchscreen;
	HitResult m_hitResult;
	ProgressListener m_progress;
	std::string m_externalStorageDir;
	Timer m_timer;
	LevelStorageSource* m_pLevelStorageSource; // TODO
	bool m_bIsLevelLoaded;
	int m_lastBlockBreakTime;
	int field_DA8;
	int field_DAC;
	bool m_bUsingScreen;
	bool m_bHasQueuedScreen;
	Screen* m_pQueuedScreen;
	int m_licenseID;
	ItemInstance m_CurrItemInstance;

	// in 0.8. Offset 3368
	double m_fDeltaTime, m_fLastUpdated;
	int m_lastActionTime;
};

