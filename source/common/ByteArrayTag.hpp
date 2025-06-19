#pragma once

#include "Tag.hpp"
#include <vector>
#include <cstdint>

class ByteArrayTag : public Tag {
public:
    ByteArrayTag();
    ByteArrayTag(const std::vector<int8_t>& data);

    void write(std::ostream& os) const override;
    void read(std::istream& is) override;

    TagType getType() const override { return TagType::TAG_Byte_Array; }

    const std::vector<int8_t>& getValue() const;
    void setValue(const std::vector<int8_t>& data);

private:
    std::vector<int8_t> value;
};
