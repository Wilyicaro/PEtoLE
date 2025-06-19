#pragma once

#include "Tag.hpp"
#include <cstdint>

class FloatTag : public Tag {
public:
    FloatTag();
    FloatTag(float val);

    void write(std::ostream& os) const override;
    void read(std::istream& is) override;

    float getValue() const;
    void setValue(float val);

    TagType getType() const override { return TagType::TAG_Float; }

private:
    float value;
};
