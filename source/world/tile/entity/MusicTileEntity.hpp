#pragma once

#include <memory>
#include <array>
#include <string>
#include "TileEntity.hpp"
#include "world/level/Level.hpp"

class MusicTileEntity : public TileEntity {
public:
    MusicTileEntity();

    void load(CompoundIO tag) override;
    void save(CompoundIO tag) override;

    void tune();
    void playNote(Level*, const TilePos&);

public:
    bool m_bOn;
    int m_note;
};
