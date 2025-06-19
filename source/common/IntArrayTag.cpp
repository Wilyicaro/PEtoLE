#include "IntArrayTag.hpp"
#include <ostream>
#include <istream>
#include <cstdint>

IntArrayTag::IntArrayTag() : value() {}

IntArrayTag::IntArrayTag(const std::vector<int32_t>& value) : value(value) {}

TagType IntArrayTag::getType() const {
    return TagType::TAG_Int_Array;
}

void IntArrayTag::write(std::ostream& os) const {
    int32_t length = static_cast<int32_t>(value.size());
    os.write(reinterpret_cast<const char*>(&length), sizeof(length));
    os.write(reinterpret_cast<const char*>(value.data()), length * sizeof(int32_t));
}

void IntArrayTag::read(std::istream& is) {
    int32_t length;
    is.read(reinterpret_cast<char*>(&length), sizeof(length));
    value.resize(length);
    is.read(reinterpret_cast<char*>(value.data()), length * sizeof(int32_t));
}

const std::vector<int32_t>& IntArrayTag::getValue() const {
    return value;
}

void IntArrayTag::setValue(const std::vector<int32_t>& value) {
    this->value = value;
}
