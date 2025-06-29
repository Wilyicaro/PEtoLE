#pragma once

#include "Tile.hpp"
#include "world/level/Level.hpp"

class RailTile : public Tile
{
public:
	RailTile(int id, int texture, bool isPowered);
	bool mayPlace(const Level*, const TilePos& pos) const override;
	void neighborChanged(Level* level, const TilePos& pos, TileID tile) override;
	bool isSolidRender() const override;
	bool isCubeShaped() const override;
	AABB* getAABB(const Level*, const TilePos& pos) override;
	int getRenderShape() const override;
	int getTexture(Facing::Name face, int data) const override;
	void updateShape(const LevelSource* level, const TilePos& pos) override;
    void setPlacedBy(Level*, const TilePos& pos, Mob*, Facing::Name face) override;
    void onPlace(Level*, const TilePos& pos) override;

    static bool hasRail(Level* level, const TilePos& pos) {
        return isRail(level->getTile(pos));
    }

    static bool isRail(int id) {
        return id == Tile::rail->m_ID || id == Tile::poweredRail->m_ID || id == Tile::detectorRail->m_ID;
    }

    static bool isPowered(Tile* tile) {
        return tile->m_ID == Tile::poweredRail->m_ID;
    }

	bool m_bIsPowered;

private:
    void updateDir(Level* level, const TilePos& pos, bool updateNeighbors);

    class Rail {
    private:
        Level* m_level;
        TilePos pos;
        bool m_powered;
        std::vector<TilePos> m_connections;

    public:
        Rail(Level* level, TilePos pos) : m_level(level), pos(pos) {
            int data = level->getData(pos);
            if (RailTile::isPowered(Tile::tiles[level->getTile(pos)])) {
                m_powered = true;
                data &= -9;
            }
            else {
                m_powered = false;
            }
            updateConnections(data);
        }

        void updateConnections(int data) {
            m_connections.clear();

            switch (data) {
            case 0:
                m_connections.emplace_back(pos.x, pos.y, pos.z - 1);
                m_connections.emplace_back(pos.x, pos.y, pos.z + 1);
                break;
            case 1:
                m_connections.emplace_back(pos.x - 1, pos.y, pos.z);
                m_connections.emplace_back(pos.x + 1, pos.y, pos.z);
                break;
            case 2:
                m_connections.emplace_back(pos.x - 1, pos.y, pos.z);
                m_connections.emplace_back(pos.x + 1, pos.y + 1, pos.z);
                break;
            case 3:
                m_connections.emplace_back(pos.x - 1, pos.y + 1, pos.z);
                m_connections.emplace_back(pos.x + 1, pos.y, pos.z);
                break;
            case 4:
                m_connections.emplace_back(pos.x, pos.y + 1, pos.z - 1);
                m_connections.emplace_back(pos.x, pos.y, pos.z + 1);
                break;
            case 5:
                m_connections.emplace_back(pos.x, pos.y, pos.z - 1);
                m_connections.emplace_back(pos.x, pos.y + 1, pos.z + 1);
                break;
            case 6:
                m_connections.emplace_back(pos.x + 1, pos.y, pos.z);
                m_connections.emplace_back(pos.x, pos.y, pos.z + 1);
                break;
            case 7:
                m_connections.emplace_back(pos.x - 1, pos.y, pos.z);
                m_connections.emplace_back(pos.x, pos.y, pos.z + 1);
                break;
            case 8:
                m_connections.emplace_back(pos.x - 1, pos.y, pos.z);
                m_connections.emplace_back(pos.x, pos.y, pos.z - 1);
                break;
            case 9:
                m_connections.emplace_back(pos.x + 1, pos.y, pos.z);
                m_connections.emplace_back(pos.x, pos.y, pos.z - 1);
                break;
            }
        }

        void removeSoftConnections() {
            for (size_t i = 0; i < m_connections.size(); ++i) {
                Rail* other = getRail(m_connections[i]);
                if (other && other->connectsTo(*this)) {
                    m_connections[i] = other->pos;
                }
                else {
                    m_connections.erase(m_connections.begin() + i);
                    --i;
                }
                SAFE_DELETE(other);
            }
        }

        bool hasRail(const TilePos& p) {
            return RailTile::hasRail(m_level, p) || RailTile::hasRail(m_level, p.above()) || RailTile::hasRail(m_level, p.below());
        }

        Rail* getRail(const TilePos& p) {
            if (RailTile::hasRail(m_level, p)) {
                return new Rail(m_level, p);
            }
            TilePos above = p.above();
            if (RailTile::hasRail(m_level, above)) {
                return new Rail(m_level, above);
            }
            TilePos below = p.below();
            if (RailTile::hasRail(m_level, below)) {
                return new Rail(m_level, below);
            }
            return nullptr;
        }

        bool connectsTo(const Rail& other) {
            for (const auto& conn : m_connections) {
                if (conn.x == other.pos.x && conn.z == other.pos.z) {
                    return true;
                }
            }
            return false;
        }

        bool hasConnection(int x, int y, int z) {
            for (const auto& conn : m_connections) {
                if (conn.x == x && conn.z == z) {
                    return true;
                }
            }
            return false;
        }

        int countPotentialConnections() {
            int count = 0;
            for (Facing::Name face : Facing::HORIZONTAL)
                if (hasRail(pos.relative(face))) ++count;

            return count;
        }

        bool canConnectTo(Rail& other) {
            if (connectsTo(other)) return true;
            if (m_connections.size() == 2) return false;
            if (m_connections.empty()) return true;

            TilePos first = m_connections[0];
            return other.pos.y == pos.y && first.y == pos.y;
        }

        void connectTo(Rail& other) {
            m_connections.push_back(other.pos);

            bool n = hasConnection(pos.x, pos.y, pos.z - 1);
            bool s = hasConnection(pos.x, pos.y, pos.z + 1);
            bool w = hasConnection(pos.x - 1, pos.y, pos.z);
            bool e = hasConnection(pos.x + 1, pos.y, pos.z);

            int newData = -1;

            if ((n || s) && !w && !e) newData = 0;
            if ((w || e) && !n && !s) newData = 1;

            if (!m_powered) 
            {
                if (s && e && !n && !w) newData = 6;
                if (s && w && !n && !e) newData = 7;
                if (n && w && !s && !e) newData = 8;
                if (n && e && !s && !w) newData = 9;
            }

            if (newData == 0) {
                if (RailTile::hasRail(m_level, TilePos(pos.x, pos.y + 1, pos.z - 1))) newData = 4;
                if (RailTile::hasRail(m_level, TilePos(pos.x, pos.y + 1, pos.z + 1))) newData = 5;
            }

            if (newData == 1) {
                if (RailTile::hasRail(m_level, TilePos(pos.x + 1, pos.y + 1, pos.z))) newData = 2;
                if (RailTile::hasRail(m_level, TilePos(pos.x - 1, pos.y + 1, pos.z))) newData = 3;
            }

            if (newData < 0) newData = 0;
            m_level->setData(pos, newData);
        }

        bool hasNeighborRail(const TilePos& p) {
            Rail* neighbor = getRail(p);
            if (!neighbor) return false;
            neighbor->removeSoftConnections();
            bool canConnect = neighbor->canConnectTo(*this);
            delete neighbor;
            return canConnect;
        }

        void place(bool powered, bool checkNeighbors) {
            bool n = hasNeighborRail(pos.north());
            bool s = hasNeighborRail(pos.south());
            bool w = hasNeighborRail(pos.west());
            bool e = hasNeighborRail(pos.east());

            int newData = -1;

            if ((n || s) && !w && !e) newData = 0;
            if ((w || e) && !n && !s) newData = 1;

            if (!m_powered)
            {
                if (s && e && !n && !w) newData = 6;
                if (s && w && !n && !e) newData = 7;
                if (n && w && !s && !e) newData = 8;
                if (n && e && !s && !w) newData = 9;
            }

            if (newData == -1) {
                if (n || s) newData = 0;
                if (w || e) newData = 1;

                if (!m_powered)
                {
                    if (powered) {
                        if (s && e) newData = 6;
                        if (w && s) newData = 7;
                        if (e && n) newData = 9;
                        if (n && w) newData = 8;
                    }
                    else {
                        if (n && w) newData = 8;
                        if (e && n) newData = 9;
                        if (w && s) newData = 7;
                        if (s && e) newData = 6;

                    }
                }
            }

            if (newData == 0) {
                if (RailTile::hasRail(m_level, TilePos(pos.x, pos.y + 1, pos.z - 1))) newData = 4;
                if (RailTile::hasRail(m_level, TilePos(pos.x, pos.y + 1, pos.z + 1))) newData = 5;
            }

            if (newData == 1) {
                if (RailTile::hasRail(m_level, TilePos(pos.x + 1, pos.y + 1, pos.z))) newData = 2;
                if (RailTile::hasRail(m_level, TilePos(pos.x - 1, pos.y + 1, pos.z))) newData = 3;
            }

            if (newData < 0) 
            {
                newData = !isRail(m_level->getTile(pos)) ? 0 : m_level->getData(pos);
            }

            updateConnections(newData);

            if (m_powered) {
                newData = m_level->getData(pos) & 8 | newData;
            }


            if (checkNeighbors || m_level->getData(pos) != newData)
            {
                m_level->setData(pos, newData);
                for (const auto& conn : m_connections) {
                    Rail* neighbor = getRail(conn);
                    if (neighbor) {
                        neighbor->removeSoftConnections();
                        if (neighbor->canConnectTo(*this)) {
                            neighbor->connectTo(*this);
                        }
                        delete neighbor;
                    }
                }
            }
        }
    };

};

