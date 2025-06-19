#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <map>
#include <iostream>

enum class TagType : uint8_t {
    TAG_End = 0,
    TAG_Byte = 1,
    TAG_Short = 2,
    TAG_Int = 3,
    TAG_Long = 4,
    TAG_Float = 5,
    TAG_Double = 6,
    TAG_Byte_Array = 7,
    TAG_String = 8,
    TAG_List = 9,
    TAG_Compound = 10,
    TAG_Int_Array = 11,
    TAG_Long_Array = 12
};

class Tag {
public:
    std::string name;
    virtual ~Tag() = default;
    virtual TagType getType() const = 0;
    virtual void write(std::ostream& os) const = 0;
    virtual void read(std::istream& is) = 0;
    static void writeNamed(std::ostream& os, const std::string& name, const std::shared_ptr<Tag>& tag);
    static std::shared_ptr<Tag> readNamed(std::istream& is);
    static std::shared_ptr<Tag> create(TagType type);
};