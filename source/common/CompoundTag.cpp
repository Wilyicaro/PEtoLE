#include <stdexcept>
#include "CompoundTag.hpp"

void CompoundTag::put(std::string name, std::shared_ptr<Tag> tag) {
    tag->name = name;
    tags[name] = tag;
}

std::shared_ptr<Tag> CompoundTag::get(const std::string& name) const {
    auto it = tags.find(name);
    if (it != tags.end()) return it->second;
    return nullptr;
}

bool CompoundTag::contains(const std::string& name)
{
    return tags.find(name) != tags.end();
}

void CompoundTag::write(std::ostream& os) const {
    for (const auto& pair : tags) {
        writeNamed(os, pair.first, pair.second);
    }

    uint8_t endType = static_cast<uint8_t>(TagType::TAG_End);
    os.write(reinterpret_cast<const char*>(&endType), sizeof(endType));
}


void CompoundTag::read(std::istream& is) {
    while (true) {
        auto tag = Tag::readNamed(is);
        if (tag->getType() == TagType::TAG_End)
            break;

        if (!tag)
            throw std::runtime_error("Null tag returned in CompoundTag::read");

        tags[tag->name] = tag;
    }
}
