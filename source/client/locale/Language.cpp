#include "Language.hpp"
#include "client/options/Options.hpp"
#include "client/app/AppPlatform.hpp"

Language* Language::instance = new Language();

void Language::init(Options* options)
{
    m_translations.clear();
    loadOriginalLanguageFile("lang/en_US.lang");
    loadOriginalLanguageFile("lang/stats_US.lang");

    loadLanguageFile("lang/en_us.json");
    if (options->m_lang.get() != "en_us")
        loadLanguageFile("lang/" + options->m_lang.get() + ".json");
}

bool Language::loadLanguageFile(const std::string& path)
{
    nlohmann::json json;
    AppPlatform::singleton()->loadJson(path, json);

    if (json.empty()) return false;


    for (auto it = json.begin(); it != json.end(); ++it)
    {
        if (it->is_string())
            m_translations[it.key()] = it->get<std::string>();
    }

    return true;
}

bool Language::loadOriginalLanguageFile(const std::string& path)
{
    std::string file = AppPlatform::singleton()->readFile(path);
    if (file.empty()) return false;
    std::istringstream stream = std::istringstream(file);
    std::string line;

    while (std::getline(stream, line))
    {
        if (line.empty())
            continue;

        if (line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        std::istringstream lineStream = std::istringstream(line);
        std::vector<std::string> entry;
        std::string part;
        while (std::getline(lineStream, part, '='))
        {
            entry.push_back(part);
            if (entry.size() >= 2) break;
        }

        if (entry.size() < 2)
            continue;

        m_translations[entry[0]] = entry[1];
    }
    return true;
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
