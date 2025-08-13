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

    void load(std::shared_ptr<CompoundTag> tag) override;
    void save(std::shared_ptr<CompoundTag> tag) override;
    Packet* getUpdatePacket() override;

public:
    std::array<std::string, 4> m_messages;
    int m_selectedLine;
};
