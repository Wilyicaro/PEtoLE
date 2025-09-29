#include "PistonRenderer.hpp"
#include "Lighting.hpp"
#include "world/tile/PistonHeadTile.hpp"
#include "client/app/Minecraft.hpp"
#include <world/tile/PistonBaseTile.hpp>

void PistonRenderer::renderTyped(PistonMovingTileEntity* piston, const Vec3& pos, float partialTicks)
{
	Tile* tile = Tile::tiles[piston->getTileId()];
	if (tile && piston->getProgress(partialTicks) < 1.0F)
	{
		m_tileRenderer.m_pLevelSource = piston->m_pLevel;
		Tesselator& t = Tesselator::instance;
		bindTexture(C_TERRAIN_NAME);
		Lighting::turnOff();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		if (Minecraft::useAmbientOcclusion) {
			glShadeModel(GL_SMOOTH);
		}
		else {
			glShadeModel(GL_FLAT);
		}

		t.begin();
		Vec3f off = piston->getOff(partialTicks);
		t.offset(pos.x - piston->m_pos.x + off.x, pos.y - piston->m_pos.y + off.y, pos.z - piston->m_pos.z + off.z);
		t.color(1, 1, 1);
		if (tile == Tile::pistonHead && piston->getProgress(partialTicks) < 0.5F)
		{
			m_tileRenderer.tesselateHeadPistonInWorldNoCulling(tile, piston->m_pos, false);
		}
		else if (piston->isSourcePiston() && !piston->isExtending())
		{
			Tile::pistonHead->setHeadTexture(((PistonBaseTile*)tile)->getFaceTexture());
			m_tileRenderer.tesselateHeadPistonInWorldNoCulling(Tile::pistonHead, piston->m_pos, piston->getProgress(partialTicks) < 0.5F);
			Tile::pistonHead->resetHeadTexture();
			t.offset(pos.x - piston->m_pos.x, pos.y - piston->m_pos.y, pos.z - piston->m_pos.z);
			m_tileRenderer.tesselatePistonInWorldNoCulling(tile, piston->m_pos);
		}
		else
			m_tileRenderer.tesselateInWorldNoCulling(tile, piston->m_pos);

		t.offset(0.0, 0.0, 0.0);
		t.draw();
		Lighting::turnOn();
	}
}
