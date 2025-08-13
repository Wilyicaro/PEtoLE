/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "AppPlatform.hpp"
#include "common/Utils.hpp"
#include <fstream>
#include <sstream>
#include "thirdparty/stb_vorbis.c"

AppPlatform* AppPlatform::m_singleton = nullptr;

AppPlatform* const AppPlatform::singleton()
{
	return m_singleton;
}

AppPlatform::AppPlatform()
{
	m_singleton = this;
}

AppPlatform::~AppPlatform()
{

}

void AppPlatform::_tick()
{

}

void AppPlatform::buyGame()
{

}

int AppPlatform::checkLicense()
{
	return 0; // assume no license
}

void AppPlatform::createUserInput()
{

}

void AppPlatform::finish()
{

}

std::string AppPlatform::getDateString(int time)
{
	return "";
}

// ??? AppPlatform::getOptionStrings()
// To whoever is confused about the above, we moved it to Options::getOptionStrings()

int AppPlatform::getScreenWidth() const
{
	return C_DEFAULT_SCREEN_WIDTH; // default rez of the XPERIA PLAY?
}

int AppPlatform::getScreenHeight() const
{
	return C_DEFAULT_SCREEN_HEIGHT;
}

std::vector<std::string> AppPlatform::getUserInput()
{
	return std::vector<std::string>();
}

int AppPlatform::getUserInputStatus()
{
	return 0;
}

bool AppPlatform::hasBuyButtonWhenInvalidLicense()
{
	return false;
}

// void AppPlatform::loadTexture(const std::string&, bool);

void AppPlatform::saveScreenshot(const std::string&, int, int)
{

}

void AppPlatform::showDialog(eDialogType type)
{

}

void AppPlatform::uploadPlatformDependentData(int, void*)
{

}


Texture AppPlatform::loadTexture(const std::string&, bool bIsRequired)
{
	return Texture(0, 0, nullptr, 1, 0);
}

#ifndef ORIGINAL_CODE

bool AppPlatform::doesTextureExist(const std::string& path) const
{
	return false;
}

bool AppPlatform::isTouchscreen() const
{
	return true;
}

bool AppPlatform::hasGamepad() const
{
	return false;
}

void AppPlatform::recenterMouse()
{

}

void AppPlatform::setMouseGrabbed(bool b)
{

}

void AppPlatform::getMouseDiff(int& x, int& y)
{
	x = y = 0;
}

void AppPlatform::clearDiff()
{
	
}

void AppPlatform::updateFocused(bool focused)
{
}

bool AppPlatform::shiftPressed()
{
	return false;
}

void AppPlatform::showKeyboard(int x, int y, int w, int h)
{
	showKeyboard();
}

void AppPlatform::showKeyboard()
{
}

void AppPlatform::showKeyboard(bool bShown)
{
	if (bShown)
		showKeyboard();
	else
		hideKeyboard();
}

void AppPlatform::hideKeyboard()
{
}

void AppPlatform::onHideKeyboard()
{
}

#ifdef USE_NATIVE_ANDROID
int AppPlatform::getKeyboardUpOffset()
{
	return 0;
}
#endif

void AppPlatform::vibrate(int milliSeconds)
{
}

void AppPlatform::_fireLowMemory()
{
	
}

void AppPlatform::_fireAppSuspended()
{
	
}

void AppPlatform::_fireAppResumed()
{
	
}

void AppPlatform::_fireAppFocusLost()
{
	
}

void AppPlatform::_fireAppFocusGained()
{
	
}

void AppPlatform::_fireAppTerminated()
{
	
}

void AppPlatform::swapBuffers()
{
}

bool AppPlatform::hasFileSystemAccess()
{
	return false;
}

std::string AppPlatform::getPatchData()
{
	return "";
}

void AppPlatform::initSoundSystem()
{
}

SoundSystem* const AppPlatform::getSoundSystem() const
{
	return nullptr;
}

#endif

std::string AppPlatform::getAssetPath(const std::string &path) const
{
	std::string realPath = path;
	if (realPath.size() && realPath[0] == '/')
	{
		// trim it off
		realPath = realPath.substr(1);
	}
	realPath = "assets/" + realPath;
	
	return realPath;
}

std::optional<std::string> AppPlatform::readFile(const std::string& path) const
{
	std::ifstream file(getAssetPath(path), std::ios::binary);
	if (!file) return std::nullopt;

	std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return buffer;
}

std::optional<nlohmann::json> AppPlatform::loadJson(const std::string& path) {
	auto raw = readFile(path);
	if (!raw) return std::nullopt;

	try {
		auto json = nlohmann::json::parse(*raw);
		return json;
	}
	catch (...) {
		return std::nullopt;
	}
}

int AppPlatform::loadOgg(const std::string path, int* channels, int* sample_rate, short** output) const
{
	return stb_vorbis_decode_filename(getAssetPath(path).c_str(), channels, sample_rate, output);
}

int AppPlatform::decodeOgg(const unsigned char* mem, int len, int* channels, int* sample_rate, short** output)
{
	return stb_vorbis_decode_memory(mem, len, channels, sample_rate, output);
}
