#include "LongTag.hpp"
#include <stdexcept>
#include <istream>
#include <ostream>

LongTag::LongTag() : value(0) {}
LongTag::LongTag(int64_t val) : value(val) {}

void LongTag::write(std::ostream& os) const {
    for (int i = 7; i >= 0; --i)
        os.put(static_cast<char>((value >> (i * 8)) & 0xFF));
}

void LongTag::read(std::istream& is) {
    char bytes[8];
    if (!is.read(bytes, 8)) throw std::runtime_error("Failed to read LongTag");

    value = 0;
    for (int i = 0; i < 8; ++i)
        value |= static_cast<int64_t>(static_cast<uint8_t>(bytes[i])) << ((7 - i) * 8);
}

int64_t LongTag::getValue() const {
    return value;
}

void LongTag::setValue(int64_t val) {
    value = val;
}
