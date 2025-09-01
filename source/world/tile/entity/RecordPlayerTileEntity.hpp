#pragma once

#include <memory>
#include <array>
#include <string>
#include "TileEntity.hpp"
#include "world/level/Level.hpp"

class RecordPlayerTileEntity : public TileEntity {
public:
    RecordPlayerTileEntity();

    void load(CompoundIO tag) override;
    void save(CompoundIO tag) override;

public:
    int m_record;
};
