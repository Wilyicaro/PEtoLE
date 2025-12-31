#include "MapItemSavedData.hpp"
#include <common/Mth.hpp>
#include "world/entity/Player.hpp"

MapItemSavedData::MapItemSavedData(const std::string& id) : SavedData(id), m_dimension(0), m_xCenter(0), m_zCenter(0), m_scale(0), m_colors{}, m_ticks(0)
{
}

void MapItemSavedData::save(CompoundIO tag) const
{
	tag->putByte("dimension", m_dimension);
	tag->putInt("xCenter", m_xCenter);
	tag->putInt("zCenter", m_zCenter);
	tag->putByte("scale", m_scale);
	tag->putShort("width", 128);
	tag->putShort("height", 128);
	tag->putByteArray("colors", m_colors, 16384);
}

void MapItemSavedData::load(CompoundIO tag)
{
	m_dimension = tag->getByte("dimension");
	m_xCenter = tag->getInt("xCenter");
	m_zCenter = tag->getInt("zCenter");
	m_scale = Mth::clamp(tag->getByte("scale"), 0, 4);

	short var2 = tag->getShort("width");
	short var3 = tag->getShort("height");
	if (var2 == 128 && var3 == 128)
	{
		memcpy(m_colors, tag->getByteArray("colors").data(), 16384);
	}
	else
	{
		auto colors = tag->getByteArray("colors");
		std::fill(m_colors, m_colors + 16384, 0);
		int var5 = (128 - var2) / 2;
		int var6 = (128 - var3) / 2;

		for (int var7 = 0; var7 < var3; ++var7)
		{
			int var8 = var7 + var6;
			if (var8 >= 0 && var8 < 128)
			{
				for (int var9 = 0; var9 < var2; ++var9)
				{
					int var10 = var9 + var5;
					if (var10 >= 0 && var10 < 128)
					{
						m_colors[var10 + var8 * 128] = colors[var9 + var7 * var2];
					}
				}
			}
		}
	}
}

void MapItemSavedData::tickCarriedBy(const std::shared_ptr<Player>& player, const std::shared_ptr<ItemInstance>& item)
{
	if (m_carriedByPlayers.find(player) == m_carriedByPlayers.end())
	{
		HoldingPlayer* p = new HoldingPlayer(this, player);
		m_carriedByPlayers[player] = p;
		m_carriedBy.push_back(p);
	}

	m_icons.clear();

	for (auto it = m_carriedBy.begin(); it != m_carriedBy.end(); )
	{
		HoldingPlayer* holding = *it;
		if (!holding->m_pPlayer->m_bRemoved && holding->m_pPlayer->m_pInventory->contains(item))
		{
			float var5 = (holding->m_pPlayer->m_pos.x - m_xCenter) / float(1 << m_scale);
			float var6 = (holding->m_pPlayer->m_pos.z - m_zCenter) / float(1 << m_scale);
			uint8_t var7 = 64;
			uint8_t var8 = 64;
			if (var5 >= -var7 && var6 >= -var8 && var5 <= var7 && var6 <= var8) {
				uint8_t image = 0;
				int8_t x = (var5 * 2.0F) + 0.5;
				int8_t y = (var6 * 2.0F) + 0.5;
				uint8_t rot = (holding->m_pPlayer->m_rot.y * 16.0F / 360.0F) + 0.5;
				if (m_dimension < 0)
				{
					int var13 = m_ticks / 10;
					rot = (var13 * var13 * 34187121 + var13 * 121 >> 15 & 15);
				}

				if (holding->m_pPlayer->m_dimension == m_dimension)
					m_icons.push_back({ image, x, y, rot });
			}
			it++;
		}
		else
		{
			m_carriedByPlayers.erase(holding->m_pPlayer);
			delete holding;
			it = m_carriedBy.erase(it);
		}
	}
}

std::vector<uint8_t> MapItemSavedData::getColors(const std::shared_ptr<ItemInstance>& item, Level*, const std::shared_ptr<Player>& player)
{
	auto it = m_carriedByPlayers.find(player);
	if (it == m_carriedByPlayers.end())
	{
		return std::vector<uint8_t>();
	}
	else
	{
		return it->second->getColors(item);
	}
}

void MapItemSavedData::markDirty(int i, int j, int k)
{
	SavedData::setDirty();

	for (auto holding : m_carriedBy)
	{
		if (holding->m_dirtyX[i] < 0 || holding->m_dirtyX[i] > j)
			holding->m_dirtyX[i] = j;

		if (holding->m_dirtyY[i] < 0 || holding->m_dirtyY[i] < k)
			holding->m_dirtyY[i] = k;
	}
}

void MapItemSavedData::applyToMap(const std::vector<uint8_t>& colors)
{
	if (colors[0] == 0)
	{
		int var2 = colors[1] & 255;
		int var3 = colors[2] & 255;

		for (int var4 = 0; var4 < colors.size() - 3; ++var4)
			m_colors[(var4 + var3) * 128 + var2] = colors[var4 + 3];

		setDirty();
	}
	else if (colors[0] == 1)
	{
		m_icons.clear();

		for (int var2 = 0; var2 < (colors.size() - 1) / 3; ++var2)
			m_icons.emplace_back(colors[var2 * 3 + 1] % 16, colors[var2 * 3 + 2], colors[var2 * 3 + 3], colors[var2 * 3 + 1] / 16);
	}
}

MapItemSavedData::HoldingPlayer::HoldingPlayer(MapItemSavedData* map, std::shared_ptr<Player> player) : m_pMap(map), m_pPlayer(std::move(player)), m_dirtyX{}, m_dirtyY{}
{
	for (int i = 0; i < 128; ++i)
	{
		m_dirtyX[i] = 0;
		m_dirtyY[i] = 127;
	}
}

std::vector<uint8_t> MapItemSavedData::HoldingPlayer::getColors(const std::shared_ptr<ItemInstance>& item)
{
	int var3;
	int var10;
	if (--m_step < 0)
	{
		m_step = 4;
		std::vector<uint8_t> var2(m_pMap->m_icons.size() * 3 + 1);
		var2[0] = 1;

		for (var3 = 0; var3 < m_pMap->m_icons.size(); ++var3)
		{
			MapDecoration& var4 = m_pMap->m_icons[var3];
			var2[var3 * 3 + 1] = (var4.m_image + (var4.m_rot & 15) * 16);
			var2[var3 * 3 + 2] = var4.m_x;
			var2[var3 * 3 + 3] = var4.m_y;
		}

		bool var9 = true;
		if (!m_lastColors.empty() && m_lastColors.size() == var2.size())
		{
			for (var10 = 0; var10 < var2.size(); ++var10)
			{
				if (var2[var10] != m_lastColors[var10])
				{
					var9 = false;
					break;
				}
			}
		}
		else
			var9 = false;

		if (!var9)
			return m_lastColors = var2;
	}

	for (int var8 = 0; var8 < 10; ++var8)
	{
		var3 = m_tick * 11 % 128;
		++m_tick;
		if (m_dirtyX[var3] >= 0)
		{
			var10 = m_dirtyY[var3] - m_dirtyX[var3] + 1;
			int var5 = m_dirtyX[var3];
			std::vector<uint8_t> var6(var10 + 3);
			var6[0] = 0;
			var6[1] = var3;
			var6[2] = var5;

			for (int var7 = 0; var7 < var10; ++var7)
				var6[var7 + 3] = m_pMap->m_colors[(var7 + var5) * 128 + var3];

			m_dirtyY[var3] = -1;
			m_dirtyX[var3] = -1;
			return var6;
		}
	}

	return std::vector<uint8_t>();
}
