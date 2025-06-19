#pragma once

#include <memory>
#include <world/item/Container.hpp>

class Recipe {
public:
    virtual ~Recipe() = default;

    virtual bool matches(Container* container) = 0;
    virtual std::shared_ptr<ItemInstance> assemble(Container* container) = 0;
    virtual int size() = 0;
    virtual bool isShaped() const
    {
        return false;
    }
};