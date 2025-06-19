#pragma once

#include "Tag.hpp"
#include <vector>

class LongArrayTag : public Tag {
public:
    LongArrayTag();
    LongArrayTag(const std::vector<int64_t>& value);

    TagType getType() const override;

    void write(std::ostream& os) const override;
    void read(std::istream& is) override;

    const std::vector<int64_t>& getValue() const;
    void setValue(const std::vector<int64_t>& value);

private:
    std::vector<int64_t> value;
};
