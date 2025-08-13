/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "client/app/Minecraft.hpp"
#include "client/gui/screens/PauseScreen.hpp"
#include "client/gui/screens/StartMenuScreen.hpp"
#include "client/gui/screens/RenameMPLevelScreen.hpp"
#include "client/gui/screens/SavingWorldScreen.hpp"
#include "client/gui/screens/DeathScreen.hpp"
#include "network/ServerSideNetworkHandler.hpp"
#include "client/network/ClientSideNetworkHandler.hpp"
#include "client/gui/screens/IntroScreen.hpp"
#include "client/gui/screens/ProgressScreen.hpp"

#include "world/gamemode/SurvivalMode.hpp"
#include "world/gamemode/CreativeMode.hpp"
#include "world/level/PortalForcer.hpp"

#include "client/player/input/Controller.hpp"
#include "client/player/input/ControllerBuildInput.hpp"
#include "client/player/input/ControllerMoveInput.hpp"
#include "client/player/input/ControllerTurnInput.hpp"
#include "client/player/input/MouseTurnInput.hpp"
#include "client/player/input/KeyboardInput.hpp"
#include "client/player/input/MouseBuildInput.hpp"
#include "client/player/input/CustomInputHolder.hpp"
#include "client/player/input/TouchInputHolder.hpp"
#include "client/player/input/Multitouch.hpp"

#include "world/tile/SandTile.hpp"

#include "client/renderer/GrassColor.hpp"
#include "client/renderer/FoliageColor.hpp"

// custom:
#include "client/renderer/PatchManager.hpp"
#include "client/locale/Language.hpp"

float Minecraft::_renderScaleMultiplier = 1.0f;

int Minecraft::width  = C_DEFAULT_SCREEN_WIDTH;
int Minecraft::height = C_DEFAULT_SCREEN_HEIGHT;
bool Minecraft::useAmbientOcclusion = false;
int Minecraft::customDebugId = 0;

//@HUH: For the demo, this is defined as TRUE.
//@HUH: deadmau5 had camera cheats? That's interesting.
const bool Minecraft::DEADMAU5_CAMERA_CHEATS = true;

const char* Minecraft::progressMessages[] =
{
	"Locating server",
	"Building terrain",
	"Preparing",
	"Saving chunks",
};

Minecraft::Minecraft() :
	m_gui(this)
{
	m_options = nullptr;
	field_18 = false;
	m_bIsGamePaused = false;
	m_pLevelRenderer = nullptr;
	m_pGameRenderer = nullptr;
	m_pParticleEngine = nullptr;
	m_pSoundEngine = nullptr;
	m_pGameMode = nullptr;
	m_pTextures = nullptr;
	m_pFont = nullptr;
	m_pRakNetInstance = nullptr;
	m_pNetEventCallback = nullptr;
	field_2B0 = 0;
	m_pUser = nullptr;
	m_pLevel = nullptr;
	m_pPlayer = nullptr;
	m_pMobPersp = nullptr; // why is there a duplicate? (this is used for the camera)
	field_D0C = 0;
	m_pScreen = nullptr;
	field_D18 = 10;
	m_pInputHolder = nullptr;
	m_bGrabbedMouse = false; // this was true by default. why? we do not start off in-game...
	m_bIsTouchscreen = false;
	m_pLevelStorageSource = nullptr; // TODO
	m_bIsLevelLoaded = false;
	m_lastBlockBreakTime = 0;
	field_DA8 = 0;
	field_DAC = 0;
	m_bUsingScreen = false;
	m_bHasQueuedScreen = false;
	m_pQueuedScreen = nullptr;
	m_licenseID = -2;
	m_fLastUpdated = 0;
	m_fDeltaTime = 0;
	m_lastInteractTime = 0;

	m_Logger = new Logger();
}

int Minecraft::getLicenseId()
{
	if (m_licenseID < 0)
		m_licenseID = m_pPlatform->checkLicense();

	return m_licenseID;
}

void Minecraft::releaseMouse()
{
	if (!m_bGrabbedMouse)
		return;

	if (m_pPlayer)
		m_pPlayer->m_pMoveInput->releaseAllKeys();

	m_bGrabbedMouse = false;
	m_mouseHandler.release();

	// Note, normally the platform stuff would be located within
	// the mouse handler, but we don't have access to the platform
	// from there!
	if (!useController() && !isTouchscreen())
		platform()->recenterMouse(); // don't actually try to grab or release the mouse
	platform()->setMouseGrabbed(false);
}

void Minecraft::grabMouse()
{
	if (m_bGrabbedMouse)
		return;

	m_bGrabbedMouse = true;
	m_mouseHandler.grab();

	// This will call grabMouse again, so why are we calling it here?
	//setScreen(nullptr);

	if (useController() || isTouchscreen())
		return; // don't actually try to grab the mouse

	platform()->setMouseGrabbed(true);
}

void Minecraft::setScreen(Screen* pScreen)
{
	if (pScreen == nullptr && !isLevelReady())
	{
		pScreen = new StartMenuScreen;
	}
	else if (pScreen == nullptr && m_pPlayer->m_health <= 0)
	{
		pScreen = new DeathScreen;
	}

	if (m_bUsingScreen)
	{
		m_bHasQueuedScreen = true;
		m_pQueuedScreen = pScreen;
		return;
	}

	if (pScreen && pScreen->isErrorScreen())
	{
		// not in original
		delete pScreen;
		return;
	}

	if (m_pScreen)
	{
		m_pScreen->removed();
		delete m_pScreen;
	}

	Mouse::reset();
	Multitouch::reset();
	Controller::reset();
	Multitouch::resetThisUpdate();

	m_pScreen = pScreen;
	if (pScreen)
	{
		releaseMouse();
		// the ceil prevents under-drawing
		pScreen->init(this, ceil(width * Gui::InvGuiScale), ceil(height * Gui::InvGuiScale));
	}
	else
	{
		grabMouse();
	}
}

void Minecraft::onGraphicsReset()
{
	m_pTextures->clear();
	m_pFont->onGraphicsReset();

	if (m_pLevelRenderer)
		m_pLevelRenderer->onGraphicsReset();

	if (m_pGameRenderer)
		m_pGameRenderer->onGraphicsReset();

	EntityRenderDispatcher::getInstance()->onGraphicsReset();
}

void Minecraft::saveOptions()
{
	if (platform()->hasFileSystemAccess())
		getOptions()->save();
}

bool Minecraft::isLevelReady() const
{
	return m_pLevel && m_async.empty();
}

void Minecraft::changeDimension(int dim)
{
	setScreen(new ProgressScreen);
	m_delayed.push_back(std::bind(&Minecraft::toggleDimension, this, dim));
}

bool Minecraft::isOnline() const
{
	return m_pNetEventCallback != nullptr;
}

bool Minecraft::isOnlineClient() const
{
	if (!m_pLevel)
		return false;

	return m_pLevel->m_bIsOnline;
}

bool Minecraft::isTouchscreen() const
{
	return m_bIsTouchscreen;
}

bool Minecraft::useSplitControls() const
{
	return !m_bIsTouchscreen || m_options->m_bSplitControls;
}

bool Minecraft::useController() const
{
	return m_pPlatform->hasGamepad() && m_options->m_bUseController;
}

GameMode* Minecraft::createGameMode(GameType gameType, Level& level)
{
	switch (gameType)
	{
	case GAME_TYPE_SURVIVAL:
		return new SurvivalMode(this, level);
	case GAME_TYPE_CREATIVE:
		return new CreativeMode(this, level);
	default:
		return nullptr;
	}
}

void Minecraft::setGameMode(GameType gameType)
{
	if (m_pLevel)
	{
		m_pGameMode = createGameMode(gameType, *m_pLevel);
		m_pGameMode->initLevel(m_pLevel);
	}
}

void Minecraft::handleBuildAction(const BuildActionIntention& action)
{
	auto player = m_pPlayer;
	bool canInteract = getTimeMs() - m_lastInteractTime >= 200;

	if (action.isDestroyStart() || action.isAttack())
	{
		player->swing();
	}
	
	if (!action.isDestroy() && !m_pGameMode->field_8) // from Minecraft::handleMouseDown
	{
		m_pGameMode->stopDestroyBlock();
	}

	if (!action.isInteract()) m_lastInteractTime = 0;

	bool bInteract = true;
	switch (m_hitResult.m_hitType)
	{
	case HitResult::ENTITY:
		if (action.isAttack())
		{
			m_pGameMode->attack(player.get(), m_hitResult.m_pEnt.get());
			m_lastBlockBreakTime = getTimeMs();
		}
		else if (action.isInteract() && canInteract)
		{
			if (m_hitResult.m_pEnt->interactPreventDefault())
				bInteract = false;

			m_pGameMode->interact(player.get(), m_hitResult.m_pEnt.get());
			m_lastInteractTime = getTimeMs();
		}
		break;
	case HitResult::TILE:
		Tile* pTile = Tile::tiles[m_pLevel->getTile(m_hitResult.m_tilePos)];

		if (action.isDestroy())
		{
			if (!pTile) return;

			player->swing();

			bool extinguished = m_pLevel->extinguishFire(player.get(), m_hitResult.m_tilePos, m_hitResult.m_hitSide);
	
			if (extinguished) break;

			if (pTile != Tile::bedrock || (player->m_userType >= 100 && !m_hitResult.m_bUnk24))
			{
				bool destroyed = false;
				if (action.isDestroyStart())
				{
					destroyed = m_pGameMode->startDestroyBlock(player.get(), m_hitResult.m_tilePos, m_hitResult.m_hitSide);
					player->startDestroying();
				}

				bool contDestory = m_pGameMode->continueDestroyBlock(player.get(), m_hitResult.m_tilePos, m_hitResult.m_hitSide);

				destroyed = destroyed || contDestory;
				m_pParticleEngine->crack(m_hitResult.m_tilePos, m_hitResult.m_hitSide);

				m_lastBlockBreakTime = getTimeMs();

				if (destroyed)
				{
					/*if (isVibrateOnBlockBreakOptionEnabledOrWhatever)
						platform()->vibrate(24);*/
				}
			}
		}
		else if (action.isPick())
		{
			// Try to pick the tile.
			player->m_pInventory->selectItemById(pTile->m_ID, C_MAX_HOTBAR_ITEMS);
		}
		else if (action.isPlace() && canInteract)
		{
			std::shared_ptr<ItemInstance> pItem = getSelectedItem();
			int oldCount = pItem ? pItem->m_count : 0;
			if (m_pGameMode->useItemOn(
					player.get(),
					m_pLevel,
					pItem,
					m_hitResult.m_tilePos,
					m_hitResult.m_hitSide)
				)
			{
				bInteract = false;

				player->swing();

				m_lastInteractTime = getTimeMs();

				if (!pItem) return;

				if (player->isCreative() && pItem->m_count != oldCount)
				{
					pItem->m_count = oldCount;
				}
				else if (!pItem->m_count)
				{
					m_pPlayer->m_pInventory->setItem(m_pPlayer->m_pInventory->m_selected, nullptr);
				}
				else if (pItem->m_count != oldCount)
				{
					m_pGameRenderer->m_pItemInHandRenderer->itemPlaced();
				}

				if (isOnline())
				{
					if (pItem->m_itemID > C_MAX_TILES || pItem->m_itemID < 0)
						return;

					TilePos tp(m_hitResult.m_tilePos);

					Facing::Name hitSide = m_hitResult.m_hitSide;

					if (m_pLevel->getTile(m_hitResult.m_tilePos) == Tile::topSnow->m_ID)
					{
						hitSide = Facing::DOWN;
					}

					m_pRakNetInstance->send(new PlaceBlockPacket(player->m_EntityID, tp.relative(hitSide), TileID(pItem->m_itemID), hitSide));
				}
			}
		}
		break;
	}

	if (bInteract && action.isInteract() && canInteract)
	{
		std::shared_ptr<ItemInstance> pItem = getSelectedItem();
		if (pItem)
		{
			m_lastInteractTime = getTimeMs();
			if (m_pGameMode->useItem(player.get(), m_pLevel, pItem))
				m_pGameRenderer->m_pItemInHandRenderer->itemUsed();
		}
	}
}

void Minecraft::tickInput()
{
	if (m_pScreen)
	{
		if (!m_pScreen->field_10)
		{
			m_bUsingScreen = true;
			m_pScreen->updateEvents();
			m_bUsingScreen = false;

			if (m_bHasQueuedScreen)
			{
				setScreen(m_pQueuedScreen);
				m_pQueuedScreen = nullptr;
				m_bHasQueuedScreen = false;
			}
			return;
		}
	}

	if (!m_pPlayer)
		return;

	bool bIsInGUI = m_gui.isInside(Mouse::getX(), Mouse::getY());

	while (Mouse::next())
	{
		if (getTimeMs() - field_2B4 > 200)
			continue;

		if (Mouse::isButtonDown(BUTTON_LEFT))
			m_gui.handleClick(1, Mouse::getX(), Mouse::getY());

		if (!bIsInGUI && getOptions()->field_19)
		{
			MouseButtonType buttonType = Mouse::getEventButton();

#ifdef ENH_ALLOW_SCROLL_WHEEL
			if (buttonType == BUTTON_SCROLLWHEEL)
			{
				if (Mouse::getEventButtonState() == 0)
				{
					// @NOTE: Scroll up
					m_gui.handleKeyPressed(getOptions()->getKey(KM_SLOT_L));
				}
				else
				{
					// @NOTE: Scroll down
					m_gui.handleKeyPressed(getOptions()->getKey(KM_SLOT_R));
				}
			}
#endif
		}
	}

	while (Keyboard::next())
	{
		int keyCode = Keyboard::getEventKey();
		bool bPressed = Keyboard::getEventKeyState() == 1;

		m_pPlayer->m_pMoveInput->setKey(keyCode, bPressed);

		if (bPressed)
		{
			m_gui.handleKeyPressed(keyCode);

			for (int i = 0; i < m_gui.getNumUsableSlots(); i++)
			{
				if (getOptions()->isKey(eKeyMappingIndex(KM_SLOT_1 + i), keyCode))
					m_pPlayer->m_pInventory->selectSlot(i);
			}

			if (getOptions()->isKey(KM_TOGGLE3RD, keyCode))
			{
				getOptions()->m_bThirdPerson = !getOptions()->m_bThirdPerson;
			}
			else if (getOptions()->isKey(KM_MENU_CANCEL, keyCode))
			{
				handleBack(false);
			}
			else if (getOptions()->isKey(KM_DROP, keyCode))
			{
				m_pPlayer->drop(m_pPlayer->m_pInventory->removeItem(m_pPlayer->m_pInventory->m_selected, 1));
				
			}
			else if (getOptions()->isKey(KM_TOGGLEGUI, keyCode))
			{
				getOptions()->m_bDontRenderGui = !getOptions()->m_bDontRenderGui;
			}
			else if (getOptions()->isKey(KM_TOGGLEDEBUG, keyCode))
			{
				getOptions()->m_bDebugText = !getOptions()->m_bDebugText;
			}
#ifdef ENH_ALLOW_AO_TOGGLE
			else if (getOptions()->isKey(KM_TOGGLEAO, keyCode))
			{
				// Toggle ambient occlusion.
				getOptions()->m_bAmbientOcclusion = !getOptions()->m_bAmbientOcclusion;
				Minecraft::useAmbientOcclusion = getOptions()->m_bAmbientOcclusion;
				m_pLevelRenderer->allChanged();
			}
#endif
		}

		if (getOptions()->field_19)
			continue;

		// @TODO: Replace with KeyboardBuildInput
		if (!useController() && getTimeMs() - field_2B4 <= 200)
		{
			if (getOptions()->getKey(KM_DESTROY) == keyCode && bPressed)
			{
				BuildActionIntention intention(BuildActionIntention::KEY_DESTROY);
				handleBuildAction(intention);
			}

			if (getOptions()->getKey(KM_PLACE) == keyCode && bPressed)
			{
				BuildActionIntention intention(BuildActionIntention::KEY_USE);
				handleBuildAction(intention);
			}
		}
	}

	BuildActionIntention bai;
	IBuildInput* buildInput = m_pInputHolder->getBuildInput();
	if (buildInput && buildInput->tickBuild(m_pPlayer.get(), &bai))
		handleBuildAction(bai);

	field_2B4 = getTimeMs();

	Keyboard::reset();
	Mouse::reset();
}

void Minecraft::tickMouse()
{
	if (!m_bGrabbedMouse)
		return;

	/**
	 * iProgramInCpp's explanation on why we're recentering the mouse constantly when it's grabbed:
	 * we would recenter the mouse every frame to avoid it being stopped by the edges of the window
	 * when we grab the mouse, the Win32 AppPlatform calls ClipCursor(hWnd) which clips the cursor within the confines of the window and does not let it escape
	 * if we weren't doing that, we'd
	 * not receive WM_MOUSEMOVE events outside of the window and
	 * even if we would, we'd have the exact same problem with the edges of the screen
	 * so to fix these issues we clip the cursor within the window, and every frame we set its position back to the center.
	 * This is exactly what Minecraft Java does too
	**/

	if (useController() || isTouchscreen())
		return; // don't actually try to recenter the mouse

	platform()->recenterMouse();
}

void Minecraft::handleCharInput(char chr)
{
	if (m_pScreen)
		m_pScreen->keyboardNewChar(chr);
}

void Minecraft::resetInput()
{
	Keyboard::reset();
	Mouse::reset();
	Controller::reset();
	Multitouch::resetThisUpdate();
}

void Minecraft::sendMessage(const std::string& message)
{
	if (isOnlineClient())
	{
		// send the server a message packet
		if (m_pRakNetInstance)
			m_pRakNetInstance->send(new MessagePacket(message));
		else
			m_gui.addMessage("You aren't actually playing multiplayer!");
	}
	else
	{
		// fake the server having received a packet
		MessagePacket mp(message);
		if (m_pNetEventCallback && m_pRakNetInstance)
			m_pNetEventCallback->handle(m_pRakNetInstance->m_pRakPeerInterface->GetMyGUID(), &mp);
		else
			m_gui.addMessage("You aren't hosting a multiplayer server!");
	}
}

void Minecraft::resetPlayer(std::shared_ptr<Player> player)
{
	if (!m_pLevel->m_bIsOnline && !m_pLevel->m_pDimension->mayRespawn())
		toggleDimension();
	m_pLevel->validateSpawn();
	player->reset();

	TilePos pos = m_pLevel->getSharedSpawnPos();
	if (player->m_bHasRespawnPos) 
	{
		TilePos respawnPos = player->getRespawnPosition();
		TilePos checkedPos = Player::checkRespawnPos(m_pLevel, respawnPos);
		if (checkedPos != respawnPos)
			pos = checkedPos;
		else
			player->displayClientMessage("tile.bed.notValid");
	}
	player->setPos(Vec3(pos.x + 0.5, pos.y + 0.1, pos.z + 0.5));
	player->resetPos();

	// Of course we have to add him back into the game, if he isn't already.
	EntityVector& vec = m_pLevel->m_entities;
	for (int i = 0; i < int(vec.size()); i++)
	{
		if (vec[i] == player)
			return;
	}

	auto& vec2 = m_pLevel->m_players;
	for (int i = 0; i < int(vec2.size()); i++)
	{
		// remove the player if he is already in the player list
		if (vec2[i] == player)
		{
			vec2.erase(vec2.begin() + i);
			i--;
		}
	}

	// add him in!!
	m_pLevel->addEntity(player);

	prepareLevel("Respawning");
}

void Minecraft::respawnPlayer(std::shared_ptr<Player> player)
{
	setScreen(new ProgressScreen);
	m_delayed.push_back(std::bind(&Minecraft::resetPlayer, this, player));
	// TODO: send a RespawnPacket
}

std::string Minecraft::getVersionString() const
{
	return "b1.6.6 - PEtoLE";
}

void Minecraft::_reloadInput()
{
	if (m_pInputHolder)
		delete m_pInputHolder;

	if (isTouchscreen())
	{
		m_pInputHolder = new TouchInputHolder(this, m_options);
	}
	else if (useController())
	{
		m_pInputHolder = new CustomInputHolder(
			new ControllerMoveInput(m_options),
			new ControllerTurnInput(),
			new ControllerBuildInput()
		);
	}
	else
	{
		m_pInputHolder = new CustomInputHolder(
			new KeyboardInput(m_options),
			new MouseTurnInput(this),
			new MouseBuildInput()
		);
	}

	m_mouseHandler.setTurnInput(m_pInputHolder->getTurnInput());

	if (m_pLevel && m_pPlayer)
	{
		m_pPlayer->m_pMoveInput = m_pInputHolder->getMoveInput();
	}

	m_options->field_19 = !isTouchscreen();
}

void Minecraft::_levelGenerated()
{
	if (m_pNetEventCallback)
		m_pNetEventCallback->levelGenerated(m_pLevel);
}

void Minecraft::tick()
{
	for (auto it = m_delayed.begin(); it != m_delayed.end(); )
	{
		m_async.emplace_back(*it);
		it = m_delayed.erase(it);
	}

	for (auto it = m_async.begin(); it != m_async.end(); )
	{
		if (it->isFinished()) it = m_async.erase(it);
		else ++it;
	}

	if (!m_pScreen)
	{
		if (m_pPlayer && m_pPlayer->m_health <= 0)
		{
			setScreen(new DeathScreen);
		}
	}

	tickInput();

	m_gui.tick();

	if (m_async.empty())
	{
		SandTile::instaFall = false;

		if (m_pLevel && !isGamePaused())
		{
            m_pLevel->m_difficulty = m_options->m_difficulty;
            if (m_pLevel->m_bIsOnline)
            {
                m_pLevel->m_difficulty = 3;
            }
            
			m_pGameMode->tick();
			m_pSoundEngine->playMusicTick();
			m_pGameRenderer->tick();
			m_pLevelRenderer->tick();
			if (m_pLevelManager)
				m_pLevelManager->tick();
			else
			{
				m_pLevel->tickEntities();
				m_pLevel->tick();
			}

			if (m_pPlayer)
			{
				m_pLevel->animateTick(m_pPlayer->m_pos);
			}
		}

		m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);

		if (!isGamePaused())
		{
			m_pTextures->tick();
			m_pParticleEngine->tick();

#ifndef ORIGINAL_CODE
			if (m_pMobPersp)
			{
				m_pSoundEngine->m_pSoundSystem->setListenerPos(m_pMobPersp->m_pos.x, m_pMobPersp->m_pos.y, m_pMobPersp->m_pos.z);
				m_pSoundEngine->m_pSoundSystem->setListenerAngle(m_pMobPersp->m_rot.y, m_pMobPersp->m_rot.x);
			}
#endif

		}

		if (m_pScreen)
			m_pScreen->tick();

		Multitouch::reset();


		m_bIsGamePaused = (!isOnline() || m_pLevelManager && m_pLevelManager->m_pConnection->m_onlinePlayers.size() == 1) && m_pScreen && m_pScreen->isPauseScreen();
	}
}

void Minecraft::update()
{
	if (isGamePaused() && m_pLevel)
	{
		// Don't advance renderTicks when we're paused
		float x = m_timer.m_renderTicks;
		m_timer.advanceTime();
		m_timer.m_renderTicks = x;
	}
	else
	{
		m_timer.advanceTime();
	}

	if (m_pRakNetInstance)
	{
		m_pRakNetInstance->runEvents(m_pNetEventCallback);
	}

	for (int i = 0; i < m_timer.m_ticks; i++)
	{
		tick(); // tick(i, m_timer.m_ticks - 1); // 0.9.2
		field_DA8++;
	}

	if (m_pLevel && m_async.empty())
	{
		m_pLevel->updateLights();
	}

#ifndef ORIGINAL_CODE
	tickMouse();
#endif

	if (m_pGameMode)
		m_pGameMode->render(m_timer.m_renderTicks);

	m_pGameRenderer->render(m_timer.m_renderTicks);


	double time = getTimeS();
	m_fDeltaTime = time - m_fLastUpdated;
	m_fLastUpdated = time;
}

void Minecraft::init()
{
	// Optional features that you really should be able to get away with not including.
	Screen::setIsMenuPanoramaAvailable(platform()->doesTextureExist("gui/background/panorama_0.png"));

	GetPatchManager()->LoadPatchData(platform()->getPatchData());

	m_bIsTouchscreen = platform()->isTouchscreen();

	m_pRakNetInstance = new RakNetInstance;

	if (platform()->hasFileSystemAccess())
		m_options = new Options(m_externalStorageDir);
	else
		m_options = new Options();

	m_pTextures = new Textures(m_options, platform());
	m_pTextures->addDynamicTexture(new WaterTexture);
	m_pTextures->addDynamicTexture(new WaterSideTexture);
	m_pTextures->addDynamicTexture(new LavaTexture);
	m_pTextures->addDynamicTexture(new LavaSideTexture);
	m_pTextures->addDynamicTexture(new FireTexture(0));
	m_pTextures->addDynamicTexture(new FireTexture(1));
	m_pTextures->addDynamicTexture(new PortalTexture);
	m_pTextures->addDynamicTexture(new CompassTexture(this));
	m_pTextures->addDynamicTexture(new ClockTexture(this));

	m_options->m_bUseController = platform()->hasGamepad();
	m_options->loadControls();

	_reloadInput();

	m_pTextures->loadAndBindTerrain();
	GetPatchManager()->PatchTextures(platform(), TYPE_TERRAIN);
	m_pTextures->loadAndBindTexture(C_ITEMS_NAME);
	GetPatchManager()->PatchTextures(platform(), TYPE_ITEMS);

	GetPatchManager()->PatchTiles();

	m_pSoundEngine = new SoundEngine(platform()->getSoundSystem());

	m_pLevelRenderer = new LevelRenderer(this, m_pTextures);
	m_pGameRenderer = new GameRenderer(this);
	m_pParticleEngine = new ParticleEngine(m_pLevel, m_pTextures);
	m_pUser = new User(getOptions()->m_playerName, "");

	m_pFont = new Font(m_options, "font/default.png", m_pTextures);

	setScreen(new IntroScreen);
	m_async.emplace_back(std::bind(&Minecraft::initAssets, this));

}

void Minecraft::initAssets()
{
	m_pSoundEngine->init(m_options, m_pPlatform);
	GrassColor::init(m_pPlatform->loadTexture("misc/grasscolor.png", true));
	FoliageColor::init(m_pPlatform->loadTexture("misc/foliagecolor.png", true));
	Language::getInstance()->init();
}

Minecraft::~Minecraft()
{
	SAFE_DELETE(m_options);
	SAFE_DELETE(m_pNetEventCallback);
	SAFE_DELETE(m_pRakNetInstance);
	SAFE_DELETE(m_pLevelRenderer);
	SAFE_DELETE(m_pGameRenderer);
	SAFE_DELETE(m_pParticleEngine);
	SAFE_DELETE(m_pSoundEngine);
	SAFE_DELETE(m_pGameMode);
	SAFE_DELETE(m_pFont);
	SAFE_DELETE(m_pTextures);
	SAFE_DELETE(m_pLevel);
	SAFE_DELETE(m_pUser);
	SAFE_DELETE(m_pLevelStorageSource);
	SAFE_DELETE(m_pInputHolder);
	SAFE_DELETE(m_Logger);
}

void Minecraft::prepareLevel(const std::string& text)
{
	m_progress.progressStart(text);
	m_progress.progressStage("Building terrain");

	if (!m_pLevel->m_bIsNew)
	{
		m_pLevel->setUpdateLights(0);
	}

	TilePos pos = m_pLevel->getSharedSpawnPos();

	if (m_pPlayer)
		pos = m_pPlayer->m_pos;

	int delay = 0;
	TilePos tp(0, 64, 0);
	for (int dx = -128; dx <= 128; dx += 16) {
		for (int dz = -128; dz <= 128; dz += 16) {
			tp.x = pos.x + dx;
			tp.z = pos.z + dz;
			m_progress.progressStagePercentage(delay++ * 100 / 289);
			m_pLevel->getTile(tp);

			while (m_pLevel->updateLights()) {
			}
		}
	}

	m_pLevel->setUpdateLights(1);

	m_pLevel->prepare();

	m_progress.progressStage("Simulating world for a bit");
}

void Minecraft::sizeUpdate(int newWidth, int newHeight)
{
	// re-calculate the GUI scale.
	Gui::InvGuiScale = getBestScaleForThisScreenSize(newWidth, newHeight) / getRenderScaleMultiplier();

	// The ceil gives an extra pixel to the screen's width and height, in case the GUI scale doesn't
	// divide evenly into width or height, so that none of the game screen is uncovered.
	if (m_pScreen)
		m_pScreen->setSize(
			int(ceilf(Minecraft::width * Gui::InvGuiScale)),
			int(ceilf(Minecraft::height * Gui::InvGuiScale))
		);

	if (m_pInputHolder)
		m_pInputHolder->setScreenSize(Minecraft::width, Minecraft::height);
}

float Minecraft::getBestScaleForThisScreenSize(int width, int height)
{
//#define USE_JAVA_SCREEN_SCALING
#ifdef USE_JAVA_SCREEN_SCALING
	int scale;
	for (scale = 1; width / (scale + 1) >= 320 && height / (scale + 1) >= 240; ++scale)
	{
	}
	return 1.0f / scale;
#endif

	if (height > 1800)
		return 1.0f / 8.0f;

	if (isTouchscreen())
	{
		if (height > 1100)
			return 1.0f / 6.0f;

		if (height > 900)
			return 1.0f / 5.0f;

		if (height > 700)
			return 1.0f / 4.0f;

		if (height > 500)
			return 1.0f / 3.0f;

		if (height > 300)
			return 1.0f / 2.0f;
	}
	else
	{
		if (height > 1600)
			return 1.0f / 4.0f;

		if (height > 800)
			return 1.0f / 3.0f;

		if (height > 400)
			return 1.0f / 2.0f;
	}

	return 1.0f;
}

bool Minecraft::pauseGame()
{
	if (isGamePaused() || m_pScreen) return false;
	m_pLevel->savePlayerData();
	setScreen(new PauseScreen);

	return true;
}

bool Minecraft::resumeGame()
{
	m_bIsGamePaused = false;
	if (m_pScreen)
		setScreen(nullptr);

	return true;
}

void Minecraft::setLevel(Level* pLevel, const std::string& text, std::shared_ptr<LocalPlayer> pLocalPlayer)
{
	m_progress.progressStart(text);
	m_progress.progressStage(Util::EMPTY_STRING);
	m_pMobPersp = nullptr;
	m_pSoundEngine->playStreaming(Util::EMPTY_STRING);

	if (pLevel)
	{
		if (pLocalPlayer && m_pPlayer == nullptr)
		{
			// We're getting a LocalPlayer from a server
			m_pPlayer = pLocalPlayer;
			pLocalPlayer->resetPos();
		}
		//@NOTE: Removing this for now, as generateLevel always adds the player, so this was causing problems when toggling dimensions...
		//else if (m_pLocalPlayer)
		//{
		//	m_pLocalPlayer->resetPos();
		//	pLevel->addEntity(m_pLocalPlayer);
		//}

		m_pLevel = pLevel;
		setGameMode(m_pPlayer ? m_pPlayer->getPlayerGameType() : pLevel->getDefaultGameType());
		if (!pLevel->m_bIsOnline)
			prepareLevel(text);


		std::shared_ptr<LocalPlayer> pLocalPlayer = m_pPlayer;
		if (!pLocalPlayer)
		{
			pLocalPlayer = m_pGameMode->createPlayer(pLevel);
			m_pPlayer = pLocalPlayer;
			m_pPlayer->resetPos();
			m_pGameMode->initPlayer(pLocalPlayer.get());
		}

		if (pLocalPlayer)
			pLocalPlayer->m_pMoveInput = m_pInputHolder->getMoveInput();

		if (m_pLevelRenderer)
			m_pLevelRenderer->setLevel(pLevel);

		if (m_pParticleEngine)
			m_pParticleEngine->setLevel(pLevel);

		m_pGameMode->adjustPlayer(m_pPlayer.get());

		pLevel->validateSpawn();
		pLevel->loadPlayer(m_pPlayer);
	
		if (pLevel->m_bIsNew) 
			m_pLevel->save(true, m_progress);
		if (m_pGameMode->isCreativeType() != m_pPlayer->getPlayerGameType()) setGameMode(m_pPlayer->getPlayerGameType());

		m_pMobPersp = m_pPlayer;
		m_pLevel = pLevel;


		if (m_pRakNetInstance->m_bIsHost && m_pLevelManager && m_pLevelManager->m_pConnection->m_onlinePlayers.empty())
		{
			m_pRakNetInstance->announceServer(m_pUser->m_guid);
			_levelGenerated();
		}
		else if (!m_pRakNetInstance->m_bIsHost)
			_levelGenerated();
	}
	else
	{
		m_pLevel->save(true, m_progress);
		m_pPlayer = nullptr;
		m_pRakNetInstance->disconnect();
		m_pLevelRenderer->setLevel(nullptr);
		m_pParticleEngine->setLevel(nullptr);
		delete m_pLevelManager;
		m_pLevelManager = nullptr;
		m_pLevel = nullptr;
		m_pNetEventCallback = nullptr;
	}
}

void Minecraft::toggleDimension(int dim)
{
	if (m_pPlayer->m_dimension == dim) {
		m_pPlayer->m_dimension = 0;
	}
	else {
		m_pPlayer->m_dimension = dim;
	}

	m_pLevel->removeEntity(m_pPlayer);
	m_pLevel->getChunk(m_pPlayer->m_chunkPos)->removeEntity(m_pPlayer);
	m_pPlayer->m_bRemoved = false;
	Vec3 fPos(m_pPlayer->m_pos);
	const constexpr real teleportConst = 8.0;
	Level* newLevel;
	if (m_pPlayer->m_dimension == dim)
	{
		fPos.x /= teleportConst;
		fPos.z /= teleportConst;
		m_pPlayer->moveTo(fPos, m_pPlayer->m_rot);
		if (m_pPlayer->isAlive())
			m_pLevel->tick(m_pPlayer, false);
		newLevel = m_pLevelManager->getLevel(dim);
		setLevel(newLevel, "Entering the Nether", m_pPlayer);
	}
	else
	{
		fPos.x *= teleportConst;
		fPos.z *= teleportConst;
		m_pPlayer->moveTo(fPos, m_pPlayer->m_rot);
		if (m_pPlayer->isAlive())
			m_pLevel->tick(m_pPlayer, false);
		newLevel = m_pLevelManager->getLevel(0);
		setLevel(newLevel, "Leaving the Nether", m_pPlayer);
	}

	m_pPlayer->m_pLevel = m_pLevel;
	if (m_pPlayer->isAlive())
	{
		m_pPlayer->moveTo(fPos, m_pPlayer->m_rot);
		m_pLevel->tick(m_pPlayer, false);
		PortalForcer().force(m_pLevel, m_pPlayer.get());
	}
}

void Minecraft::selectLevel(const std::string& a, const std::string& b, int64_t c)
{
	setScreen(new ProgressScreen);
	m_pLevelManager = m_pLevelStorageSource->selectLevel(a, false);
	hostMultiplayer();
	if (m_pLevelManager->m_bIsNew)
	{
		m_pLevelManager->m_levelData.setSeed(c);
		m_pLevelManager->m_levelData.setLevelName(b);
		m_delayed.push_back(std::bind(&Minecraft::setLevel, this, m_pLevelManager->getLevel(), "Generating level", nullptr));
	}
	else
		m_delayed.push_back(std::bind(&Minecraft::setLevel, this, m_pLevelManager->getLevel(m_pLevelManager->m_levelData.getDimension()), "Loading level", nullptr));
	m_bIsLevelLoaded = true;
}

LevelStorageSource* Minecraft::getLevelSource()
{
	return m_pLevelStorageSource;
}

std::shared_ptr<ItemInstance> Minecraft::getSelectedItem()
{
	return m_pPlayer->getSelectedItem();
}

int Minecraft::getFpsIntlCounter()
{
	return 0;
}

void Minecraft::leaveGame()
{
	m_bIsGamePaused = true;
	setScreen(new ProgressScreen);
	m_delayed.push_back(std::bind(&Minecraft::setLevel, this, nullptr, "", nullptr));
	m_bIsLevelLoaded = false;
}

void Minecraft::hostMultiplayer()
{
#ifndef __EMSCRIPTEN__
	m_pRakNetInstance->host(m_pUser->m_guid, C_DEFAULT_PORT, C_MAX_CONNECTIONS);
	ServerSideNetworkHandler* handler = new ServerSideNetworkHandler(this, m_pRakNetInstance);
	m_pNetEventCallback = handler;
	m_pLevelManager->setConnection(handler);
#endif
}

void Minecraft::joinMultiplayer(const PingedCompatibleServer& serverInfo)
{
#ifndef __EMSCRIPTEN__
	if (field_18 && m_pNetEventCallback)
	{
		field_18 = false;
		m_pRakNetInstance->connect(serverInfo.m_address.ToString(), serverInfo.m_address.GetPort());
	}
#endif
}

void Minecraft::cancelLocateMultiplayer()
{
#ifndef __EMSCRIPTEN__
	field_18 = false;
	m_pRakNetInstance->stopPingForHosts();
	delete m_pNetEventCallback;
	m_pNetEventCallback = nullptr;
#endif
}

void Minecraft::locateMultiplayer()
{
#ifndef __EMSCRIPTEN__
	field_18 = true;
	m_pRakNetInstance->pingForHosts(C_DEFAULT_PORT);
	m_pNetEventCallback = new ClientSideNetworkHandler(this, m_pRakNetInstance);
#endif
}
