#include "ResultContainer.hpp"
#include "ItemInstance.hpp"

ResultContainer::ResultContainer() {
    items[0] = nullptr;
}

int ResultContainer::getContainerSize() {
    return 1;
}

std::shared_ptr<ItemInstance> ResultContainer::getItem(int index) {
    return items[index];
}

std::string ResultContainer::getName() {
    return "Result";
}

std::shared_ptr<ItemInstance> ResultContainer::removeItem(int index, int) {
    if (items[index] != nullptr) {
        std::shared_ptr<ItemInstance> result = items[index];
        items[index] = nullptr;
        return result;
    }
    return nullptr;
}

void ResultContainer::setItem(int index, std::shared_ptr<ItemInstance> item) {
    items[index] = item;
}

int ResultContainer::getMaxStackSize() {
    return 64;
}

void ResultContainer::setChanged() {
    // No-op
}

bool ResultContainer::stillValid(Player*) {
    return true;
}
