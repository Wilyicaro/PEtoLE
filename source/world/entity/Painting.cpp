#include "Painting.hpp"
#include "world/level/Level.hpp"

Painting::Painting(Level* level) : Entity(level), m_checkInterval(0), m_paintingType(0), m_dir(0)
{
	m_pType = EntityType::painting;
    m_heightOffset = 0.0f;
    setSize(0.5F, 0.5F);
}

Painting::Painting(Level* level, const TilePos& pos, int face) : Painting(level)
{
    m_tilePos = pos;
    std::vector<int> candidates;

    for (int i = 0; i < PaintingType::types.size(); ++i) {
        m_paintingType = i;
        setDir(face);
        if (survives())
            candidates.push_back(i);
    }

    if (!candidates.empty())
        m_paintingType = candidates[m_random.nextInt(candidates.size())];

    setDir(face);
}

void Painting::setDir(int face)
{
    m_dir = face;
    m_rotPrev.y = m_rot.y = (face * 90);
    float var2 = getPaintingType().m_width;
    float var3 = getPaintingType().m_height;
    float var4 = getPaintingType().m_width;
    if (face != 0 && face != 2)
        var2 = 0.5F;
    else
        var4 = 0.5F;

    var2 /= 32.0F;
    var3 /= 32.0F;
    var4 /= 32.0F;
    Vec3 putPos = m_tilePos.center();
    const float var8 = 0.5625F;
    if (face == 0)
        putPos.z -= var8;

    if (face == 1)
        putPos.x -= var8;

    if (face == 2)
        putPos.z += var8;

    if (face == 3)
        putPos.x += var8;

    if (face == 0)
        putPos.x -= offs(getPaintingType().m_width);

    if (face == 1)
        putPos.z += offs(getPaintingType().m_width);

    if (face == 2)
        putPos.x += offs(getPaintingType().m_width);

    if (face == 3)
        putPos.z -= offs(getPaintingType().m_width);

    putPos.y += offs(getPaintingType().m_height);
    setPos(putPos);
    const float var9 = -0.00625F;
    m_hitbox = AABB((putPos.x - var2 - var9), (putPos.y - var3 - var9), (putPos.z - var4 - var9), (putPos.x + var2 + var9), (putPos.y + var3 + var9), (putPos.z + var4 + var9));
}

void Painting::tick()
{
    if (m_checkInterval++ == 100 && !m_pLevel->m_bIsOnline)
    {
        m_checkInterval = 0;
        if (!survives())
        {
            remove();
            m_pLevel->addEntity(std::make_shared<ItemEntity>(m_pLevel, m_pos, std::make_shared<ItemInstance>(Item::painting)));
        }
    }
}

bool Painting::survives()
{
    if (!m_pLevel->getCubes(this, m_hitbox)->empty())
        return false;
    else
    {
        int var1 = getPaintingType().m_width / 16;
        int var2 = getPaintingType().m_height / 16;
        TilePos tp(m_tilePos);
        if (m_dir == 0)
            tp.x = Mth::floor(m_pos.x - (getPaintingType().m_width / 32.0F));

        if (m_dir == 1)
            tp.z = Mth::floor(m_pos.z - (getPaintingType().m_width / 32.0F));

        if (m_dir == 2)
            tp.x = Mth::floor(m_pos.x - (getPaintingType().m_width / 32.0F));
      
        if (m_dir == 3)
            tp.z = Mth::floor(m_pos.z - (getPaintingType().m_width / 32.0F));

        tp.y = Mth::floor(m_pos.y - (getPaintingType().m_height / 32.0F));

        int dy;
        for (int dxz = 0; dxz < var1; ++dxz) {
            for (dy = 0; dy < var2; ++dy) {
                Material* var8;
                if (m_dir != 0 && m_dir != 2)
                    var8 = m_pLevel->getMaterial(TilePos(m_tilePos.x, tp.y + dy, tp.z + dxz));
                else
                    var8 = m_pLevel->getMaterial(TilePos(tp.x + dxz, tp.y + dy, m_tilePos.z));

                if (!var8->isSolid())
                    return false;
            }
        }

        auto entities = m_pLevel->getEntities(this, m_hitbox);

        for (auto& ent : entities)
        {
            if (ent->getType() == getType())
                return false;
        }

        return true;
    }
}

void Painting::addAdditionalSaveData(CompoundIO tag) 
{
    tag->putByte("Dir", m_dir);
    tag->putString("Motive", getPaintingType().m_name);
    tag->putInt("TileX", m_tilePos.x);
    tag->putInt("TileY", m_tilePos.y);
    tag->putInt("TileZ", m_tilePos.z);
}

void Painting::readAdditionalSaveData(CompoundIO tag) {
    m_dir = tag->getByte("Dir");
    m_tilePos.x = tag->getInt("TileX");
    m_tilePos.y = tag->getInt("TileY");
    m_tilePos.z = tag->getInt("TileZ");

    std::string type = tag->getString("Motive");

    for (int i = 0; i < PaintingType::types.size(); ++i)
        if (PaintingType::types[i].m_name == type)
            m_paintingType = i;

    setDir(m_dir);
}

bool Painting::hurt(Entity*, int)
{
    if (!m_bRemoved && !m_pLevel->m_bIsOnline) {
        remove();
        markHurt();
        m_pLevel->addEntity(std::make_shared<ItemEntity>(m_pLevel, m_pos, std::make_shared<ItemInstance>(Item::painting)));
    }

    return true;
}

float Painting::offs(int s)
{
    return s == 32 || s == 64 ? 0.5f : 0.0f;
}
