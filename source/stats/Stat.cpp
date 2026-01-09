#include "Stat.hpp"

Stat::Stat(int id, const std::string& name, const StatFormatter& formatter) : m_id(id), m_name(name), m_pFormatter(&formatter), m_bRoot(false)
{
}
