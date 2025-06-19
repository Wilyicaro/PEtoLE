#include "FloatTag.hpp"
#include <stdexcept>
#include <cstring>
#include <istream>
#include <ostream>

FloatTag::FloatTag() : value(0.0f) {}
FloatTag::FloatTag(float val) : value(val) {}

void FloatTag::write(std::ostream& os) const {
    uint32_t bits;
    std::memcpy(&bits, &value, sizeof(float));
    for (int i = 3; i >= 0; --i)
        os.put(static_cast<char>((bits >> (i * 8)) & 0xFF));
}

void FloatTag::read(std::istream& is) {
    char bytes[4];
    if (!is.read(bytes, 4)) throw std::runtime_error("Failed to read FloatTag");

    uint32_t bits = 0;
    for (int i = 0; i < 4; ++i)
        bits |= static_cast<uint8_t>(bytes[i]) << ((3 - i) * 8);

    std::memcpy(&value, &bits, sizeof(float));
}

float FloatTag::getValue() const {
    return value;
}

void FloatTag::setValue(float val) {
    value = val;
}
