#pragma once

#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include "BitStream.h" // RakNet
#include "world/level/levelgen/chunk/ChunkPos.hpp"

#define SECTOR_BYTES 4096
#define CHUNK_HEADER 5

class RegionFile {
public:
    RegionFile(const ChunkPos& pos, const std::string directory);
    ~RegionFile();

    bool firstRead = true;
    bool open();
    bool readHeaders();
    void close();

    bool readChunk(const ChunkPos& pos, std::vector<uint8_t>& outData);
    void writeOffset(int index, int offset);
    void writeTimestamp(int index, int timestamp);
    void writeAt(int sectorStart, const std::vector<uint8_t>& data);
    bool writeChunk(const ChunkPos& pos, const std::vector<uint8_t>& uncompressedData);

private:
    FILE* m_pFile;
    std::string m_fileName;

    std::vector<int32_t> offsets;
    std::vector<int32_t> timestamps;
    std::vector<uint8_t> sectorFlags;
};
