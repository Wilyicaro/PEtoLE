#include "RecordItem.hpp"
#include "world/entity/Player.hpp"
#include "world/level/Level.hpp"
#include "world/tile/RecordPlayerTile.hpp"

RecordItem::RecordItem(int id, const std::string& record) : Item(id), m_record(record)
{
	m_maxStackSize = 1;
}

bool RecordItem::useOn(ItemInstance* item, Player*, Level* level, const TilePos& pos, Facing::Name face)
{
	if (level->getTile(pos) == Tile::recordPlayer->m_ID && !level->getData(pos))
	{
		if (level->m_bIsOnline)
			return true;
		else
		{
			Tile::recordPlayer->playRecord(level, pos, m_itemID);
			level->levelEvent(nullptr, LevelEvent::SOUND_PLAY_RECORD, pos, m_itemID);
			--item->m_count;
			return true;
		}
	}
	else {
		return false;
	}
}

const std::string& RecordItem::getStreamingMusic()
{
	return m_record;
}
