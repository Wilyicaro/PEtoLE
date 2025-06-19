#include "ShortTag.hpp"
#include <stdexcept>
#include <istream>
#include <ostream>

ShortTag::ShortTag() : value(0) {}
ShortTag::ShortTag(int16_t val) : value(val) {}

void ShortTag::write(std::ostream& os) const {
    os.put(static_cast<char>((value >> 8) & 0xFF));
    os.put(static_cast<char>(value & 0xFF));
}

void ShortTag::read(std::istream& is) {
    char high, low;
    if (!is.get(high) || !is.get(low)) throw std::runtime_error("Failed to read ShortTag");
    value = (static_cast<uint8_t>(high) << 8) | static_cast<uint8_t>(low);
}

int16_t ShortTag::getValue() const {
    return value;
}

void ShortTag::setValue(int16_t val) {
    value = val;
}
