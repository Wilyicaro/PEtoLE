#include "RegionFile.hpp"
#include <ctime>
#include <cassert>
#include <cstring>
#include <common/Logger.hpp>
#include <common/Utils.hpp>


RegionFile::RegionFile(const ChunkPos& pos, const std::string directory) {
    m_pFile = nullptr;

    m_fileName = directory + "/r." + std::to_string(pos.x >> 5) + "." + std::to_string(pos.z >> 5) + ".mcr";

    offsets.resize(1024, 0);
    timestamps.resize(1024, 0);
}

RegionFile::~RegionFile() {
    close();
}

void RegionFile::close() {
    if (m_pFile) {
        fclose(m_pFile);
        m_pFile = nullptr;
    }
}

bool RegionFile::open() {
    close();
    m_pFile = fopen(m_fileName.c_str(), "r+b");

    if (!m_pFile) {
        m_pFile = fopen(m_fileName.c_str(), "w+b");
        if (!m_pFile) return false;
    }

    if (m_pFile)
        readHeaders();

    return true;
}

bool RegionFile::readHeaders() {
    if (!firstRead) return false;
    firstRead = false;

    fseek(m_pFile, 0, SEEK_END);
    long fileLength = ftell(m_pFile);

    if (fileLength < SECTOR_BYTES) {
        fseek(m_pFile, 0, SEEK_SET);

        for (int i = 0; i < 1024; ++i) writeIntBE(m_pFile, 0);
        for (int i = 0; i < 1024; ++i) writeIntBE(m_pFile, 0);

        fflush(m_pFile);
        fileLength = 2 * SECTOR_BYTES;
    }

    if ((fileLength % SECTOR_BYTES) != 0) {
        int padding = SECTOR_BYTES - (fileLength % SECTOR_BYTES);
        std::vector<uint8_t> pad(padding, 0);
        fwrite(pad.data(), 1, pad.size(), m_pFile);
        fflush(m_pFile);
        fileLength += padding;
    }

    int sectorCount = static_cast<int>(fileLength / SECTOR_BYTES);

    sectorFlags.assign(sectorCount, true);
    sectorFlags[0] = false;
    sectorFlags[1] = false;

    fseek(m_pFile, 0, SEEK_SET);

    for (int i = 0; i < 1024; ++i) {
        int offset = readIntBE(m_pFile);
        offsets[i] = offset;

        if (offset != 0) {
            int sectorStart = offset >> 8;
            int sectorNum = offset & 0xFF;

            if (sectorStart + sectorNum <= sectorCount)
                for (int j = 0; j < sectorNum; ++j)
                    sectorFlags[sectorStart + j] = false;
        }
    }

    for (int i = 0; i < 1024; ++i) {
        timestamps[i] = readIntBE(m_pFile);
    }


    return true;
}

bool RegionFile::readChunk(const ChunkPos& pos, std::vector<uint8_t>& outData) {
    int index = pos.regionIndex();

    int offset = offsets[index];

    if (!offset) return false;

    int sector = offset >> 8;
    int count = offset & 0xFF;

    if (sector + count > sectorFlags.size()) return false;

    fseek(m_pFile, sector * SECTOR_BYTES, SEEK_SET);
    int length = readIntBE(m_pFile);

    if (length > SECTOR_BYTES * count || length < 1) return false;

    uint8_t compressionType = fgetc(m_pFile);
    if (compressionType != 2) return false;

    size_t finalLength = length - 1;
    std::vector<uint8_t> compressed(finalLength);
    if (fread(compressed.data(), 1, finalLength, m_pFile) != finalLength) return false;

    try {
        outData = decompressZlibStream(compressed.data(), compressed.size());
    }
    catch (...) {
        return false;
    }

    return true;
}

void RegionFile::writeOffset(int index, int offset) {
    fseek(m_pFile, index * 4, SEEK_SET);
    writeIntBE(m_pFile, offset);
    offsets[index] = offset;
}

void RegionFile::writeTimestamp(int index, int timestamp) {
    fseek(m_pFile, SECTOR_BYTES + index * 4, SEEK_SET);
    writeIntBE(m_pFile, timestamp);
    timestamps[index] = timestamp;
}

void RegionFile::writeAt(int sectorStart, const std::vector<uint8_t>& data) {
    int totalLen = static_cast<int>(data.size()) + 1;

    fseek(m_pFile, sectorStart * SECTOR_BYTES, SEEK_SET);
    writeIntBE(m_pFile, totalLen);
    fputc(2, m_pFile);
    fwrite(data.data(), 1, data.size(), m_pFile);

    int written = 4 + 1 + static_cast<int>(data.size());
    int pad = SECTOR_BYTES * ((totalLen + 4 + SECTOR_BYTES - 1) / SECTOR_BYTES) - written;
    if (pad > 0) {
        std::vector<uint8_t> padding(pad, 0);
        fwrite(padding.data(), 1, pad, m_pFile);
    }
}

bool RegionFile::writeChunk(const ChunkPos& pos, const std::vector<uint8_t>& uncompressedData) {
    std::vector<uint8_t> compressedData;
    try {
        compressedData = compressZlibStream(uncompressedData.data(), uncompressedData.size());
    }
    catch (...) {
        return false;
    }

    int index = pos.regionIndex();
    int offset = offsets[index];
    int oldStart = offset >> 8;
    int oldCount = offset & 0xFF;

    int dataLen = static_cast<int>(compressedData.size());
    int totalLen = dataLen + 1;
    int sectorsNeeded = (totalLen + 4 + SECTOR_BYTES - 1) / SECTOR_BYTES;

    if (sectorsNeeded >= 256) return false;

    if (oldStart != 0 && oldCount == sectorsNeeded) {
        writeAt(oldStart, compressedData);
        writeTimestamp(index, static_cast<int>(getMillis() / 1000));
        return true;
    }

    std::fill(sectorFlags.begin() + oldStart, sectorFlags.begin() + oldStart + oldCount, true);

    int start = -1;
    int count = 0;

    for (size_t i = 2; i < sectorFlags.size(); ++i) {
        if (count != 0) {
            if (sectorFlags[i]) {
                ++count;
            }
            else {
                count = 0;
            }
        }
        else if (sectorFlags[i]) {
            start = static_cast<int>(i);
            count = 1;
        }

        if (count >= sectorsNeeded) {
            break;
        }
    }

    if (count >= sectorsNeeded) {
        for (int i = 0; i < sectorsNeeded; ++i) {
            sectorFlags[start + i] = false;
        }
    }
    else {
        start = static_cast<int>(sectorFlags.size());
        fseek(m_pFile, 0, SEEK_END);
        std::vector<uint8_t> empty(SECTOR_BYTES, 0);
        for (int i = 0; i < sectorsNeeded; ++i) {
            fwrite(empty.data(), 1, SECTOR_BYTES, m_pFile);
            sectorFlags.push_back(false);
        }
    }

    writeAt(start, compressedData);
    writeOffset(index, (start << 8) | sectorsNeeded);
    writeTimestamp(index, static_cast<int>(getMillis() / 1000));
    fflush(m_pFile);
    return true;
}