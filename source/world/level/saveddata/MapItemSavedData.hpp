#pragma once

#include "SavedData.hpp"
#include <world/item/ItemInstance.hpp>
#include <unordered_map>

class Player;

class MapItemSavedData : public SavedData
{
public:
	MapItemSavedData(const std::string&);

	void save(CompoundIO) const override;
	void load(CompoundIO) override;

	void tickCarriedBy(const std::shared_ptr<Player>&, const std::shared_ptr<ItemInstance>&);
	std::vector<uint8_t> getColors(const std::shared_ptr<ItemInstance>&, Level*, const std::shared_ptr<Player>&);
	void markDirty(int i, int j, int k);
	void applyToMap(const std::vector<uint8_t>&);

public:
	struct MapDecoration
	{
		uint8_t m_image, m_rot;
		int8_t m_x, m_y;

		MapDecoration(uint8_t image, int8_t x, int8_t y, uint8_t rot) : m_image(image), m_x(x), m_y(y), m_rot(rot)
		{
		}
	};

	class HoldingPlayer
	{
	public:
		HoldingPlayer(MapItemSavedData*, std::shared_ptr<Player>);

		std::vector<uint8_t> getColors(const std::shared_ptr<ItemInstance>&);

	public:
		MapItemSavedData* m_pMap;
		std::shared_ptr<Player> m_pPlayer;
		int m_dirtyX[128], m_dirtyY[128];
		std::vector<uint8_t> m_lastColors;

	private:
		int m_tick = 0, m_step = 0;
	};

	int m_xCenter, m_zCenter;
	int m_ticks;
	int8_t m_dimension, m_scale;
	uint8_t m_colors[16384];
	std::vector<MapDecoration> m_icons;
	std::vector<HoldingPlayer*> m_carriedBy;
	std::unordered_map<std::shared_ptr<Player>, HoldingPlayer*> m_carriedByPlayers;
};

