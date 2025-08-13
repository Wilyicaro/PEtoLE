#pragma once

#include <memory>
#include <array>
#include <string>
#include "TileEntity.hpp"
#include "world/level/Level.hpp"

class MusicTileEntity : public TileEntity {
public:
    MusicTileEntity();

    void load(std::shared_ptr<CompoundTag> tag) override;
    void save(std::shared_ptr<CompoundTag> tag) override;

    void tune();
    void playNote(Level*, const TilePos&);

public:
    bool m_bOn;
    int m_note;
};
