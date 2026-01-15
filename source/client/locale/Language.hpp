#pragma once

#include <unordered_map>
#include <string>
#include "common/Util.hpp"

class Options;

class Language
{
public:
    void init(Options*);
    bool loadLanguageFile(const std::string& path);
    bool contains(const std::string& key) const;
    const std::string& get(const std::string& key) const;
    template<typename... Args>
    std::string get(const std::string& key, Args&&... args) const {
        return Util::format(get(key), args...);
    }

    static Language* getInstance() 
    {
        return instance;
    }

private:
    std::unordered_map<std::string, std::string> m_translations;
    static Language* instance;
};
