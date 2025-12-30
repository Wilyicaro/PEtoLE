#include "FakeLevelChunk.hpp"
#include "world/tile/Tile.hpp"
#include "world/level/Level.hpp"

FakeLevelChunk::FakeLevelChunk(Level* pLevel, TileID* pBlockData, const ChunkPos& pos, ContentType content) : LevelChunk(pLevel, pBlockData, pos)
{
	m_bDontSave = true;
	contentType = content;
}

void FakeLevelChunk::addEntity(std::shared_ptr<Entity>)
{

}

void FakeLevelChunk::removeEntity(std::shared_ptr<Entity>)
{

}

void FakeLevelChunk::removeEntity(std::shared_ptr<Entity>, int vec)
{

}

void FakeLevelChunk::lightLava()
{

}

void FakeLevelChunk::skyBrightnessChanged()
{

}

void FakeLevelChunk::recalcBlockLights()
{
	if (contentType != ContentType::NONE)
		LevelChunk::recalcBlockLights();
}

int FakeLevelChunk::getRawBrightness(const ChunkTilePos& pos, int skySubtract)
{
	return contentType == ContentType::NONE ? 7 : LevelChunk::getRawBrightness(pos, skySubtract);
}


bool FakeLevelChunk::isSkyLit(const ChunkTilePos& pos)
{
	return contentType != ContentType::NONE && LevelChunk::isSkyLit(pos);
}


void FakeLevelChunk::load()
{
	if (!m_bLoaded && contentType != ContentType::NONE) {
		LevelChunk::recalcHeightmap();
		m_bLoaded = true;
	}
}

void FakeLevelChunk::unload()
{

}

void FakeLevelChunk::markUnsaved()
{

}

TileID FakeLevelChunk::getTile(const ChunkTilePos& pos)
{
	if (contentType == ContentType::DATA) return LevelChunk::getTile(pos);
	if (contentType == ContentType::FLAT) return pos.y <= 3 ? (pos.y == 0 ? Tile::bedrock : pos.y == 3 ? Tile::grass : Tile::dirt)->m_ID : TILE_AIR;
	if (contentType == ContentType::NONE || pos.y > m_pLevel->getSeaLevel()) return TILE_AIR;
	return (pos.y == 0 ? Tile::bedrock : pos.y == m_pLevel->getSeaLevel() - 10 ? Tile::gravel : pos.y <= m_pLevel->getSeaLevel() - 10 ? Tile::stone : Tile::calmWater)->m_ID;
}

bool FakeLevelChunk::setTile(const ChunkTilePos& pos, TileID tile)
{
	return true;
}

bool FakeLevelChunk::setTileAndData(const ChunkTilePos& pos, TileID tile, int data)
{
	return true;
}

int FakeLevelChunk::getData(const ChunkTilePos& pos)
{
	return 0;
}

void FakeLevelChunk::setData(const ChunkTilePos& pos, int data)
{

}

void FakeLevelChunk::recalcHeight(const ChunkTilePos& pos)
{

}

void FakeLevelChunk::recalcHeightmap()
{
}

bool FakeLevelChunk::isEmpty()
{
	return contentType == ContentType::NONE;
}