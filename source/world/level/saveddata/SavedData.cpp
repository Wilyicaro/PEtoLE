#include "SavedData.hpp"

SavedData::SavedData(const std::string& id) : m_id(id), m_bDirty(false)
{
}

bool SavedData::isDirty() const
{
	return m_bDirty;
}

void SavedData::setDirty(bool dirty)
{
	m_bDirty = dirty;
}