/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp

	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#include "LevelRenderer.hpp"
#include "client/app/Minecraft.hpp"
#include "renderer/GL/GL.hpp"
#include "common/Mth.hpp"

#include "world/tile/LeafTile.hpp"
#include "world/tile/GrassTile.hpp"
#include "TileEntityRenderDispatcher.hpp"

LevelRenderer::LevelRenderer(Minecraft* pMC, Textures* pTexs)
{
	m_destroyProgress = 0.0f;
	m_noEntityRenderFrames = 2;
	m_totalEntities = 0;
	m_renderedEntities = 0;
	m_culledEntities = 0;
	m_cullStep = 0;
	m_totalChunks = 0;
	m_offscreenChunks = 0;
	m_occludedChunks = 0;
	m_renderedChunks = 0;
	m_emptyChunks = 0;
	m_chunkFixOffs = 0;
	m_resortedMinX = 0;
	m_resortedMinY = 0;
	m_resortedMinZ = 0;
	m_resortedMaxX = 0;
	m_resortedMaxY = 0;
	m_resortedMaxZ = 0;
	m_pLevel = nullptr;
	m_chunks = nullptr;
	m_sortedChunks = nullptr;
	m_chunksLength = 0;
	m_pTileRenderer = nullptr;
	m_xChunks = 0;
	m_yChunks = 0;
	m_zChunks = 0;
	field_B0 = 0;
	m_bOcclusionVisible = false;
	m_lastViewDistance = -1;
	m_ticksSinceStart = 0;
	m_nBuffers = 26136;

	m_pMinecraft = pMC;
	m_pTextures = pTexs;
	m_oPos = Vec3(-9999, -9999, -9999);
	m_pBuffers = new GLuint[m_nBuffers];
	xglGenBuffers(m_nBuffers, m_pBuffers);
	LOG_I("numBuffers: %d", m_nBuffers);

	xglGenBuffers(1, &m_starBuffer);
	generateStars();

	xglGenBuffers(1, &m_skyBuffer);
	xglGenBuffers(1, &m_darkBuffer);
	generateSky(); // inlined in the 0.1.0 demo
}

void LevelRenderer::generateSky()
{
	int s = 128;
	int d = 256 / s + 2;

	Tesselator& t = Tesselator::instance;
	t.begin();
	m_skyBufferCount = 0;
	float yy = 16.0f;

	for (int xx = -s * d; xx <= s * d; xx += s)
	{
		for (int zz = -s * d; zz <= s * d; zz += s)
		{
			t.vertex(xx + 0, yy, zz + 0);
			t.vertex(xx + s, yy, zz + 0);
			t.vertex(xx + s, yy, zz + s);
			t.vertex(xx + 0, yy, zz + s);

			m_skyBufferCount += 4;
		}
	}

	t.end(m_skyBuffer);

	// This code is almost the same, ugly

	t.begin();
	m_darkBufferCount = 0;
	yy = -16.0f;

	for (int xx = -s * d; xx <= s * d; xx += s)
	{
		for (int zz = -s * d; zz <= s * d; zz += s)
		{
			t.vertex(xx + s, yy, zz + 0);
			t.vertex(xx + 0, yy, zz + 0);
			t.vertex(xx + 0, yy, zz + s);
			t.vertex(xx + s, yy, zz + s);

			m_darkBufferCount += 4;
		}
	}

	t.end(m_darkBuffer);
}

void LevelRenderer::generateStars()
{
	Random random = Random(10842L);
	Tesselator& t = Tesselator::instance;
	t.begin();
	m_starBufferCount = 0;

	for (int i = 0; i < 1500; i++)
	{
		float x = (random.nextFloat() * 2.0f - 1.0f);
		float y = (random.nextFloat() * 2.0f - 1.0f);
		float z = (random.nextFloat() * 2.0f - 1.0f);
		float ss = (0.25f + random.nextFloat() * 0.25f);
		float d = x * x + y * y + z * z;
		if (d < 1.0f && d > 0.01f)
		{
			d = 1.0f / Mth::sqrt(d);
			x *= d;
			y *= d;
			z *= d;
			float xp = x * 100.0f;
			float yp = y * 100.0f;
			float zp = z * 100.0f;
			float yRot = Mth::atan2(x, z);
			float ySin = Mth::sin(yRot);
			float yCos = Mth::cos(yRot);
			float xRot = Mth::atan2(Mth::sqrt(x * x + z * z), y);
			float xSin = Mth::sin(xRot);
			float xCos = Mth::cos(xRot);
			float zRot = random.nextFloat() * M_PI * 2.0f;
			float zSin = Mth::sin(zRot);
			float zCos = Mth::cos(zRot);

			for (int c = 0; c < 4; c++)
			{
				float ___xo = 0.0f;
				float ___yo = ((c & 2) - 1) * ss;
				float ___zo = ((c + 1 & 2) - 1) * ss;
				float __yo = ___yo * zCos - ___zo * zSin;
				float __zo = ___zo * zCos + ___yo * zSin;
				float _yo = __yo * xSin + ___xo * xCos;
				float _xo = ___xo * xSin - __yo * xCos;
				float xo = _xo * ySin - __zo * yCos;
				float zo = __zo * ySin + _xo * yCos;
				t.vertex(xp + xo, yp + _yo, zp + zo);
			}
			m_starBufferCount += 4;
		}
	}

	t.end(m_starBuffer);
}

void LevelRenderer::deleteChunks()
{
	for (int i = 0; i < m_zChunks; i++)
	{
		for (int j = 0; j < m_yChunks; j++)
		{
			for (int k = 0; k < m_xChunks; k++)
			{
				int index = k + m_xChunks * (j + m_yChunks * i);
				delete m_chunks[index];
			}
		}
	}

	if (m_chunks)
		delete[] m_chunks;
	m_chunks = nullptr;

	if (m_sortedChunks)
		delete[] m_sortedChunks;
	m_sortedChunks = nullptr;
}

void LevelRenderer::cull(Culler* pCuller, float f)
{
	for (int i = 0; i < m_chunksLength; i++)
	{
		Chunk* pChunk = m_chunks[i];
		if (pChunk->isEmpty())
			continue;

		//@TODO: What does the shift do? (x % 4 == 0)?
		if (!pChunk->m_bVisible || !((i + m_cullStep) << 28))
		{
			pChunk->cull(pCuller);
		}
	}

	m_cullStep++;
}

void LevelRenderer::allChanged()
{
	deleteChunks();

	LeafTile* pLeaves = (LeafTile*)Tile::leaves;

	pLeaves->m_bTransparent = m_pMinecraft->getOptions()->m_bFancyGraphics;
	pLeaves->m_TextureFrame = !pLeaves->m_bTransparent + pLeaves->m_oTex;

	TileRenderer::m_bFancyGrass = m_pMinecraft->getOptions()->m_bFancyGrass;
	TileRenderer::m_bBiomeColors = m_pMinecraft->getOptions()->m_bBiomeColors;

	m_lastViewDistance = m_pMinecraft->getOptions()->m_iViewDistance;

	int x1 = 64 << (3 - m_lastViewDistance);
	if (x1 >= 400)
		x1 = 400;

	m_xChunks = x1 / 16 + 1;
	m_zChunks = x1 / 16 + 1;
	m_yChunks = 8;

	m_chunksLength = m_yChunks * m_xChunks * m_zChunks;
	LOG_I("chunksLength: %d", m_chunksLength);
	m_chunks = new Chunk* [m_chunksLength];
	m_sortedChunks = new Chunk* [m_chunksLength];

	m_resortedMinX = 0;
	m_resortedMinY = 0;
	m_resortedMinZ = 0;

	m_dirtyChunks.clear();
	m_renderableTileEntities.clear();

	m_resortedMaxX = m_xChunks;
	m_resortedMaxY = m_zChunks;
	m_resortedMaxZ = m_yChunks;

	int x2 = 0, x3 = 0;

	// These are actually Chunk coordinates that get converted to Tile coordinates
	TilePos cp(0, 0, 0);
	for (cp.x = 0; cp.x < m_xChunks; cp.x++)
	{
		if (m_yChunks <= 0)
			continue;

		for (cp.y = 0; cp.y < m_yChunks; cp.y++)
		{
			for (cp.z = 0; cp.z < m_zChunks; cp.z++)
			{
				int index = cp.x + m_xChunks * (cp.y + m_yChunks * cp.z);

				Chunk* pChunk = new Chunk(m_pLevel, m_renderableTileEntities, cp * 16, 16, x3 + field_B0, &m_pBuffers[x3]);

				if (m_bOcclusionVisible)
					pChunk->field_50 = 0;

				pChunk->field_4E = false;
				pChunk->m_bOcclusionVisible = true;
				pChunk->m_bVisible = true;
				pChunk->field_48 = x2++;
				pChunk->setDirty();
				m_chunks[index] = pChunk;
				m_sortedChunks[index] = pChunk;

				x3 += 3;
				m_dirtyChunks.push_back(pChunk);
			}
		}
	}

	if (m_pLevel)
	{
		auto& pMob = m_pMinecraft->m_pMobPersp;
		if (pMob)
		{
			resortChunks(pMob->m_pos);

			std::sort(&m_sortedChunks[0], &m_sortedChunks[m_chunksLength], DistanceChunkSorter(pMob.get()));
		}

		m_pLevel->viewDistance = x1 / 16 + 1;
	}

	m_noEntityRenderFrames = 2;
}

void LevelRenderer::resortChunks(const TilePos& pos)
{
	TilePos tp(pos - 8);
	m_resortedMinX = 0x7FFFFFFF;
	m_resortedMinY = 0x7FFFFFFF;
	m_resortedMinZ = 0x7FFFFFFF;
	m_resortedMaxX = 0x80000000;
	m_resortedMaxY = 0x80000000;
	m_resortedMaxZ = 0x80000000;

	int blkCount = m_xChunks * 16;
	int blkCntHalf = blkCount / 2;

	for (int fx = 0; fx < m_xChunks; fx++)
	{
		int x1 = 16 * fx;
		int x2 = x1 + blkCntHalf - tp.x;
		if (x2 < 0) x2 -= blkCount - 1;
		x2 /= blkCount;
		x1 -= blkCount * x2;

		if (m_resortedMinX > x1)
			m_resortedMinX = x1;
		if (m_resortedMaxX < x1)
			m_resortedMaxX = x1;

		for (int fz = 0; fz < m_zChunks; fz++)
		{
			int z1 = 16 * fz;
			int z2 = z1 + blkCntHalf - tp.z;
			if (z2 < 0) z2 -= blkCount - 1;
			z2 /= blkCount;
			z1 -= blkCount * z2;

			if (m_resortedMinZ > z1)
				m_resortedMinZ = z1;
			if (m_resortedMaxZ < z1)
				m_resortedMaxZ = z1;

			for (int fy = 0; fy < m_yChunks; fy++)
			{
				int y1 = 16 * fy;
				if (m_resortedMinY > y1)
					m_resortedMinY = y1;
				if (m_resortedMaxY < y1)
					m_resortedMaxY = y1;

				Chunk* pChunk = m_chunks[(fz * m_yChunks + fy) * m_xChunks + fx];
				bool wasDirty = pChunk->isDirty();
				pChunk->setPos(TilePos(x1, y1, z1));

				if (!wasDirty && pChunk->isDirty())
					m_dirtyChunks.push_back(pChunk);
			}
		}
	}
}

void LevelRenderer::entityAdded(Entity* pEnt)
{
	if (pEnt->isPlayer())
		// @HUH: Why would you do that?
		EntityRenderDispatcher::getInstance()->onGraphicsReset();
}

std::string LevelRenderer::gatherStats1()
{
	//@NOTE: This data is based on the Java Edition pre-1.8 legend. This may not be accurate, but it's a good guideline.
	//See https://minecraft.fandom.com/wiki/Debug_screen#Pre-1.8_legend
	std::stringstream ss;
	ss  << "C: " << m_renderedChunks << "/" << m_totalChunks // Number of chunk sections rendered over total number of chunks.
		<< ". F: " << m_offscreenChunks // Number of chunk sections loaded outside the viewing distance.
		<< ", O: " << m_occludedChunks // Number of occluded chunk sections.
		<< ", E: " << m_emptyChunks // Number of empty chunk sections.
		<< "\n";

	return ss.str();
}

std::string LevelRenderer::gatherStats2()
{
	std::stringstream ss;
	ss  << "E: " << m_renderedEntities << "/" << m_totalEntities
		<< ". B: " << m_culledEntities
		<< ", I: " << m_totalEntities - m_culledEntities - m_renderedEntities
		<< "\n";

	return ss.str();
}

void LevelRenderer::onGraphicsReset()
{
	xglGenBuffers(m_nBuffers, m_pBuffers);
	allChanged();

	xglGenBuffers(1, &m_starBuffer);
	generateStars();

	xglGenBuffers(1, &m_skyBuffer);
	generateSky(); // inlined in the 0.1.0 demo
}

void LevelRenderer::renderAABBOutline(const AABB& aabb) const
{
	Tesselator& t = Tesselator::instance;

	t.begin(GL_LINE_STRIP);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.draw();
	t.begin(GL_LINE_STRIP);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.draw();
	t.begin(GL_LINES);
	t.vertex(aabb.min.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.min.z);
	t.vertex(aabb.max.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.max.x, aabb.max.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.min.y, aabb.max.z);
	t.vertex(aabb.min.x, aabb.max.y, aabb.max.z);
	t.draw();
}

void LevelRenderer::checkQueryResults(int a, int b)
{
}

void LevelRenderer::renderSameAsLast(int a, float b)
{
	for (auto& list : m_renderLists)
		list.render();
}

int LevelRenderer::renderChunks(int start, int end, int a, float b)
{
	m_pChunks.clear();

	int result = 0;
	for (int i = start; i < end; i++)
	{
		Chunk* pChunk = m_sortedChunks[i];
		if (!a)
		{
			m_totalChunks++;
			if (pChunk->empty[0])
			{
				m_emptyChunks++;
			}
			else if (pChunk->m_bVisible)
			{
				if (!m_bOcclusionVisible || pChunk->m_bOcclusionVisible)
					m_renderedChunks++;
				else
					m_occludedChunks++;
			}
			else
			{
				m_offscreenChunks++;
			}
		}

		if (!pChunk->empty[a] && pChunk->m_bVisible && pChunk->m_bOcclusionVisible && pChunk->getList(a) >= 0)
		{
			result++;
			m_pChunks.push_back(pChunk);
		}
	}

	auto& pMob = m_pMinecraft->m_pMobPersp;

	float fPosX = pMob->m_posPrev.x + (pMob->m_pos.x - pMob->m_posPrev.x) * b;
	float fPosY = pMob->m_posPrev.y + (pMob->m_pos.y - pMob->m_posPrev.y) * b;
	float fPosZ = pMob->m_posPrev.z + (pMob->m_pos.z - pMob->m_posPrev.z) * b;

	int lists = 0;

	for (auto& list : m_renderLists)
		list.clear();

	for (Chunk* pChk : m_pChunks)
	{
		auto& renderChunk = *pChk->getRenderChunk(a);
		int list = -1;
		for (int l = 0; l < lists; ++l) {
			if (m_renderLists[l].isAt(renderChunk)) {
				list = l;
	
			}
		}

		if (list < 0) {
			list = lists++;
			m_renderLists[list].init(fPosX, fPosY, fPosZ);
		}

		m_renderLists[list].addR(renderChunk);
	}

	renderSameAsLast(a, b);
	return result;
}

int LevelRenderer::render(Mob* pMob, int a, float b)
{
	for (int i = 0; i < 10; i++)
	{
		m_chunkFixOffs = (m_chunkFixOffs + 1) % m_chunksLength;
		Chunk* pChunk = m_chunks[m_chunkFixOffs];

		if (!pChunk->m_bDirty)
			continue;

		std::vector<Chunk*>::iterator iter = std::find(m_dirtyChunks.begin(), m_dirtyChunks.end(), pChunk);
		if (iter != m_dirtyChunks.end())
			continue;

		m_dirtyChunks.push_back(pChunk);
	}

	if (m_pMinecraft->getOptions()->m_iViewDistance != m_lastViewDistance)
		allChanged();

	if (!a)
		m_totalChunks = m_offscreenChunks = m_occludedChunks = m_renderedChunks = m_emptyChunks = 0;

	Vec3 mobPos = pMob->m_posPrev + (pMob->m_pos - pMob->m_posPrev) * b;

	float dX = pMob->m_pos.x - m_oPos.x, dY = pMob->m_pos.y - m_oPos.y, dZ = pMob->m_pos.z - m_oPos.z;

	if (dX * dX + dY * dY + dZ * dZ > 16.0f)
	{
		m_oPos.x = pMob->m_pos.x;
		m_oPos.y = pMob->m_pos.y;
		m_oPos.z = pMob->m_pos.z;

		resortChunks(pMob->m_pos);
		std::sort(&m_sortedChunks[0], &m_sortedChunks[m_chunksLength], DistanceChunkSorter(pMob));
	}

	// @NOTE: Field_B8 doesn't appear to be used??
	if (m_bOcclusionVisible && !a && !m_pMinecraft->getOptions()->m_bAnaglyphs)
	{
		int c = 16;
		checkQueryResults(0, c);
		
		// @HUH: why 16?
		for (int i = 0; i < c; i++)
			m_sortedChunks[i]->m_bOcclusionVisible = true;

		int d = renderChunks(0, c, 0, b);

		do
		{
			int cold = c;
			c *= 2;
			if (c > m_chunksLength)
				c = m_chunksLength;

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			glDisable(GL_ALPHA_TEST);
			glDisable(GL_FOG);
			glColorMask(false, false, false, false);
			glDepthMask(false);
			checkQueryResults(cold, c);

			glPushMatrix();

			float lastX = 0.0f, lastY = 0.0f, lastZ = 0.0f;

			for (int i = cold; i < c; i++)
			{
				if (m_chunks[i]->isEmpty())
				{
					m_chunks[i]->m_bVisible = false;
					continue;
				}

				if (!m_chunks[i]->m_bVisible)
				{
					m_chunks[i]->m_bOcclusionVisible = true;
					continue;
				}

				if (!m_chunks[i]->field_4E)
					continue;

				int roughDist = int(Mth::sqrt(m_chunks[i]->distanceToSqr(pMob)) / 128.0f + 1.0f);

				if (m_ticksSinceStart % roughDist != i % roughDist)
					continue;
				
				float fXdiff, fYdiff, fZdiff;
				fXdiff = float(m_chunks[i]->m_pos.x) - mobPos.x - lastX;
				fYdiff = float(m_chunks[i]->m_pos.y) - mobPos.y - lastY;
				fZdiff = float(m_chunks[i]->m_pos.z) - mobPos.z - lastZ;

				if (fXdiff != 0.0f || fYdiff != 0.0f || fZdiff != 0.0f)
				{
					glTranslatef(fXdiff, fYdiff, fZdiff);

					lastX += fXdiff;
					lastY += fYdiff;
					lastZ += fZdiff;
				}

				m_chunks[i]->renderBB();
				m_chunks[i]->field_4E = true;
			}
			glPopMatrix();
	
			glColorMask(true, true, true, true);

			glDepthMask(true);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_ALPHA_TEST);
			glEnable(GL_FOG);

			d += renderChunks(cold, c, a, b);
		}
		while (c < m_chunksLength);

		return d;
	}
	else
	{
		return renderChunks(0, m_chunksLength, a, b);
	}
}

void LevelRenderer::setLevel(Level* level)
{
	if (m_pLevel)
		m_pLevel->removeListener(this);

	m_oPos.x = -9999;
	m_oPos.y = -9999;
	m_oPos.z = -9999;

	EntityRenderDispatcher::getInstance()->setLevel(level);
	EntityRenderDispatcher::getInstance()->setMinecraft(m_pMinecraft);

	m_pLevel = level;

	delete m_pTileRenderer;
	m_pTileRenderer = new TileRenderer(m_pLevel);

	if (level)
	{	
		level->addListener(this);
		allChanged();
	}
}

void LevelRenderer::setDirty(const TilePos& min, const TilePos& max)
{
	int minX = Mth::intFloorDiv(min.x, 16);
	int minY = Mth::intFloorDiv(min.y, 16);
	int minZ = Mth::intFloorDiv(min.z, 16);
	int maxX = Mth::intFloorDiv(max.x, 16);
	int maxY = Mth::intFloorDiv(max.y, 16);
	int maxZ = Mth::intFloorDiv(max.z, 16);

	for (int x = minX; x <= maxX; x++)
	{
		int x1 = x % m_xChunks;
		if (x1 < 0)
			x1 += m_xChunks;

		for (int y = minY; y <= maxY; y++)
		{
			int y1 = y % m_yChunks;
			if (y1 < 0)
				y1 += m_yChunks;

			for (int z = minZ; z <= maxZ; z++)
			{
				int z1 = z % m_zChunks;
				if (z1 < 0)
					z1 += m_zChunks;

				Chunk* pChunk = m_chunks[x1 + m_xChunks * (y1 + m_yChunks * z1)];
				if (pChunk->isDirty())
					continue;

				m_dirtyChunks.push_back(pChunk);
				pChunk->setDirty();
			}
		}
	}
}

void LevelRenderer::setTilesDirty(const TilePos& min, const TilePos& max)
{
	setDirty(min - 1, max + 1);
}

void LevelRenderer::tick()
{
	m_ticksSinceStart++;
}

typedef std::vector<Chunk*> ChunkVector;
typedef ChunkVector::iterator ChunkVectorIterator;

bool LevelRenderer::updateDirtyChunks(Mob* pMob, bool b)
{
	constexpr int C_MAX = 3;
	DirtyChunkSorter dcs(pMob);
	Chunk* pChunks[C_MAX] = { nullptr };
	ChunkVector* pVec = nullptr;

	int nr1 = 0;
	int sz = int(m_dirtyChunks.size());
	for (int i = 0; i < sz; i++)
	{
		Chunk* pChunk = m_dirtyChunks[i];
		if (!b)
		{
			if (pChunk->distanceToSqr(pMob) > 1024.0f)
			{
				int j;
				// find place to insert this chunk within the pChunks array
				for (j = 0; j < C_MAX; j++)
				{
					if (pChunks[j] && !dcs(pChunks[j], pChunk))
						break;
				}
				// insert it
				if (--j <= 0)
					continue;
				
				for (int k = j; --k != 0;) {
					pChunks[k - 1] = pChunks[k];
				}

				pChunks[j] = pChunk;
				continue;
			}
		}
		else if (!pChunk->m_bVisible)
		{
			continue;
		}

		if (!pVec)
			pVec = new ChunkVector;

		nr1++;
		pVec->push_back(pChunk);
		m_dirtyChunks[i] = nullptr;
	}

	if (pVec)
	{
		if (pVec->size() > 1)
			std::sort(pVec->begin(), pVec->end(), dcs);

		for (int i = int(pVec->size()) - 1; i >= 0; i--)
		{
			(*pVec)[i]->rebuild();
			(*pVec)[i]->setClean();
		}

		SAFE_DELETE(pVec);
	}

	int nr2 = 0;
	for (int m = C_MAX - 1; m >= 0; m--)
	{
		if (!pChunks[m])
			continue;

		if (!pChunks[m]->m_bVisible && m != C_MAX - 1)
		{
			pChunks[m] = nullptr;
			pChunks[0] = nullptr;
			break;
		}

		pChunks[m]->rebuild();
		pChunks[m]->setClean();
		nr2++;
	}

	int nr3 = 0;
	int nr4 = 0;
	for (; nr4 < int(m_dirtyChunks.size()); nr4++)
	{
		Chunk* pChunk = m_dirtyChunks[nr4];
		if (!pChunk)
			continue;

		bool flag = false;
		for (int j = 0; j < C_MAX; j++)
		{
			if (pChunk == pChunks[j])
				flag = true;
		}

		if (flag)
			continue;

		if (nr3 != nr4)
			m_dirtyChunks[nr3] = pChunk;

		nr3++;
	}

	if (nr4 > nr3)
		m_dirtyChunks.erase(m_dirtyChunks.begin() + nr3, m_dirtyChunks.end());

	return nr1 + nr2 == sz;
}

void LevelRenderer::renderHit(Player* pPlayer, const HitResult& hr, int i, void* vp, float f)
{
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);


	if (!i && m_destroyProgress > 0.0f)
	{
		glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);

		m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		glPushMatrix();
		Tile* pTile = nullptr;
		TileID tile = m_pLevel->getTile(hr.m_tilePos);
		if (tile > 0)
			pTile = Tile::tiles[tile];
		glDisable(GL_ALPHA_TEST);
		glPolygonOffset(-3.0f, -3.0f);
		glEnable(GL_POLYGON_OFFSET_FILL);

		float px = pPlayer->m_posPrev.x + (pPlayer->m_pos.x - pPlayer->m_posPrev.x) * f;
		float py = pPlayer->m_posPrev.y + (pPlayer->m_pos.y - pPlayer->m_posPrev.y) * f;
		float pz = pPlayer->m_posPrev.z + (pPlayer->m_pos.z - pPlayer->m_posPrev.z) * f;

		Tesselator& t = Tesselator::instance;
		glEnable(GL_ALPHA_TEST);
		t.begin();
		t.offset(-px, -py, -pz);
		t.noColor();
		if (!pTile)
			pTile = Tile::stone;

		m_pTileRenderer->tesselateInWorld(pTile, hr.m_tilePos, 240 + int(m_destroyProgress * 10.0f));

		t.draw();
		t.offset(0, 0, 0);

		glPolygonOffset(0.0f, 0.0f);
		glDisable(GL_POLYGON_OFFSET_FILL);

		glDepthMask(true); //@HUH: What is the reason for this? You never disable the depth mask.
		glPopMatrix();
	}

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

void LevelRenderer::renderHitSelect(Player* pPlayer, const HitResult& hr, int i, void* vp, float f)
{
	if (i) return;

	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
	glEnable(GL_DEPTH_TEST);

	m_pMinecraft->m_pTextures->loadAndBindTexture(C_TERRAIN_NAME);

	Tile* pTile = nullptr;
	TileID tileID = m_pLevel->getTile(hr.m_tilePos);
	if (tileID > 0)
		pTile = Tile::tiles[tileID];

	glDisable(GL_ALPHA_TEST);
	glColor4f(0.65f, 0.65f, 0.65f, 0.65f);
	glPushMatrix();
	glPolygonOffset(-0.3f, -0.3f);
	glEnable(GL_POLYGON_OFFSET_FILL);

	float px = pPlayer->m_posPrev.x + (pPlayer->m_pos.x - pPlayer->m_posPrev.x) * f;
	float py = pPlayer->m_posPrev.y + (pPlayer->m_pos.y - pPlayer->m_posPrev.y) * f;
	float pz = pPlayer->m_posPrev.z + (pPlayer->m_pos.z - pPlayer->m_posPrev.z) * f;

	Tesselator& t = Tesselator::instance;
	t.begin();
	t.offset(-px, -py, -pz);
	t.noColor();
	if (!pTile)
		pTile = Tile::stone;

	m_pTileRenderer->tesselateInWorld(pTile, hr.m_tilePos);

	t.draw();
	t.offset(0, 0, 0);

	glPolygonOffset(0.0f, 0.0f);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_TEXTURE_2D);
	glDepthMask(true);
	glPopMatrix();
	glEnable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}

void LevelRenderer::renderHitOutline(Player* pPlayer, const HitResult& hr, int i, void* vp, float f)
{
	if (i != 0 || hr.m_hitType != 0)
		return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0, 0, 0, 0.4f);
	glLineWidth(2.0f);
	glDisable(GL_TEXTURE_2D);
	glDepthMask(false);
	const constexpr real var6 = 0.002;
	Tile* pTile = Tile::tiles[m_pLevel->getTile(hr.m_tilePos)];

	if (pTile) {
		pTile->updateShape(m_pLevel, hr.m_tilePos);
		AABB tileAABB = pTile->getTileAABB(m_pLevel, hr.m_tilePos);
		tileAABB.expand(var6, var6, var6);
		tileAABB.move(-(pPlayer->m_posPrev + (pPlayer->m_pos - pPlayer->m_posPrev) * f));
		this->renderAABBOutline(tileAABB);
	}

	glDepthMask(true);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void LevelRenderer::tileChanged(const TilePos& pos)
{
	setDirty(pos - 1, pos + 1);
}

void LevelRenderer::renderEntities(Vec3 pos, Culler* culler, float f)
{
	if (m_noEntityRenderFrames > 0)
	{
		m_noEntityRenderFrames--;
		return;
	}

	auto& mob = m_pMinecraft->m_pMobPersp;
	
	TileEntityRenderDispatcher::getInstance()->prepare(m_pLevel, m_pMinecraft->m_pTextures, m_pMinecraft->m_pFont, mob, f);
	EntityRenderDispatcher::getInstance()->prepare(m_pLevel, m_pMinecraft->m_pTextures, m_pMinecraft->m_pFont, mob, m_pMinecraft->getOptions(), f);

	m_totalEntities = 0;
	m_renderedEntities = 0;
	m_culledEntities = 0;

	
	TileEntityRenderDispatcher::off = EntityRenderDispatcher::off = mob->m_posPrev + (mob->m_pos - mob->m_posPrev) * f;

	const EntityVector* pVec = m_pLevel->getAllEntities();
	m_totalEntities = int(pVec->size());

	for (int i = 0; i < m_totalEntities; i++)
	{
		auto& pEnt = (*pVec)[i];
		if (!pEnt->shouldRender(pos))
			continue;

		if (!culler->isVisible(pEnt->m_hitbox))
			continue;

		if (m_pMinecraft->m_pMobPersp == pEnt && !m_pMinecraft->getOptions()->m_bThirdPerson)
			continue;

		if (m_pLevel->hasChunkAt(pEnt->m_pos))
		{
			m_renderedEntities++;
			EntityRenderDispatcher::getInstance()->render(pEnt.get(), f);
		}
	}

	for (auto& tileEntity : m_renderableTileEntities)
	{
		TileEntityRenderDispatcher::getInstance()->render(tileEntity.get(), f);
	}
}

extern int t_keepPic;

void LevelRenderer::takePicture(std::shared_ptr<TripodCamera> pCamera, Entity* pOwner)
{
	auto pOldMob = m_pMinecraft->m_pMobPersp;
	bool bOldDontRenderGui = m_pMinecraft->getOptions()->m_bDontRenderGui;
	bool bOldThirdPerson = m_pMinecraft->getOptions()->m_bThirdPerson;

#ifdef ENH_CAMERA_NO_PARTICLES
	extern bool g_bDisableParticles;
	g_bDisableParticles = true;
#endif

	m_pMinecraft->m_pMobPersp = pCamera;
	m_pMinecraft->getOptions()->m_bDontRenderGui = true;
	m_pMinecraft->getOptions()->m_bThirdPerson = false; // really from the perspective of the camera
	m_pMinecraft->m_pGameRenderer->render(0.0f);
	m_pMinecraft->m_pMobPersp = pOldMob;
	m_pMinecraft->getOptions()->m_bDontRenderGui = bOldDontRenderGui;
	m_pMinecraft->getOptions()->m_bThirdPerson = bOldThirdPerson;

#ifdef ENH_CAMERA_NO_PARTICLES
	g_bDisableParticles = false;
#endif

	t_keepPic = -1;

	static char str[256];
	// @HUH: This has the potential to overwrite a file
#ifdef ORIGINAL_CODE
	sprintf(str, "%s/games/com.mojang/img_%.4d.jpg", m_pMinecraft->m_externalStorageDir.c_str(), getTimeMs());
#else
	sprintf(str, "img_%.4d.png", getTimeMs());
#endif

	m_pMinecraft->platform()->saveScreenshot(std::string(str), Minecraft::width, Minecraft::height);
}

void LevelRenderer::addParticle(const std::string& name, const Vec3& pos, const Vec3& dir)
{
	// TODO: Who's the genius who decided it'd be better to check a name string rather than an enum?
	float maxDist = 256.0f;
	if (name == "explodeColor")
		maxDist = 16384.0f;

	if (m_pMinecraft->m_pMobPersp->distanceToSqr_inline(pos) > maxDist)
		return;

	ParticleEngine* pe = m_pMinecraft->m_pParticleEngine;
	if (name == "bubble")
	{
		pe->add(new BubbleParticle(m_pLevel, pos, dir));
		return;
	}
	if (name == "smoke")
	{
		pe->add(new SmokeParticle(m_pLevel, pos, dir, 1.0f));
		return;
	}
	if (name == "explode")
	{
		pe->add(new ExplodeParticle(m_pLevel, pos, dir));
		return;
	}
	if (name == "explodeColor")
	{
		ExplodeParticle* pExplPart = new ExplodeParticle(m_pLevel, pos, dir);
		pExplPart->m_bIsUnlit = true;
		pExplPart->m_rCol = Mth::random();
		pExplPart->m_gCol = Mth::random();
		pExplPart->m_bCol = Mth::random();
		pExplPart->scale(3.0f);
		pe->add(pExplPart);
		return;
	}
	if (name == "flame")
	{
		pe->add(new FlameParticle(m_pLevel, pos, dir));
		return;
	}
	if (name == "flame2")
	{
		FlameParticle* pFlamePart = new FlameParticle(m_pLevel, pos, dir);
		pFlamePart->scale(4.0f);
		pe->add(pFlamePart);
		return;
	}
	if (name == "lava")
	{
		pe->add(new LavaParticle(m_pLevel, pos));
		return;
	}
	if (name == "largesmoke")
	{
		pe->add(new SmokeParticle(m_pLevel, pos, dir, 2.5f));
		return;
	}
	if (name == "reddust")
	{
		pe->add(new RedDustParticle(m_pLevel, pos, dir));
		return;
	}

	LOG_W("Unknown particle type: %s", name.c_str());
}

void LevelRenderer::playSound(const std::string& name, const Vec3& pos, float volume, float pitch)
{
	// TODO: Who's the genius who decided it'd be better to check a name string rather than an enum?
	float mult = 1.0f, maxDist = 16.0f;
	float playerDist = m_pMinecraft->m_pMobPersp->distanceToSqr(pos);

	if (volume > 1.0f)
	{
		mult = 16.0f;
		maxDist = volume * mult;
	}

	if (name == "random.explode")
	{
		volume *= 1.0f - playerDist / 65536.0f;
		if (volume < 0.0f)
			return;
		maxDist = 256.0f;
	}

	if (maxDist * maxDist > playerDist)
		m_pMinecraft->m_pSoundEngine->play(name, pos, volume, pitch);
}

void LevelRenderer::renderSky(float alpha)
{
	if (m_pMinecraft->m_pLevel->m_pDimension->m_bFoggy)
		return;

	glDisable(GL_TEXTURE_2D);

	Vec3f sc = m_pLevel->getSkyColor(m_pMinecraft->m_pMobPersp.get(), alpha);
	if (m_pMinecraft->getOptions()->m_bAnaglyphs)
	{
		sc.x = (((sc.x * 30.0f) + (sc.y * 59.0f)) + (sc.z * 11.0f)) / 100.0f;
		sc.y = ((sc.x * 30.0f) + (sc.y * 70.0f)) / 100.0f;
		sc.z = ((sc.x * 30.0f) + (sc.z * 70.0f)) / 100.0f;
	}

	glColor4f(sc.x, sc.y, Mth::Min(1.0f, sc.z), 1.0f);

	Tesselator& t = Tesselator::instance;

	glDepthMask(false);
	glEnable(GL_FOG);
	glColor4f(sc.x, sc.y, sc.z, 1.0f);

	drawArrayVT(m_skyBuffer, m_skyBufferCount);

	glDisable(GL_FOG);
	glDisable(GL_ALPHA_TEST);

	// Sunrise
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float* c = m_pLevel->m_pDimension->getSunriseColor(m_pLevel->getTimeOfDay(alpha), alpha);
	if (c != nullptr)
	{
		glDisable(GL_TEXTURE_2D);
		glShadeModel(GL_SMOOTH); // I'd rather fuck up the sunrise gradient than AO, but it doesn't even look like it does that

		glPushMatrix();
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(m_pLevel->getTimeOfDay(alpha) > 0.5f ? 180 : 0, 0.0f, 0.0f, 1.0f);

		t.begin(GL_TRIANGLE_FAN);
		t.color(c[0], c[1], c[2], c[3]);
		t.vertex(0.0f, 100.0f, 0.0f);
		t.color(c[0], c[1], c[2], 0.0f);

		int steps = 16;
		for (int i = 0; i <= steps; i++)
		{
			float a = i * M_PI * 2.0f / steps;
			float sin = Mth::sin(a);
			float cos = Mth::cos(a);
			t.vertex((sin * 120.0f), (cos * 120.0f), (-cos * 40.0f * c[3]));
		}

		t.draw();
		glPopMatrix();
		glShadeModel(GL_FLAT); // fuck it, don't bother, doing this SOMEHOW breaks AO
	}

	// Sun, moon, stars
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_ONE, GL_ONE);
	glPushMatrix();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTranslatef(sc.x, sc.y, sc.z);
	glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(m_pLevel->getTimeOfDay(alpha) * 360.0f, 1.0f, 0.0f, 0.0f);

	float ss = 30.0f;

	
	m_pTextures->loadAndBindTexture("terrain/sun.png");
	t.begin();
	t.vertexUV(-ss, 100.0f, -ss, 0.0f, 0.0f);
	t.vertexUV(ss, 100.0f, -ss, 1.0f, 0.0f);
	t.vertexUV(ss, 100.0f, ss, 1.0f, 1.0f);
	t.vertexUV(-ss, 100.0f, ss, 0.0f, 1.0f);
	t.draw();

	ss = 20.0f;
	m_pTextures->loadAndBindTexture("terrain/moon.png");
	t.begin();
	t.vertexUV(-ss, -100.0f, ss, 1.0f, 1.0f);
	t.vertexUV(ss, -100.0f, ss, 0.0f, 1.0f);
	t.vertexUV(ss, -100.0f, -ss, 0.0f, 0.0f);
	t.vertexUV(-ss, -100.0f, -ss, 1.0f, 0.0f);
	t.draw();
	

	glDisable(GL_TEXTURE_2D);

	float a = m_pLevel->getStarBrightness(alpha);
	if (a > 0.0f)
	{
		glColor4f(a, a, a, a);
		drawArrayVT(m_starBuffer, m_starBufferCount);
	}

	// Dark plane
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_FOG);
	glPopMatrix();

	glColor4f(sc.x * 0.2f + 0.04f, sc.y * 0.2f + 0.04f, sc.z * 0.6f + 0.1f, 1.0f);
	glDisable(GL_TEXTURE_2D);
	drawArrayVT(m_darkBuffer, m_darkBufferCount);
	glEnable(GL_TEXTURE_2D);

	glDepthMask(true);
}

void LevelRenderer::renderClouds(float partialTick)
{
	if (m_pMinecraft->getOptions()->m_bFancyGraphics)
	{
		renderAdvancedClouds(partialTick);
		return;
	}

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	float yPos = Mth::Lerp(m_pMinecraft->m_pMobPersp->m_posPrev.y, m_pMinecraft->m_pMobPersp->m_pos.y, partialTick); // not certain if this old pos Y is used
	m_pTextures->loadAndBindTexture("environment/clouds.png");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Vec3 cloudColor = m_pLevel->getCloudColor(partialTick);

	float offX = Mth::Lerp(m_pMinecraft->m_pMobPersp->m_oPos.x, m_pMinecraft->m_pMobPersp->m_pos.x, partialTick) + (float(m_ticksSinceStart) + partialTick) * 0.03f;
	float offZ = Mth::Lerp(m_pMinecraft->m_pMobPersp->m_oPos.z, m_pMinecraft->m_pMobPersp->m_pos.z, partialTick);
	
	int dx2048 = Mth::floor(offX / 2048.0f);
	int dz2048 = Mth::floor(offZ / 2048.0f);

	offX -= float(dx2048 * 2048);
	offZ -= float(dz2048 * 2048);

	Tesselator& t = Tesselator::instance;

	float fYPos = ((float)C_MAX_Y - yPos) + 0.33f;
	offX /= 2048.0f;
	offZ /= 2048.0f;
	t.begin();
	t.color(cloudColor.x, cloudColor.y, cloudColor.z, 0.8f);

	const int incr = 16 * 2;
	const int maxX = 256;
	const int maxZ = 256;
	for (int x = -maxX; x < maxX; x += incr)
	{
		for (int z = -maxZ; z < maxZ; z += incr)
		{
			t.vertexUV(float(x) + 0.0f, fYPos, float(z) + incr, float(x + 0.0f) / 2048.0f + offX, float(z + incr) / 2048.0f + offZ);
			t.vertexUV(float(x) + incr, fYPos, float(z) + incr, float(x + incr) / 2048.0f + offX, float(z + incr) / 2048.0f + offZ);
			t.vertexUV(float(x) + incr, fYPos, float(z) + 0.0f, float(x + incr) / 2048.0f + offX, float(z + 0.0f) / 2048.0f + offZ);
			t.vertexUV(float(x) + 0.0f, fYPos, float(z) + 0.0f, float(x + 0.0f) / 2048.0f + offX, float(z + 0.0f) / 2048.0f + offZ);
		}

	}

	t.voidBeginAndEndCalls(false); // why??
	t.draw();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

void LevelRenderer::renderAdvancedClouds(float partialTick)
{
	glDisable(GL_CULL_FACE);

	float yOffs = m_pMinecraft->m_pMobPersp->m_posPrev.y + (m_pMinecraft->m_pMobPersp->m_pos.y - m_pMinecraft->m_pMobPersp->m_posPrev.y) * partialTick;

	Tesselator& t = Tesselator::instance;
	constexpr float ss = 12.0f;
	constexpr float h = 4.0f;

	// @NOTE: Using Mth::Lerp will use incorrect logic
	float xo = (m_pMinecraft->m_pMobPersp->m_oPos.x + (m_pMinecraft->m_pMobPersp->m_pos.x - m_pMinecraft->m_pMobPersp->m_oPos.x) * partialTick + ((float(m_ticksSinceStart) + partialTick) * 0.03f)) / ss;
	float zo = (m_pMinecraft->m_pMobPersp->m_oPos.z + (m_pMinecraft->m_pMobPersp->m_pos.z - m_pMinecraft->m_pMobPersp->m_oPos.z) * partialTick) / ss + 0.33f;

	float yy = ((float)m_pLevel->m_pDimension->getCloudHeight() - yOffs) + 0.33f;

	int xOffs = Mth::floor(xo / 2048);
	int zOffs = Mth::floor(zo / 2048);

	xo -= xOffs * 2048;
	zo -= zOffs * 2048;

	m_pTextures->loadAndBindTexture("environment/clouds.png");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Vec3 cc = m_pLevel->getCloudColor(partialTick);
	float cr = cc.x;
	float cg = cc.y;
	float cb = cc.z;
    float uo;
    float vo;
    float scale;
    
	if (m_pMinecraft->getOptions()->m_bAnaglyphs)
	{
        uo = (cr * 30.0F + cg * 59.0F + cb * 11.0F) / 100.0F;
        vo = (cr * 30.0F + cg * 70.0F) / 100.0F;
        scale = (cr * 30.0F + cb * 70.0F) / 100.0F;
        cr = uo;
        cg = vo;
        cb = scale;
	}
    
    uo = (float)(xo * 0.0);
    vo = (float)(zo * 0.0);

	//uo = xo * 0.0f;
	//vo = zo * 0.0f;
	scale = 1.0f / 256.0f;

	uo = Mth::floor(xo) * scale;
	vo = Mth::floor(zo) * scale;

	float xoffs = xo - Mth::floor(xo);
	float zoffs = zo - Mth::floor(zo);

	constexpr int D = 8;
	constexpr int radius = 3;
	constexpr float e = 1.0f / 1024.0f;

	glScalef(ss, 1.0f, ss);

	for (int pass = 0; pass < 2; pass++)
	{
		if (pass == 0)
			glColorMask(false, false, false, false);
		else
			glColorMask(true, true, true, true);

		for (int xPos = -radius + 1; xPos <= radius; xPos++)
		{
			for (int zPos = -radius + 1; zPos <= radius; zPos++)
			{
				t.begin();
				float xx = xPos * D;
				float zz = zPos * D;
				float xp = xx - xoffs;
				float zp = zz - zoffs;

				if (yy > -h - 1.0f)
				{
					t.color(cr * 0.7f, cg * 0.7f, cb * 0.7f, 0.8f);
					t.vertexUV((xp + 0.0f), (yy + 0.0f), (zp + D), ((xx + 0.0f) * scale + uo), ((zz + D) * scale + vo));
					t.vertexUV((xp + D), (yy + 0.0f), (zp + D), ((xx + D) * scale + uo), ((zz + D) * scale + vo));
					t.vertexUV((xp + D), (yy + 0.0f), (zp + 0.0f), ((xx + D) * scale + uo), ((zz + 0.0f) * scale + vo));
					t.vertexUV((xp + 0.0f), (yy + 0.0f), (zp + 0.0f), ((xx + 0.0f) * scale + uo), ((zz + 0.0f) * scale + vo));
				}

				if (yy <= h + 1.0f)
				{
					t.color(cr, cg, cb, 0.8f);
					t.normal(0.0f, 1.0f, 0.0f);
					t.vertexUV((xp + 0.0f), (yy + h - e), (zp + D), ((xx + 0.0f) * scale + uo), ((zz + D) * scale + vo));
					t.vertexUV((xp + D), (yy + h - e), (zp + D), ((xx + D) * scale + uo), ((zz + D) * scale + vo));
					t.vertexUV((xp + D), (yy + h - e), (zp + 0.0f), ((xx + D) * scale + uo), ((zz + 0.0f) * scale + vo));
					t.vertexUV((xp + 0.0f), (yy + h - e), (zp + 0.0f), ((xx + 0.0f) * scale + uo), ((zz + 0.0f) * scale + vo));
				}

				t.color(cr * 0.9f, cg * 0.9f, cb * 0.9f, 0.8f);

				if (xPos > -1)
				{
					t.normal(-1.0f, 0.0f, 0.0f);

					for (int i = 0; i < D; i++)
					{
						t.vertexUV((xp + i + 0.0f), (yy + 0.0f), (zp + D), ((xx + i + 0.5f) * scale + uo), ((zz + D) * scale + vo));
						t.vertexUV((xp + i + 0.0f), (yy + h), (zp + D), ((xx + i + 0.5f) * scale + uo), ((zz + D) * scale + vo));
						t.vertexUV((xp + i + 0.0f), (yy + h), (zp + 0.0f), ((xx + i + 0.5f) * scale + uo), ((zz + 0.0f) * scale + vo));
						t.vertexUV((xp + i + 0.0f), (yy + 0.0f), (zp + 0.0f), ((xx + i + 0.5f) * scale + uo), ((zz + 0.0f) * scale + vo));
					}
				}

				if (xPos <= 1)
				{
					t.normal(1.0f, 0.0f, 0.0f);

					for (int i = 0; i < D; i++)
					{
						t.vertexUV((xp + i + 1.0f - e), (yy + 0.0f), (zp + D), ((xx + i + 0.5f) * scale + uo), ((zz + D) * scale + vo));
						t.vertexUV((xp + i + 1.0f - e), (yy + h), (zp + D), ((xx + i + 0.5f) * scale + uo), ((zz + D) * scale + vo));
						t.vertexUV((xp + i + 1.0f - e), (yy + h), (zp + 0.0f), ((xx + i + 0.5f) * scale + uo), ((zz + 0.0f) * scale + vo));
						t.vertexUV((xp + i + 1.0f - e), (yy + 0.0f), (zp + 0.0f), ((xx + i + 0.5f) * scale + uo), ((zz + 0.0f) * scale + vo));
					}
				}

				t.color(cr * 0.8f, cg * 0.8f, cb * 0.8f, 0.8f);
				if (zPos > -1)
				{
					t.normal(0.0f, 0.0f, -1.0f);

					for (int i = 0; i < D; i++)
					{
						t.vertexUV((xp + 0.0f), (yy + h), (zp + i + 0.0f), ((xx + 0.0f) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + D), (yy + h), (zp + i + 0.0f), ((xx + D) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + D), (yy + 0.0f), (zp + i + 0.0f), ((xx + D) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + 0.0f), (yy + 0.0f), (zp + i + 0.0f), ((xx + 0.0f) * scale + uo), ((zz + i + 0.5f) * scale + vo));
					}
				}

				if (zPos <= 1)
				{
					t.normal(0.0f, 0.0f, 1.0f);

					for (int i = 0; i < D; i++)
					{
						t.vertexUV((xp + 0.0f), (yy + h), (zp + i + 1.0f - e), ((xx + 0.0f) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + D), (yy + h), (zp + i + 1.0f - e), ((xx + D) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + D), (yy + 0.0f), (zp + i + 1.0f - e), ((xx + D) * scale + uo), ((zz + i + 0.5f) * scale + vo));
						t.vertexUV((xp + 0.0f), (yy + 0.0f), (zp + i + 1.0f - e), ((xx + 0.0f) * scale + uo), ((zz + i + 0.5f) * scale + vo));
					}
				}

				t.draw();
			}
		}
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

void LevelRenderer::skyColorChanged()
{
	for (int i = 0; i < m_chunksLength; i++)
	{
		Chunk* pChunk = m_chunks[i];
		if (!pChunk->m_bSkyLit)
			continue;

		if (pChunk->isDirty())
			continue;

		m_dirtyChunks.push_back(pChunk);
		pChunk->setDirty();
	}
}
