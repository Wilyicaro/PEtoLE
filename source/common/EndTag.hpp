#pragma once
#include "Tag.hpp"

class EndTag : public Tag {
public:
    TagType getType() const override { return TagType::TAG_End; }

    void write(std::ostream& os) const override;
    void read(std::istream& is) override;

    EndTag();
};