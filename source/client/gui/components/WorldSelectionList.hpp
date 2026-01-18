/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include "ScrolledSelectionList.hpp"

class SelectWorldScreen;

class WorldSelectionList : public ScrolledSelectionList
{
public:
	WorldSelectionList(SelectWorldScreen*);

	int getItemCount() override;
	void selectItem(int, bool) override;
	bool isSelectedItem(int) override;

	void renderBackground(float) override;
	void renderItem(int i, int j, int k, int l, Tesselator& tes) override;

private:
	SelectWorldScreen* m_pParent;
};

