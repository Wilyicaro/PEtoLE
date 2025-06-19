#pragma once

#include "Tag.hpp"
#include <vector>
#include <memory>

class ListTag : public Tag {
public:
    ListTag();
    ListTag(TagType elementType);

    void write(std::ostream& os) const override;
    void read(std::istream& is) override;

    TagType getType() const override { return TagType::TAG_List; }
    TagType getElementType() const;

    void add(std::shared_ptr<Tag> tag);
    const std::vector<std::shared_ptr<Tag>>& getValue() const;

private:
    TagType elementType;
    std::vector<std::shared_ptr<Tag>> value;
};
