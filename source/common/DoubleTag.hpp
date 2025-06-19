#pragma once

#include "Tag.hpp"
#include <cstdint>

class DoubleTag : public Tag {
public:
    DoubleTag();
    DoubleTag(double val);

    void write(std::ostream& os) const override;
    void read(std::istream& is) override;

    double getValue() const;
    void setValue(double val);

    TagType getType() const override { return TagType::TAG_Double; }

private:
    double value;
};
