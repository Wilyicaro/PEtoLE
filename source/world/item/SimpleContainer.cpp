#include "SimpleContainer.hpp"
#include "client/locale/Language.hpp"

SimpleContainer::SimpleContainer(int size, const std::string& name) : m_name(name), m_items(size)
{
}

int SimpleContainer::getContainerSize() 
{
    return m_items.size();
}

std::shared_ptr<ItemInstance> SimpleContainer::getItem(int index) 
{
    return m_items[index];
}

std::shared_ptr<ItemInstance> SimpleContainer::removeItem(int index, int count)
{
    if (m_items[index]) {
        std::shared_ptr<ItemInstance> result;
        if (m_items[index]->m_count <= count) {
            result = m_items[index];
            m_items[index] = nullptr;
            setChanged();
            return result;
        }
        else {
            result = m_items[index]->remove(count);
            if (!m_items[index]->m_count) {
                m_items[index] = nullptr;
            }
            setChanged();
            return result;
        }
    }
    return nullptr;
}

void SimpleContainer::setItem(int index, std::shared_ptr<ItemInstance> item)
{
    m_items[index] = item;
    if (item && item->m_count > getMaxStackSize()) {
        item->m_count = getMaxStackSize();
    }
    setChanged();
}

const std::string& SimpleContainer::getName()
{
    return Language::getInstance()->get(m_name);
}

int SimpleContainer::getMaxStackSize()
{
    return 64;
}

void SimpleContainer::setChanged()
{
}

bool SimpleContainer::stillValid(Player* player)
{
    return true;
}

void SimpleContainer::load(CompoundIO tag)
{
    clear();
    auto list = tag->getList("Items");

    for (const auto& element : list->getValue()) {
        auto itemTag = std::dynamic_pointer_cast<CompoundTag>(element);
        if (itemTag) {
            int slot = itemTag->getByte("Slot") & 255;
            if (slot >= 0 && slot < static_cast<int>(m_items.size())) {
                m_items[slot] = std::make_shared<ItemInstance>(itemTag);
            }
        }
    }
}

void SimpleContainer::save(CompoundIO tag)
{
    auto list = std::make_shared<ListTag>();

    for (int i = 0; i < static_cast<int>(m_items.size()); ++i) {
        if (m_items[i]) {
            auto itemTag = std::make_shared<CompoundTag>();
            itemTag->putByte("Slot", static_cast<uint8_t>(i));
            m_items[i]->save(itemTag);
            list->add(itemTag);
        }
    }

    tag->put("Items", list);
}

void SimpleContainer::clear()
{
    std::fill(m_items.begin(), m_items.end(), nullptr);
}