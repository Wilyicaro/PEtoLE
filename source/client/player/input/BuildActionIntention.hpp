/********************************************************************
	ReMinecraftPE
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

class BuildActionIntention
{
public:
	enum BuildActionIntent
	{
		NONE              = 0<<0,    		 //  0 0 0 0 0 0 0  //  

		PLACE             = 1<<0,			 //  0 0 0 0 0 0 1  //  indicates the desire to place a block
		DESTROY			  =	1<<1,			 //  0 0 0 0 0 1 0  //  causes the player to start/continue destroying the block in front of them, used to cause Minecraft to not automatically handle the BuildActionIntention
		ATTACK            = 1<<3,			 //  0 0 0 1 0 0 0  //  causes the player to take a swing in the air
		INTERACT          = 1<<4,			 //  0 0 1 0 0 0 0  //  causes the player to use the item in their hand
		// Custom bits
		PICK              = 1<<5,            //  1 0 0 0 0 0 0  //  causes the player to equip the block they're currently looking at

		TOUCH_TAP               = ATTACK   | PLACE,             // touch screen was tapped and released quickly
		KEY_DESTROY             = ATTACK   | DESTROY,			// the destroy key was pressed
		KEY_USE                 = INTERACT | PLACE,             // the use key was pressed
		TOUCH_HOLD              = INTERACT | DESTROY,			// touch screen being held down
	};

public:
	BuildActionIntention() : m_type(NONE), m_bContinue(false) {}
	BuildActionIntention(BuildActionIntent type, bool canContinue = false) : m_type(type), m_bContinue(canContinue) {}

	bool isAttack() const {
		return (m_type & ATTACK) != NONE;
	}
	
	bool isInteract() const {
		return (m_type & INTERACT) != NONE;
	}

	bool isDestroy() const {
		return (m_type & DESTROY) != NONE;
	}

	bool isPlace() const {
		return (m_type & PLACE) != NONE;
	}

	bool isPick() const {
		return (m_type & PICK) != NONE;
	}

	bool canContinue() const {
		return m_bContinue;
	}

private:
	BuildActionIntent m_type;
	bool m_bContinue;
};

