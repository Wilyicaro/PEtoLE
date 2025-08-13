#pragma once

#include "Level.hpp"

class PortalForcer
{
public:
	void force(Level*, Entity*);

	bool findPortal(Level*, Entity*);

	bool createPortal(Level*, Entity*);

private:
	Random m_random;
};

