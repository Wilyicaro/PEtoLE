#pragma once

#include "Tag.hpp"
#include <cstdint>

class LongTag : public Tag {
public:
    LongTag();
    LongTag(int64_t val);

    void write(std::ostream& os) const override;
    void read(std::istream& is) override;

    int64_t getValue() const;
    void setValue(int64_t val);

    TagType getType() const override { return TagType::TAG_Long; }

private:
    int64_t value;
};
