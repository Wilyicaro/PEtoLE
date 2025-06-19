#pragma once

#include "Tag.hpp"
#include <cstdint>

class ByteTag : public Tag {
public:
    ByteTag();
    ByteTag(int8_t val);
    TagType getType() const override { return TagType::TAG_Byte; }
    void write(std::ostream& os) const override;
    void read(std::istream& is) override;

    int8_t getValue() const;
    void setValue(int8_t val);

private:
    int8_t value;
};
