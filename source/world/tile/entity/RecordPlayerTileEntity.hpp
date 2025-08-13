#pragma once

#include <memory>
#include <array>
#include <string>
#include "TileEntity.hpp"
#include "world/level/Level.hpp"

class RecordPlayerTileEntity : public TileEntity {
public:
    RecordPlayerTileEntity();

    void load(std::shared_ptr<CompoundTag> tag) override;
    void save(std::shared_ptr<CompoundTag> tag) override;

public:
    int m_record;
};
