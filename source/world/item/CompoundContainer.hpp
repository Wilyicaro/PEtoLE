#pragma once

#include <memory>
#include <string>
#include "Container.hpp"
#include "ItemInstance.hpp"

class CompoundContainer : public Container {
private:
    std::string m_name;
    Container* m_c1;
    Container* m_c2;

public:
    CompoundContainer(const std::string& name, Container* c1, Container* c2)
        : m_name(name), m_c1(c1), m_c2(c2) {}

    int getContainerSize() override {
        return m_c1->getContainerSize() + m_c2->getContainerSize();
    }

    std::string getName() override {
        return m_name;
    }

    std::shared_ptr<ItemInstance> getItem(int index) override {
        if (index >= m_c1->getContainerSize()) {
            return m_c2->getItem(index - m_c1->getContainerSize());
        } else {
            return m_c1->getItem(index);
        }
    }

    std::shared_ptr<ItemInstance> removeItem(int index, int count) override {
        if (index >= m_c1->getContainerSize()) {
            return m_c2->removeItem(index - m_c1->getContainerSize(), count);
        } else {
            return m_c1->removeItem(index, count);
        }
    }

    void setItem(int index, std::shared_ptr<ItemInstance> item) override {
        if (index >= m_c1->getContainerSize()) {
            m_c2->setItem(index - m_c1->getContainerSize(), item);
        } else {
            m_c1->setItem(index, item);
        }
    }

    int getMaxStackSize() override {
        return m_c1->getMaxStackSize();
    }

    void setChanged() override {
        m_c1->setChanged();
        m_c2->setChanged();
    }

    bool stillValid(Player* player) override {
        return m_c1->stillValid(player) && m_c2->stillValid(player);
    }
};
