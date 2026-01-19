/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "TitleScreen.hpp"
#include "OptionsScreen.hpp"
#include "SelectWorldScreen.hpp"
#include "JoinGameScreen.hpp"
#include "client/locale/Language.hpp"

#if defined(_WIN32) || (defined(TARGET_OS_MAC) && TARGET_OS_IPHONE == 0)
#define CAN_QUIT
#endif

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

TitleScreen::TitleScreen() :
	m_startButton  (2,   0, 0, 200, 20, Language::getInstance()->get("menu.singleplayer")),
	m_joinButton   (3,   0, 0, 200, 20, Language::getInstance()->get("menu.multiplayer")),
	m_optionsButton(4,   0, 0, 200, 20, Language::getInstance()->get("menu.options")),
	m_exitButton    (5,   0, 0, 200, 20, Language::getInstance()->get("menu.quit"))
{
	m_chosenSplash = -1;
}

void TitleScreen::buttonClicked(Button* pButton)
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

void TitleScreen::init()
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

	m_mojangText = "\xFFMojang AB";
	m_mojangTextWidth = m_width - 1 - m_pFont->width(m_mojangText);

	m_versionText = "Minecraft " + m_pMinecraft->getVersionString();

	m_versionTextWidth = (m_width - m_pFont->width(m_versionText)) / 2;
}

bool TitleScreen::isInGameScreen()
{
	return false;
}

void TitleScreen::drawTitle()
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	Textures* tx = m_pMinecraft->m_pTextures;
	const int titleYPos = 30;
	short width = 274;
	int leftPos = m_width / 2 - width / 2;
	tx->loadAndBindTexture("title/mclogo.png");
	blit(leftPos, titleYPos, 0, 0, 155, 44, 256, 256);
	blit(leftPos + 155, titleYPos, 0, 45, 155, 44, 256, 256);
}

void TitleScreen::render(int a, int b, float c)
{
#ifdef TITLE_CROP_MODE
	fill(0, 0, m_width, m_height, 0xFF00FF00);
#else
	renderMenuBackground(c);
#endif

	drawTitle();

	drawSplash();

	if (isMenuPanoramaAvailable())
		drawString(m_pFont, m_versionText, 2, m_height - 10, 0xFFFFFF);
	else 
		drawString(m_pFont, m_versionText, 2, 2, 0x505050);

	drawString(m_pFont, m_mojangText, m_mojangTextWidth, m_height - 10, 0x00FFFFFF);

	Screen::render(a, b, c);
}

void TitleScreen::drawSplash()
{
	glPushMatrix();

	std::string splashText = getSplashString();
	int textWidth = m_pFont->width(splashText);

	glTranslatef(float(m_width) / 2.0f + 90.0f, 70.0f, 0.0f);
	glRotatef(-20.0f, 0.0f, 0.0f, 1.0f);

	float timeMS = float(getTimeMs() % 1000) / 1000.0f;
	float scale = 1.8f - Mth::abs(0.1f * Mth::sin(2.0f * float(M_PI) * timeMS));
	scale = (scale * 100.0f) / (32.0f + textWidth);
	glScalef(scale, scale, scale);

	drawCenteredString(m_pFont, splashText, 0, -8, 0xFFFF00);

	glPopMatrix();
}

std::string TitleScreen::getSplashString()
{
	if (m_chosenSplash == -1)
	{
		Random random;
		m_chosenSplash = random.nextInt(int(sizeof(gSplashes) / sizeof(*gSplashes)));
	}

	return std::string(gSplashes[m_chosenSplash]);
}

void TitleScreen::onClose()
{
}