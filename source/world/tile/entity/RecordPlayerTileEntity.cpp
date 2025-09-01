#include "RecordPlayerTileEntity.hpp"

RecordPlayerTileEntity::RecordPlayerTileEntity() : m_record(0)
{
    m_pType = TileEntityType::recordPlayer;
}

void RecordPlayerTileEntity::load(CompoundIO tag)
{
    TileEntity::load(tag);
    tag->putByte("Record", m_record);
}

void RecordPlayerTileEntity::save(CompoundIO tag)
{
    TileEntity::save(tag);
    m_record = Mth::Max(0, tag->getByte("Record"));
}
