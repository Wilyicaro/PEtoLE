/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/
#pragma once

#include "Item.hpp"

class CameraItem : public Item
{
public:
	CameraItem(int id);

	std::shared_ptr<ItemInstance> use(std::shared_ptr<ItemInstance> inst, Level* level, Player* player) override;
};
