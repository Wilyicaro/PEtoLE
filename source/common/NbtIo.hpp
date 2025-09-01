#pragma once

#include "CompoundTag.hpp"

class NbtIo {
public:
    static std::shared_ptr<CompoundTag> read(const std::string& path);
    static bool write(CompoundIO, FILE*);
    static bool write(CompoundIO, const std::string& path);
};