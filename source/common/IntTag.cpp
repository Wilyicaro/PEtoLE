#include "IntTag.hpp"
#include <stdexcept>
#include <istream>
#include <ostream>

IntTag::IntTag() : value(0) {}
IntTag::IntTag(int32_t val) : value(val) {}

void IntTag::write(std::ostream& os) const {
    os.put(static_cast<char>((value >> 24) & 0xFF));
    os.put(static_cast<char>((value >> 16) & 0xFF));
    os.put(static_cast<char>((value >> 8) & 0xFF));
    os.put(static_cast<char>(value & 0xFF));
}

void IntTag::read(std::istream& is) {
    char bytes[4];
    if (!is.read(bytes, 4)) throw std::runtime_error("Failed to read IntTag");
    value =
        (static_cast<uint8_t>(bytes[0]) << 24) |
        (static_cast<uint8_t>(bytes[1]) << 16) |
        (static_cast<uint8_t>(bytes[2]) << 8)  |
        static_cast<uint8_t>(bytes[3]);
}

int32_t IntTag::getValue() const {
    return value;
}

void IntTag::setValue(int32_t val) {
    value = val;
}
