/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#ifdef USE_SDL
#include "../thirdparty/SDL2/SDL2.h"

// because SDL sucks and makes no sense and sets bit 1<<30 for some keycodes for some godamn reason
enum eSDLVirtualKeys
{
	#define CODE(x) SDLVK_ ## x,
	#include "SDLKeyCodes.h"
	#undef  CODE
};

static int getSDLVirtualKeyCode(int key)
{
	switch (key) {
#define CODE(x) case SDLK_ ## x: return SDLVK_ ## x;
#include "compat/SDLKeyCodes.h"
#undef  CODE
	}
	return SDLVK_UNKNOWN;
}

static int getSDLKeyCode(int key)
{
	switch (key) {
#define CODE(x) case SDLVK_ ## x: return SDLK_ ## x;
#include "compat/SDLKeyCodes.h"
#undef  CODE
	}
	return SDLK_UNKNOWN;
}

#endif

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#elif __APPLE__
    // https://i.stack.imgur.com/LD8pT.png
    #define AKEYCODE_FORWARD_DEL 0x75
    #define AKEYCODE_ARROW_LEFT  0x7B
    #define AKEYCODE_ARROW_RIGHT 0x7C
    #define AKEYCODE_DEL         0x33
    #define AKEYCODE_ENTER       0x24
#endif

#ifdef USE_SDL
// See https://www.libsdl.org/release/SDL-1.2.15/docs/html/sdlkey.html
#define AKEYCODE_FORWARD_DEL   SDLVK_DELETE
#define AKEYCODE_ARROW_LEFT    SDLVK_LEFT
#define AKEYCODE_ARROW_RIGHT   SDLVK_RIGHT
#define AKEYCODE_ARROW_DOWN    SDLVK_DOWN
#define AKEYCODE_ARROW_UP      SDLVK_UP
#define AKEYCODE_DEL	       SDLVK_BACKSPACE
#define AKEYCODE_ENTER         SDLVK_RETURN
#define AKEYCODE_A             SDLVK_a
#define AKEYCODE_Z             SDLVK_z
#define AKEYCODE_0             SDLVK_0
#define AKEYCODE_9             SDLVK_9
#define AKEYCODE_SPACE         SDLVK_SPACE
#define AKEYCODE_COMMA         SDLVK_COMMA
#define AKEYCODE_PERIOD        SDLVK_PERIOD
#define AKEYCODE_PLUS          SDLVK_PLUS
#define AKEYCODE_MINUS         SDLVK_MINUS
#define AKEYCODE_SEMICOLON     SDLVK_SEMICOLON
#define AKEYCODE_SLASH         SDLVK_SLASH
#define AKEYCODE_GRAVE         SDLVK_BACKQUOTE
#define AKEYCODE_BACKSLASH     SDLVK_BACKSLASH
#define AKEYCODE_APOSTROPHE    SDLVK_QUOTE
#define AKEYCODE_LEFT_BRACKET  SDLVK_LEFTBRACKET
#define AKEYCODE_RIGHT_BRACKET SDLVK_RIGHTBRACKET
#elif defined(_WIN32)
// See https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
#define AKEYCODE_FORWARD_DEL   VK_DELETE
#define AKEYCODE_ARROW_LEFT    VK_LEFT
#define AKEYCODE_ARROW_RIGHT   VK_RIGHT
#define AKEYCODE_ARROW_DOWN    VK_DOWN
#define AKEYCODE_ARROW_UP	   VK_UP
#define AKEYCODE_DEL	       VK_BACK
#define AKEYCODE_ENTER         VK_RETURN
#define AKEYCODE_A             'A'
#define AKEYCODE_Z             'Z'
#define AKEYCODE_0             '0'
#define AKEYCODE_9             '9'
#define AKEYCODE_SPACE         VK_SPACE
#define AKEYCODE_COMMA         VK_OEM_COMMA
#define AKEYCODE_PERIOD        VK_OEM_PERIOD
#define AKEYCODE_PLUS          VK_OEM_PLUS
#define AKEYCODE_MINUS         VK_OEM_MINUS
#define AKEYCODE_SEMICOLON     VK_OEM_1
#define AKEYCODE_SLASH         VK_OEM_2
#define AKEYCODE_GRAVE         VK_OEM_3
#define AKEYCODE_BACKSLASH     VK_OEM_5
#define AKEYCODE_APOSTROPHE    VK_OEM_7
#define AKEYCODE_LEFT_BRACKET  VK_OEM_4
#define AKEYCODE_RIGHT_BRACKET VK_OEM_6
#endif

#ifdef USE_NATIVE_ANDROID
    #include <android/keycodes.h>

    #define AKEYCODE_ARROW_LEFT  AKEYCODE_DPAD_LEFT
    #define AKEYCODE_ARROW_RIGHT AKEYCODE_DPAD_RIGHT
    #define AKEYCODE_ARROW_UP  AKEYCODE_DPAD_UP
    #define AKEYCODE_ARROW_DOWN AKEYCODE_DPAD_DOWN
#endif
