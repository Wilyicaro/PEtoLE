#include "LongArrayTag.hpp"
#include <ostream>
#include <istream>
#include <cstdint>

LongArrayTag::LongArrayTag() : value() {}

LongArrayTag::LongArrayTag(const std::vector<int64_t>& value) : value(value) {}

TagType LongArrayTag::getType() const {
    return TagType::TAG_Long_Array;
}

void LongArrayTag::write(std::ostream& os) const {
    int32_t length = static_cast<int32_t>(value.size());
    os.write(reinterpret_cast<const char*>(&length), sizeof(length));
    os.write(reinterpret_cast<const char*>(value.data()), length * sizeof(int64_t));
}

void LongArrayTag::read(std::istream& is) {
    int32_t length;
    is.read(reinterpret_cast<char*>(&length), sizeof(length));
    value.resize(length);
    is.read(reinterpret_cast<char*>(value.data()), length * sizeof(int64_t));
}

const std::vector<int64_t>& LongArrayTag::getValue() const {
    return value;
}

void LongArrayTag::setValue(const std::vector<int64_t>& value) {
    this->value = value;
}
