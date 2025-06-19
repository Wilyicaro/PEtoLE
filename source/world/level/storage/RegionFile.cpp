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
        // Criação do arquivo
        m_pFile = fopen(m_fileName.c_str(), "w+b");
        if (!m_pFile) return false;

        // Inicializa os cabeçalhos com zeros
        for (int i = 0; i < 1024; ++i) writeIntBE(m_pFile, 0);
        for (int i = 0; i < 1024; ++i) writeIntBE(m_pFile, 0);

        fflush(m_pFile);
    }

    return true;
}

bool RegionFile::readHeaders() {
    if (!firstRead) return false;
    firstRead = false;
    fseek(m_pFile, 0, SEEK_SET);

    sectorCount = 2;
    for (int i = 0; i < 1024; ++i) {
        int offset = offsets[i] = readIntBE(m_pFile);
        if (offset != 0) {
            int start = offset >> 8;
            int count = offset & 0xFF;
            int end = start + count;
            if (end > sectorCount) sectorCount = end;
        }
    }

    for (int i = 0; i < 1024; ++i) {
        timestamps[i] = readIntBE(m_pFile);
    }


    sectorFlags.assign(sectorCount, false);
    sectorFlags[0] = sectorFlags[1] = true;

    // Marca os setores usados
    for (int i = 0; i < 1024; ++i) {
        int offset = offsets[i];
        if (offset != 0) {
            int start = offset >> 8;
            int count = offset & 0xFF;
            for (int j = 0; j < count; ++j)
                if ((start + j) < sectorCount)
                    sectorFlags[start + j] = true;
        }
    }

    return true;
}

void RegionFile::writeHeaders() {
    fseek(m_pFile, 0, SEEK_SET);

    // Escreve os campos de entrada
    for (int i = 0; i < 1024; ++i) writeIntBE(m_pFile, offsets[i]);
    for (int i = 0; i < 1024; ++i) writeIntBE(m_pFile, timestamps[i]);

    fflush(m_pFile);
}

int RegionFile::findFreeSectors(int sectorsNeeded) {
    int start = 0;
    int consecutive = 0;

    // Verificar se os setores consecutivos estão livres
    for (size_t i = 2; i < sectorFlags.size(); ++i) {
        if (!sectorFlags[i]) {
            if (consecutive == 0) start = i;
            ++consecutive;
            if (consecutive >= sectorsNeeded) {
                // Verificar se os setores consecutivos são válidos e não estão sendo usados por outra chunk
                bool overlap = false;
                for (int j = 0; j < sectorsNeeded; ++j) {
                    if (sectorFlags[start + j]) {
                        overlap = true;
                        break;
                    }
                }

                if (!overlap) {
                    // Marcar os setores como ocupados
                    for (int i = 0; i < sectorsNeeded; ++i) {
                        sectorFlags[start + i] = true;
                    }
                    LOG_I("Allocated sectors: start=%d, needed=%d\n", start, sectorsNeeded);
                    return start;
                }
            }
        }
        else {
            consecutive = 0;
        }
    }

    start = sectorFlags.size();
    sectorFlags.resize(sectorFlags.size() + sectorsNeeded, false);
    LOG_I("Extended field_28: new size=%zu, allocated start=%d\n", sectorFlags.size(), start);
    return start;
}

bool RegionFile::readChunk(const ChunkPos& pos, std::vector<uint8_t>& outData) {
    int index = pos.regionIndex();

    readHeaders();

    int entry = offsets[index];

    if (!entry) return false;

    int sectorStart = entry >> 8;
    int sectorCount = entry & 0xFF;
    fseek(m_pFile, sectorStart * SECTOR_BYTES, SEEK_SET);

    int length = readIntBE(m_pFile);
    if (length <= 0 || length > sectorCount * SECTOR_BYTES) return false;

    uint8_t compressionType = fgetc(m_pFile);
    if (compressionType != 2) return false;

    int compressedLen = length - 1;
    std::vector<uint8_t> compressedData(compressedLen);
    fread(compressedData.data(), 1, compressedLen, m_pFile);

    try {
        outData = decompressZlibStream(compressedData.data(), compressedData.size());
    }
    catch (...) {
        return false;
    }

    return true;
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
    int totalLen = 1 + compressedData.size();
    int sectorsNeeded = (totalLen + 4 + SECTOR_BYTES - 1) / SECTOR_BYTES;

    int sectorStart = findFreeSectors(sectorsNeeded);
    for (int i = 0; i < sectorsNeeded; ++i) {
        sectorFlags[sectorStart + i] = true;
    }

    if (sectorStart + sectorsNeeded > sectorCount) sectorCount = sectorStart + sectorsNeeded;

    fseek(m_pFile, sectorStart * SECTOR_BYTES, SEEK_SET);
    writeIntBE(m_pFile, totalLen);
    fputc(2, m_pFile);
    fwrite(compressedData.data(), 1, compressedData.size(), m_pFile);

    int written = 4 + 1 + compressedData.size();
    int pad = sectorsNeeded * SECTOR_BYTES - written;
    if (pad > 0) {
        std::vector<uint8_t> padding(pad, 0);
        fwrite(padding.data(), 1, pad, m_pFile);
    }

    offsets[index] = (sectorStart << 8) | sectorsNeeded;
    timestamps[index] = static_cast<int>(time(nullptr));

    writeHeaders();

    fflush(m_pFile);
    return true;
}



