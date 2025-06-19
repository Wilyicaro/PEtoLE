#pragma once

#include "Tag.hpp"
#include <cstdint>

class ShortTag : public Tag {
public:
    ShortTag();
    ShortTag(int16_t val);

    void write(std::ostream& os) const override;
    void read(std::istream& is) override;

    int16_t getValue() const;
    void setValue(int16_t val);

    TagType getType() const override { return TagType::TAG_Short; }

private:
    int16_t value;
};
