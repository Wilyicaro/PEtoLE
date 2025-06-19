#pragma once

#include "Tag.hpp"
#include <string>

class StringTag : public Tag {
public:
    StringTag();
    StringTag(const std::string& value);

    void write(std::ostream& os) const override;
    void read(std::istream& is) override;

    TagType getType() const override { return TagType::TAG_String; }

    const std::string& getValue() const;
    void setValue(const std::string& value);

private:
    std::string value;
};
