#pragma once

#include "Item.hpp"

class ComplexItem : public Item
{
public:
    ComplexItem(int id) : Item(id)
    {
    }

    bool isComplex() const { return true; }
};