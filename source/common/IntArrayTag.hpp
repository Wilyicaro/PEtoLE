#pragma once

#include "Tag.hpp"
#include <vector>

class IntArrayTag : public Tag {
public:
    IntArrayTag();
    IntArrayTag(const std::vector<int32_t>& value);

    TagType getType() const override;

    void write(std::ostream& os) const override;
    void read(std::istream& is) override;

    const std::vector<int32_t>& getValue() const;
    void setValue(const std::vector<int32_t>& value);

private:
    std::vector<int32_t> value;
};
