#pragma once

#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include "client/app/AppPlatform.hpp"
#include "common/Util.hpp"

class Language {
public:
    void init();
    bool loadLanguageFile(const std::string& path);
    bool contains(const std::string& key) const;
    const std::string& get(const std::string& key) const;
    template<typename... Args>
    const std::string& get(const std::string& key, Args... args) const {
        return Util::format(get(key), args...);
    }

    static Language* getInstance() 
    {
        return m_singleton;
    }

private:
    std::unordered_map<std::string, std::string> m_translations;
    static Language* m_singleton;
};
