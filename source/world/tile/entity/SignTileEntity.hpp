#pragma once

#include <memory>
#include <array>
#include <string>
#include "TileEntity.hpp"
#include "world/level/Level.hpp"

class SignTileEntity : public TileEntity {
public:
    SignTileEntity();
    ~SignTileEntity() = default;

    void load(CompoundIO tag) override;
    void save(CompoundIO tag) override;
    Packet* getUpdatePacket() override;

public:
    std::array<std::string, 4> m_messages;
    int m_selectedLine;
};
