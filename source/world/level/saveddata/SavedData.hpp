#pragma once

#include <common/CompoundTag.hpp>
#include <string>

class SavedData
{
public:
	SavedData(const std::string&);

	virtual void load(CompoundIO tag) = 0;
	virtual void save(CompoundIO tag) const = 0;

	bool isDirty() const;
	void setDirty(bool = true);

public:
	const std::string m_id;
	bool m_bDirty;
};

