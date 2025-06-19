#include "StringTag.hpp"
#include <stdexcept>
#include <istream>
#include <ostream>

StringTag::StringTag() {}
StringTag::StringTag(const std::string& value) : value(value) {}

void StringTag::write(std::ostream& os) const {
    uint16_t length = static_cast<uint16_t>(value.size());
    os.put((length >> 8) & 0xFF);
    os.put(length & 0xFF);
    os.write(value.data(), length);
}

void StringTag::read(std::istream& is) {
    uint16_t length = 0;
    for (int i = 0; i < 2; ++i) {
        int byte = is.get();
        if (byte == EOF) throw std::runtime_error("Failed to read StringTag length");
        length = (length << 8) | static_cast<uint8_t>(byte);
    }

    value.resize(length);
    is.read(&value[0], length);
    if (is.gcount() != length) throw std::runtime_error("Failed to read full StringTag value");
}

const std::string& StringTag::getValue() const {
    return value;
}

void StringTag::setValue(const std::string& val) {
    value = val;
}
