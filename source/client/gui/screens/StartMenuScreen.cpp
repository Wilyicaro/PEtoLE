/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "StartMenuScreen.hpp"
#include "InvalidLicenseScreen.hpp"
#include "OptionsScreen.hpp"
#include "ProgressScreen.hpp"
#include "SelectWorldScreen.hpp"
#include "JoinGameScreen.hpp"
#include "client/locale/Language.hpp"

#if defined(_WIN32) || (defined(TARGET_OS_MAC) && TARGET_OS_IPHONE == 0)
#define CAN_QUIT
#endif

// special mode so that we can crop out the title:
//#define TITLE_CROP_MODE

const char gLogoLine1[] = "??? ??? #   # # #   # ### ### ### ### ### ### $$$ $$$";
const char gLogoLine2[] = "? ? ?   ## ## # ##  # #   #   # # # # #    #  $ $ $  ";
const char gLogoLine3[] = "??  ??  # # # # # # # ##  #   ##  ### ##   #  $$  $$ ";
const char gLogoLine4[] = "? ? ?   #   # # #  ## #   #   # # # # #    #  $   $  ";
const char gLogoLine5[] = "? ? ??? #   # # #   # ### ### # # # # #    #  $   $$$";

const char* gLogoLines[] = {
	gLogoLine1,
	gLogoLine2,
	gLogoLine3,
	gLogoLine4,
	gLogoLine5,
};

// actual name
const char* gSplashes[] =
{
	"One star! Deal with it notch!",
	"100% more yellow text!",
	"Glowing creepy eyes!",
	"Toilet friendly!",
	"Annoying touch buttons!",
	"Astronomically accurate!",
	"0xffff-1 chunks!",
	"Cubism!",
	"Pocket!",
	"Mostly harmless!",
	"!!!1!",
	"Dramatic lighting!",
	"As seen on TV!",
	"Awesome!",
	"100% pure!",
	"May contain nuts!",
	"Better than Prey!",
	"Less polygons!",
	"Sexy!",
	"Limited edition!",
	"Flashing letters!",
	"Made by Mojang!",
	"It's here!",
	"Best in class!",
	"It's alpha!",
	"100% dragon free!",
	"Excitement!",
	"More than 500 sold!",
	"One of a kind!",
	"Heaps of hits on YouTube!",
	"Indev!",
	"Spiders everywhere!",
	"Check it out!",
	"Holy cow, man!",
	"It's a game!",
	"Made in Sweden!",
	"Uses C++!",
	"Reticulating splines!",
	"Minecraft!",
	"Yaaay!",
	"Multiplayer!",
	"Touch compatible!",
	"Undocumented!",
	"Ingots!",
	"Exploding creepers!",
	"That's no moon!",
	"l33t!",
	"Create!",
	"Survive!",
	"Dungeon!",
	"Exclusive!",
	"The bee's knees!",
	"Down with O.P.P.!",
	"Open source!", //"Closed source!",
	"Classy!",
	"Wow!",
	"Not on steam!",
	"Oh man!",
	"Awesome community!",
	"Pixels!",
	"Teetsuuuuoooo!",
	"Kaaneeeedaaaa!",
	"Enhanced!",
	"90% bug free!",
	"Pretty!",
	"12 herbs and spices!",
	"Fat free!",
	"Absolutely no memes!",
	"Free dental!",
	"Ask your doctor!",
	"Minors welcome!",
	"Cloud computing!",
	"Legal in Finland!",
	"Hard to label!",
	"Technically good!",
	"Bringing home the bacon!",
	"Quite Indie!",
	"GOTY!",
	"Euclidian!",
	"Now in 3D!",
	"Inspirational!",
	"Herregud!",
	"Complex cellular automata!",
	"Yes, sir!",
	"Played by cowboys!",
#ifdef USE_GLES
	"OpenGL ES 1.1!",
#else
	"OpenGL 1.5!",
#endif
	"Thousands of colors!",
	"Try it!",
	"Age of Wonders is better!",
	"Try the mushroom stew!",
	"Sensational!",
	"Hot tamale, hot hot tamale!",
	"Play him off, keyboard cat!",
	"Guaranteed!",
	"Macroscopic!",
	"Bring it on!",
	"Random splash!",
	"Call your mother!",
	"Monster infighting!",
	"Loved by millions!",
	"Ultimate edition!",
	"Freaky!",
	"You've got a brand new key!",
	"Water proof!",
	"Uninflammable!",
	"Whoa, dude!",
	"All inclusive!",
	"Tell your friends!",
	"NP is not in P!",
	"Notch <3 ez!",
	"Livestreamed!",
	"Haunted!",
	"Polynomial!",
	"Terrestrial!",
	"All is full of love!",
	"Full of stars!",
	"Scientific!",
	"Cooler than Spock!",
	"Collaborate and listen!",
	"Never dig down!",
	"Take frequent breaks!",
	"Not linear!",
	"Han shot first!",
	"Nice to meet you!",
	"Buckets of lava!",
	"Ride the pig!",
	"Larger than Earth!",
	"sqrt(-1) love you!",
	"Phobos anomaly!",
	"Punching wood!",
	"Falling off cliffs!",
	"0% sugar!",
	"150% hyperbole!",
	"Synecdoche!",
	"Let's danec!",
	"Seecret Friday update!",
	"Ported implementation!",
	"Lewd with two dudes with food!",
	"Kiss the sky!",
	"20 GOTO 10!",
	"Verlet intregration!", // [sic]
	"Peter Griffin!",
	"Do not distribute!",
	"Cogito ergo sum!",
	"4815162342 lines of code!",
	"A skeleton popped out!",
	"The Work of Notch!",
	"The sum of its parts!",
	"BTAF used to be good!",
	"I miss ADOM!",
	"umop-apisdn!",
	"OICU812!",
	"Bring me Ray Cokes!",
	"Finger-licking!",
	"Thematic!",
	"Pneumatic!",
	"Sublime!",
	"Octagonal!",
	"Une baguette!",
	"Gargamel plays it!",
	"Rita is the new top dog!",
	"SWM forever!",
	"Representing Edsbyn!",
	"Matt Damon!",
	"Supercalifragilisticexpialidocious!",
	"Consummate V's!",
	"Cow Tools!",
	"Double buffered!",
	"V-synched!",
	"Fan fiction!",
	"Flaxkikare!",
	"Jason! Jason! Jason!",
	"Hotter than the sun!",
	"Internet enabled!",
	"Autonomous!",
	"Engage!",
	"Fantasy!",
	"DRR! DRR! DRR!",
	"Kick it root down!",
	"Regional resources!",
	"Woo, facepunch!",
	"Woo, somethingawful!",
	"Woo, /v/!",
	"Woo, tigsource!",
	"Woo, minecraftforum!",
	"Woo, worldofminecraft!",
	"Woo, reddit!",
	"Woo, 2pp!",
	"Not Google anlyticsed!",
	"Give us Gordon!",
	"Tip your waiter!",
	"Very fun!",
	"12345 is a bad password!",
	"Vote for net neutrality!",
	"Lives in a pineapple under the sea!",
	"Omnipotent!",
	"Gasp!",
	"...!",
	"Bees, bees, bees, bees!",
	"Haha, LOL!",
	"Hampsterdance!",
	"Switches and ores!",
	"Menger sponge!",
	"idspispopd!",
	"Eple (original edit)!",
	"So fresh, so clean!",
	"Don't look directly at the bugs!",
	"Oh, ok, Pigmen!",
	"Scary!",
	"Play Minecraft, Watch Topgear, Get Pig!",
	"Twittered about!",
	"Jump up, jump up, and get down!",
	"Joel is neat!",
	"A riddle, wrapped in a mystery!",
	"Huge tracts of land!",
	"Welcome to your Doom!",
	"Stay a while, stay forever!",
	"Stay a while and listen!",
	"Treatment for your rash!",
	"\"Autological\" is!",
	"Information wants to be free!",
	"\"Almost never\" is an interesting concept!",
	"Lots of truthiness!",
	"The creeper is a spy!",
	"It's groundbreaking!",
	"Let our battle's begin!",
	"The sky is the limit!",
	"Jeb has amazing hair!",
	"Casual gaming!",
	"Undefeated!",
	"Kinda like Lemmings!",
	"Follow the train, CJ!",
	"Leveraging synergy!",
	"DungeonQuest is unfair!",
	"110813!",
	"90210!",
	"Tyrion would love it!",
	"Also try VVVVVV!",
	"Also try Super Meat Boy!",
	"Also try Terraria!",
	"Also try Mount And Blade!",
	"Also try Project Zomboid!",
	"Also try World of Goo!",
	"Also try Limbo!",
	"Also try Pixeljunk Shooter!",
	"Also try Braid!",
	"That's super!",
	"Bread is pain!",
	"Read more books!",
	"Khaaaaaaaaan!",
	"Less addictive than TV Tropes!",
	"More addictive than lemonade!",
	"Bigger than a bread box!",
	"Millions of peaches!",
	"Fnord!",
	"This is my true form!",
	"Totally forgot about Dre!",
	"Don't bother with the clones!",
	"Pumpkinhead!",
	"Hobo humping slobo babe!",
	"Endless!",
	"Feature packed!",
	"Boots with the fur!",
	"Stop, hammertime!",
	"Conventional!",
	"Homeomorphic to a 3-sphere!",
	"Doesn't avoid double negatives!",
	"Place ALL the blocks!",
	"Does barrel rolls!",
	"Meeting expectations!",
	"PC gaming since 1873!",
	"Ghoughpteighbteau tchoghs!",
	"Got your nose!",
	"Haley loves Elan!",
	"Afraid of the big, black bat!",
	"Doesn't use the U-word!",
	"Child's play!",
	"See you next Friday or so!",
	"150 bpm for 400000 minutes!",
	"Technologic!",
	"Funk soul brother!",
	"Pumpa kungen!",
	"Helo Cymru!",
	"My life for Aiur!",
	"Lennart lennart = new Lennart();",
	"I see your vocabulary has improved!",
	"Who put it there?",
	"You can't explain that!",
	"if not ok then return end",
	"SOPA means LOSER in Swedish!",
	"Big Pointy Teeth!",
	"Bekarton guards the gate!",
	"Mmmph, mmph!",
	"Don't feed avocados to parrots!",
	"Swords for everyone!",
	"Plz reply to my tweet!",
	".party()!",
	"Take her pillow!",
	"Put that cookie down!",
	"Pretty scary!",
	"I have a suggestion.",
	"Now with extra hugs!",
	"Almost C++03!", // "Almost C++11!",
	"Woah.",
	"HURNERJSGER?",
	"What's up, Doc?",

	// custom:
	"https://github.com/ReMinecraftPE/mcpe",
	"Also try Minecraft!",
	"Also try Noita!"
};

StartMenuScreen::StartMenuScreen() :
	m_startButton  (2,   0, 0, 200, 20, Language::getInstance()->get("menu.singleplayer")),
	m_joinButton   (3,   0, 0, 200, 20, Language::getInstance()->get("menu.multiplayer")),
	m_optionsButton(4,   0, 0, 200, 20, Language::getInstance()->get("menu.options")),
	m_exitButton    (5,   0, 0, 200, 20, Language::getInstance()->get("menu.quit"))
{
	m_pTiles = nullptr;
	m_chosenSplash = -1;

	// note: do it here because we don't want the title to
	// show up differently when you resize
	TitleTile::regenerate();
}

StartMenuScreen::~StartMenuScreen()
{
	SAFE_DELETE(m_pTiles);
}

void StartMenuScreen::_updateLicense()
{
	int licenseID = m_pMinecraft->getLicenseId();
	if (licenseID < 0)
	{
		m_optionsButton.m_bEnabled = false;
		m_startButton.m_bEnabled = false;
		m_joinButton.m_bEnabled = false;
	}
	else if (licenseID <= 1)
	{
		m_optionsButton.m_bEnabled = true;
		m_startButton.m_bEnabled = true;
#ifdef __EMSCRIPTEN__
		m_joinButton.m_bEnabled = false;
#else
		m_joinButton.m_bEnabled = true;
#endif
	}
	else
	{
		m_pMinecraft->setScreen(new InvalidLicenseScreen(licenseID, m_pMinecraft->platform()->hasBuyButtonWhenInvalidLicense()));
	}
}

void StartMenuScreen::buttonClicked(Button* pButton)
{
	if (pButton->m_buttonId == m_startButton.m_buttonId)
	{
#if defined(DEMO)
		m_pMinecraft->selectLevel("_DemoLevel", "_DemoLevel", int(getEpochTimeS()));
#else
		m_pMinecraft->setScreen(new SelectWorldScreen);
#endif
	}
	else if (pButton->m_buttonId == m_joinButton.m_buttonId)
	{
		m_pMinecraft->locateMultiplayer();
		m_pMinecraft->setScreen(new JoinGameScreen);
	}
	else if (pButton->m_buttonId == m_exitButton.m_buttonId)
	{
		m_pMinecraft->quit();
	}
	else if (pButton->m_buttonId == m_optionsButton.m_buttonId)
	{
		m_pMinecraft->setScreen(new OptionsScreen(m_pMinecraft->getOptions()));
	}
}

void StartMenuScreen::init()
{
	int yPos = m_height / 4 + 48;

	m_startButton.m_yPos = yPos;
	m_joinButton.m_yPos = yPos + 24;

	m_optionsButton.m_yPos = yPos + 48;
	m_exitButton.m_yPos = yPos + 72;

	m_startButton.m_xPos = (m_width - m_startButton.m_width) / 2;

	int x1 = m_width - m_joinButton.m_width;

	m_joinButton.m_xPos = x1 / 2;
	m_optionsButton.m_xPos = x1 / 2;
	m_exitButton.m_xPos = x1 / 2;

	// add the buttons to the screen:
	m_buttons.push_back(&m_startButton);
	m_buttons.push_back(&m_joinButton);
	m_buttons.push_back(&m_optionsButton);

	m_buttons.push_back(&m_exitButton);

	for (int i = 0; i < int(m_buttons.size()); i++)
		m_buttonTabList.push_back(m_buttons[i]);

	field_154 = "\xFFMojang AB";
	field_16C = m_width - 1 - m_pFont->width(field_154);

	field_170 = "v0.1.0 alpha"
#ifdef DEMO
		" (Demo)"
#endif
		;

	field_188 = (m_width - m_pFont->width(field_170)) / 2;


	_updateLicense();
}

bool StartMenuScreen::isInGameScreen()
{
	return false;
}

void StartMenuScreen::drawLegacyTitle()
{
	Textures* tx = m_pMinecraft->m_pTextures;

	const int titleYPos = 30;
	short width = 274;
	int leftPos = m_width / 2 - width / 2;
	tx->loadAndBindTexture("title/mclogo.png");
	blit(leftPos, titleYPos, 0, 0, 155, 44, 256, 256);
	blit(leftPos + 155, titleYPos, 0, 45, 155, 44, 256, 256);

}

void StartMenuScreen::render(int a, int b, float c)
{
#ifdef TITLE_CROP_MODE
	fill(0, 0, m_width, m_height, 0xFF00FF00);
#else
	//renderBackground();
	renderMenuBackground(c);
#endif

	//int titleYPos = 4;
	//int titleYPos = 30; // -- MC Java position.


	drawLegacyTitle();

	drawString(m_pFont, field_154, field_16C, m_height - 10, 0x00FFFFFF);

	drawSplash();

	Screen::render(a, b, c);
}

void StartMenuScreen::tick()
{
	Screen::tick();
	_updateLicense();

	if (m_pTiles)
	{
		int Width = int(sizeof gLogoLine1 - 1);
		int Height = int(sizeof gLogoLines / sizeof * gLogoLines);
		for (int i = 0; i < Width * Height; i++)
			m_pTiles[i]->tick();
	}
}

void StartMenuScreen::drawSplash()
{
	glPushMatrix();

	std::string splashText = getSplashString();
	int textWidth = m_pFont->width(splashText);
	//int textHeight = m_pFont->height(splashText);

	glTranslatef(float(m_width) / 2.0f + 90.0f, 70.0f, 0.0f);
	glRotatef(-20.0f, 0.0f, 0.0f, 1.0f);

	float timeMS = float(getTimeMs() % 1000) / 1000.0f;
	float scale = 1.8f - Mth::abs(0.1f * Mth::sin(2.0f * float(M_PI) * timeMS));
	scale = (scale * 100.0f) / (32.0f + textWidth);
	glScalef(scale, scale, scale);

	drawCenteredString(m_pFont, splashText, 0, -8, 0xFFFF00);

	glPopMatrix();
}

std::string StartMenuScreen::getSplashString()
{
	if (m_chosenSplash == -1)
	{
		Random random;
		m_chosenSplash = random.nextInt(int(sizeof(gSplashes) / sizeof(*gSplashes)));
	}

	return std::string(gSplashes[m_chosenSplash]);
}

void StartMenuScreen::onClose()
{
}

Tile* TitleTile::_tiles[3];
Random TitleTile::_random;
bool TitleTile::_firstTimeInit = true;

TitleTile::TitleTile(StartMenuScreen* pScreen, int x, int y)
{
	height = float(10 + y) + 32.0f * pScreen->m_random.nextFloat() + float(y);
	lastHeight = height;
	dropVel = 0;
}

void TitleTile::tick()
{
	lastHeight = height;

	if (height > 0.0f)
		dropVel -= 0.6f;
	
	height += dropVel;
	dropVel *= 0.9f;

	if (height < 0.0f)
	{
		height = 0.0f;
		dropVel = 0.0f;
	}
}

Tile* TitleTile::getTileFromChar(char c)
{
	switch (c)
	{
		case '?': return _tiles[1];
		case '$': return _tiles[2];
		default:  return _tiles[0];
	}
}

// NOTE: Using the tile enum instead of Tile::tileName->id, may want to.. not?
static const int _tileBlockList[] = {
	TILE_BOOKSHELF,
	TILE_STAIRS_WOOD,
	TILE_STAIRS_STONE,
	TILE_TOPSNOW,
	TILE_GRASS,
	TILE_INFO_UPDATEGAME1,
	TILE_INFO_UPDATEGAME2,
	TILE_STONESLAB_HALF,
};
static const int _tileBlockListSize = sizeof _tileBlockList / sizeof(int);

Tile* TitleTile::getRandomTile(Tile* except1, Tile* except2)
{
	TileID id;
	for (;;)
	{
		id = _random.nextInt(256);
		for (int i = 0; i < _tileBlockListSize; i++) {
			if (_tileBlockList[i] == id) {
				// N.B. Air does not have a tile
				id = TILE_AIR;
				break;
			}
		}

		if (!Tile::tiles[id])
			continue;

		// If found a tile, check if it can be rendered
		Tile* pTile = Tile::tiles[id];
		if (!TileRenderer::canRender(pTile->getRenderShape()))
			continue;

		if (pTile == except1 || pTile == except2)
			continue;

		return pTile;
	}
}

void TitleTile::regenerate()
{
	if (_firstTimeInit)
	{
		_firstTimeInit = false;
		_random.setSeed(getTimeMs());
	}

	_tiles[0] = getRandomTile(nullptr,   nullptr);
	_tiles[1] = getRandomTile(_tiles[0], nullptr);
	_tiles[2] = getRandomTile(_tiles[0], _tiles[1]);
}
