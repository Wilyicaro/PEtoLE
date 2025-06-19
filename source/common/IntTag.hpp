#pragma once

#include "Tag.hpp"
#include <cstdint>

class IntTag : public Tag {
public:
    IntTag();
    IntTag(int32_t val);

    void write(std::ostream& os) const override;
    void read(std::istream& is) override;

    int32_t getValue() const;
    void setValue(int32_t val);

    TagType getType() const override { return TagType::TAG_Int; }

private:
    int32_t value;
};
