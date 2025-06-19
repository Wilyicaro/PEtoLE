#include "Tag.hpp"
#include "ByteTag.hpp"
#include "ShortTag.hpp"
#include "IntTag.hpp"
#include "LongTag.hpp"
#include "FloatTag.hpp"
#include "DoubleTag.hpp"
#include "ByteArrayTag.hpp"
#include "StringTag.hpp"
#include "ListTag.hpp"
#include "CompoundTag.hpp"
#include "IntArrayTag.hpp"
#include "LongArrayTag.hpp"
#include "EndTag.hpp"
#include "Util.hpp"
#include "Utils.hpp"


void Tag::writeNamed(std::ostream& os, const std::string& name, const std::shared_ptr<Tag>& tag) {
    uint8_t type = static_cast<uint8_t>(tag->getType());
    os.put(type);
    if (tag->getType() != TagType::TAG_End) {
        writeBE16(os, name.size());
        os.write(name.data(), name.size());

        tag->write(os);
    }
}

inline uint16_t ntohs_manual(uint16_t val) {
	return (val << 8) | (val >> 8);
}

std::shared_ptr<Tag> Tag::readNamed(std::istream& is) {
    uint8_t typeByte;
    is.read(reinterpret_cast<char*>(&typeByte), sizeof(typeByte));
    TagType type = static_cast<TagType>(typeByte);

    auto tag = Tag::create(type);
    if (!tag) throw std::runtime_error("Unknown tag type in readNamed");

    if (type == TagType::TAG_End) {
        return tag;
    }

    uint16_t nameLen;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    nameLen = ntohs_manual(nameLen);

    std::string name(nameLen, '\0');
    is.read(&name[0], nameLen);


    tag->read(is);

    tag->name = name;
    return tag;
}

std::shared_ptr<Tag> Tag::create(TagType type) {
    switch (type) {
    case TagType::TAG_End:         return std::make_shared<EndTag>();
    case TagType::TAG_Byte:        return std::make_shared<ByteTag>();
    case TagType::TAG_Short:       return std::make_shared<ShortTag>();
    case TagType::TAG_Int:         return std::make_shared<IntTag>();
    case TagType::TAG_Long:        return std::make_shared<LongTag>();
    case TagType::TAG_Float:       return std::make_shared<FloatTag>();
    case TagType::TAG_Double:      return std::make_shared<DoubleTag>();
    case TagType::TAG_Byte_Array:  return std::make_shared<ByteArrayTag>();
    case TagType::TAG_String:      return std::make_shared<StringTag>();
    case TagType::TAG_List:        return std::make_shared<ListTag>();
    case TagType::TAG_Compound:    return std::make_shared<CompoundTag>();
    case TagType::TAG_Int_Array:   return std::make_shared<IntArrayTag>();
    case TagType::TAG_Long_Array:  return std::make_shared<LongArrayTag>();
    default: return nullptr;
    }
}
