#include "DoubleTag.hpp"
#include <stdexcept>
#include <cstring>
#include <istream>
#include <ostream>

DoubleTag::DoubleTag() : value(0.0) {}
DoubleTag::DoubleTag(double val) : value(val) {}

void DoubleTag::write(std::ostream& os) const {
    uint64_t bits;
    std::memcpy(&bits, &value, sizeof(double));
    for (int i = 7; i >= 0; --i)
        os.put(static_cast<char>((bits >> (i * 8)) & 0xFF));
}

void DoubleTag::read(std::istream& is) {
    char bytes[8];
    if (!is.read(bytes, 8)) throw std::runtime_error("Failed to read DoubleTag");

    uint64_t bits = 0;
    for (int i = 0; i < 8; ++i)
        bits |= static_cast<uint64_t>(static_cast<uint8_t>(bytes[i])) << ((7 - i) * 8);

    std::memcpy(&value, &bits, sizeof(double));
}

double DoubleTag::getValue() const {
    return value;
}

void DoubleTag::setValue(double val) {
    value = val;
}
