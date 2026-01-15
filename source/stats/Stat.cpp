#include "Stat.hpp"
#include "client/locale/Language.hpp"

Stat::Stat(int id, const std::string& name, const StatFormatter& formatter) :
	m_id(id),
	m_name(name),
	m_pFormatter(&formatter),
	m_bRoot(false),
	m_nameFormatter([](const std::string& desc) { return Language::getInstance()->get(desc); })
{
}

IdStat::IdStat(int id, const std::string& name, int typeId, const StatFormatter& formatter) : Stat(id, name, formatter), m_typeId(typeId)
{
}

int IdStat::getTypeId() const
{
	return m_typeId;
}
