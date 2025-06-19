#include "Language.hpp"

Language* Language::m_singleton = new Language();

void Language::init()
{
    loadLanguageFile("lang/en_us.json");
}

bool Language::loadLanguageFile(const std::string& path)
{
    auto json = AppPlatform::singleton()->loadJson(path);

    if (!json) return false;


    for (auto it = json->begin(); it != json->end(); ++it) {
        if (it->is_string()) {
            m_translations[it.key()] = it->get<std::string>();
        }
    }

	return false;
}

bool Language::contains(const std::string& key) const
{
    return m_translations.find(key) != m_translations.end();
}

const std::string& Language::get(const std::string& key) const
{
    auto it = m_translations.find(key);
    if (it != m_translations.end()) {
        return it->second;
    }
    return key;
}
