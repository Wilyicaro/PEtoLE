#include "NbtIo.hpp"
#include "Utils.hpp"
#include <sstream>

std::shared_ptr<CompoundTag> NbtIo::read(const std::string& path)
{
    FILE* pFile = std::fopen(path.c_str(), "rb");
    if (!pFile)
        return nullptr;

    std::fseek(pFile, 0, SEEK_END);
    size_t size = std::ftell(pFile);
    std::fseek(pFile, 0, SEEK_SET);

    std::vector<uint8_t> compressedData(size);
    std::fread(compressedData.data(), 1, size, pFile);
    std::fclose(pFile);

    std::vector<uint8_t> decompressed = decompressZlibStream(compressedData.data(), compressedData.size(), true);
    std::istringstream is(std::string(reinterpret_cast<const char*>(decompressed.data()), decompressed.size()));
    return std::dynamic_pointer_cast<CompoundTag>(Tag::readNamed(is));
}

bool NbtIo::write(CompoundIO tag, FILE* pFile)
{
    std::ostringstream oss(std::ios::binary);
    Tag::writeNamed(oss, "", tag);

    std::string nbtData = oss.str();

    std::vector<uint8_t> compressedData(nbtData.begin(), nbtData.end());
    try {
        compressedData = compressZlibStream(compressedData.data(), compressedData.size(), true);
    }
    catch (...) {
        return false;
    }

    fwrite(compressedData.data(), 1, compressedData.size(), pFile);
    fclose(pFile);
    return true;
}

bool NbtIo::write(CompoundIO tag, const std::string& path)
{
    FILE* pFile = fopen(path.c_str(), "wb");

    return pFile && write(tag, pFile);
}
