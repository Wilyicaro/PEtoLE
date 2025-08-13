#include "TileEntity.hpp"
#include "world/level/Level.hpp"

std::shared_ptr<TileEntity> TileEntity::loadStatic(std::shared_ptr<CompoundTag> tag)
{
    std::string id = tag->contains("id") ? tag->getString("id") : "";
    auto it = TileEntityType::getByName(id);
    if (it) {
        auto tileEntity = it->newTileEntity();
        tileEntity->load(tag);
        return tileEntity;
    } else {
        LOG_I("Skipping TileEntity with id %s", id.c_str());
        return nullptr;
    }
}

void TileEntity::load(std::shared_ptr<CompoundTag> tag)
{
    m_pos.x = tag->getInt("x");
    m_pos.y = tag->getInt("y");
    m_pos.z = tag->getInt("z");
}

void TileEntity::save(std::shared_ptr<CompoundTag> tag)
{
    if (!m_pType)
        throw std::runtime_error("Missing mapping for class: " + std::string(typeid(*this).name()));

    tag->putString("id", getId());
    tag->putInt("x", m_pos.x);
    tag->putInt("y", m_pos.y);
    tag->putInt("z", m_pos.z);
}

int TileEntity::getData() const
{
    return m_pLevel->getData(m_pos);
}

void TileEntity::setData(int data)
{
    m_pLevel->setData(m_pos, data);
}

void TileEntity::setChanged()
{
    if (m_pLevel)
        m_pLevel->tileEntityChanged(m_pos, shared_from_this());
}

real TileEntity::distanceToSqr(const Vec3& vec) const
{
    return m_pos.center().distanceToSqr(vec);
}

Tile* TileEntity::getTile() const
{
    return Tile::tiles[m_pLevel->getTile(m_pos)];
}

std::string TileEntity::getId() const
{
    return m_pType ? m_pType->getName() : "Unknown";
}
