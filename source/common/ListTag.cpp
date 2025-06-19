// ListTag.cpp
#include "ListTag.hpp"
#include <stdexcept>
#include <iostream>

ListTag::ListTag() : elementType(TagType::TAG_End) {}
ListTag::ListTag(TagType type) : elementType(type) {}

void ListTag::write(std::ostream& os) const {
    os.put(static_cast<uint8_t>(elementType));
    uint32_t length = static_cast<uint32_t>(value.size());
    for (int i = 3; i >= 0; --i)
        os.put((length >> (i * 8)) & 0xFF);

    for (const auto& tag : value)
        tag->write(os);
}

void ListTag::read(std::istream& is) {
    int typeByte = is.get();
    if (typeByte == EOF) throw std::runtime_error("Failed to read ListTag element type");

    elementType = static_cast<TagType>(typeByte);

    uint32_t length = 0;
    for (int i = 0; i < 4; ++i) {
        int byte = is.get();
        if (byte == EOF) throw std::runtime_error("Failed to read ListTag length");
        length = (length << 8) | static_cast<uint8_t>(byte);
    }

    value.clear();
    for (uint32_t i = 0; i < length; ++i) {
        std::shared_ptr<Tag> tag = Tag::create(elementType);
        tag->read(is);
        value.push_back(tag);
    }
}

TagType ListTag::getElementType() const {
    return elementType;
}

void ListTag::add(std::shared_ptr<Tag> tag) {
    if (elementType == TagType::TAG_End) {
        elementType = tag->getType();
    }
    else if (tag->getType() != elementType) {
        throw std::invalid_argument("ListTag element type mismatch");
    }
    value.push_back(tag);
}

const std::vector<std::shared_ptr<Tag>>& ListTag::getValue() const {
    return value;
}
