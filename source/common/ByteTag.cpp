#include "ByteTag.hpp"
#include <stdexcept>

ByteTag::ByteTag() : value(0) {}
ByteTag::ByteTag(int8_t val) : value(val) {}

void ByteTag::write(std::ostream& os) const {
    os.put(static_cast<char>(value));
}

void ByteTag::read(std::istream& is) {
    char byte;
    if (!is.get(byte)) throw std::runtime_error("Failed to read ByteTag");
    value = static_cast<int8_t>(byte);
}

int8_t ByteTag::getValue() const {
    return value;
}

void ByteTag::setValue(int8_t val) {
    value = val;
}
