#include "ByteArrayTag.hpp"
#include <stdexcept>
#include <istream>
#include <ostream>

ByteArrayTag::ByteArrayTag() {}
ByteArrayTag::ByteArrayTag(const std::vector<int8_t>& data) : value(data) {}

void ByteArrayTag::write(std::ostream& os) const {
    int32_t length = static_cast<int32_t>(value.size());
    for (int i = 3; i >= 0; --i)
        os.put((length >> (i * 8)) & 0xFF);
    
    for (int8_t byte : value)
        os.put(static_cast<char>(byte));
}

void ByteArrayTag::read(std::istream& is) {
    int32_t length = 0;
    for (int i = 0; i < 4; ++i) {
        int byte = is.get();
        if (byte == EOF) throw std::runtime_error("Failed to read ByteArrayTag length");
        length = (length << 8) | static_cast<uint8_t>(byte);
    }

    value.resize(length);
    for (int32_t i = 0; i < length; ++i) {
        int byte = is.get();
        if (byte == EOF) throw std::runtime_error("Failed to read ByteArrayTag data");
        value[i] = static_cast<int8_t>(byte);
    }
}

const std::vector<int8_t>& ByteArrayTag::getValue() const {
    return value;
}

void ByteArrayTag::setValue(const std::vector<int8_t>& data) {
    value = data;
}
