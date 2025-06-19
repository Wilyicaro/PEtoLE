#pragma once
#include "Tag.hpp"
#include "ByteArrayTag.hpp"
#include "ListTag.hpp"
#include "ByteTag.hpp"
#include "IntTag.hpp"
#include "LongTag.hpp"
#include "ShortTag.hpp"
#include "StringTag.hpp"
#include "FloatTag.hpp"
#include "DoubleTag.hpp"

class CompoundTag : public Tag {
    public:
        std::map<std::string, std::shared_ptr<Tag>> tags;
    
        TagType getType() const override { return TagType::TAG_Compound; }
        void write(std::ostream& os) const override;
        void read(std::istream& is) override;
    
        void put(std::string name, std::shared_ptr<Tag> tag);
        void put(std::shared_ptr<Tag> tag) {
            put(tag->name, tag);
        };
        std::shared_ptr<Tag> get(const std::string& name) const;

        bool contains(const std::string& name);

        std::vector<int8_t> getByteArray(const std::string& name) {
            return std::dynamic_pointer_cast<ByteArrayTag>(get(name))->getValue();
        }

        std::shared_ptr<CompoundTag> getCompound(const std::string& name) {
            return std::dynamic_pointer_cast<CompoundTag>(get(name));
        }

        std::shared_ptr<CompoundTag> getOrMakeCompound(const std::string& name) {
            auto tag = getCompound(name);
            return tag ? tag : std::make_shared<CompoundTag>();
        }

        std::shared_ptr<ListTag> getList(const std::string& name) {
            return std::dynamic_pointer_cast<ListTag>(get(name));
        }

        int8_t getByte(const std::string& name, int8_t fallback = 0) {
            auto tag = std::dynamic_pointer_cast<ByteTag>(get(name));
            return tag ? tag->getValue() : fallback;
        }

        int16_t getShort(const std::string& name, int16_t fallback = 0) {
            auto tag = std::dynamic_pointer_cast<ShortTag>(get(name));
            return tag ? tag->getValue() : fallback;
        }

        int32_t getInt(const std::string& name, int32_t fallback = 0) {
            auto tag = std::dynamic_pointer_cast<IntTag>(get(name));
            return tag ? tag->getValue() : fallback;
        }

        int64_t getLong(const std::string& name, int64_t fallback = 0) {
            auto tag = std::dynamic_pointer_cast<LongTag>(get(name));
            return tag ? tag->getValue() : fallback;
        }

        float getFloat(const std::string& name, float fallback = 0) {
            auto tag = std::dynamic_pointer_cast<FloatTag>(get(name));
            return tag ? tag->getValue() : fallback;
        }

        double getDouble(const std::string& name, double fallback = 0) {
            auto tag = std::dynamic_pointer_cast<DoubleTag>(get(name));
            return tag ? tag->getValue() : fallback;
        }

        std::string getString(const std::string& name, const std::string& fallback = "") {
            auto tag = std::dynamic_pointer_cast<StringTag>(get(name));
            return tag ? tag->getValue() : fallback;
        }

        bool getBoolean(const std::string& name, bool fallback = false) {
            return getByte(name, fallback);
        }

        void putByteArray(const std::string& name, uint8_t* value, size_t size) {
            std::vector<int8_t> arr;
            for (size_t i = 0; i < size; ++i)
                arr.push_back(static_cast<int8_t>(value[i]));
            putByteArray(name, arr);
        }

        void putByteArray(const std::string& name, std::vector<int8_t> value) {
            put(name, std::make_shared<ByteArrayTag>(value));
        }

        void putDoubleList(const std::string& name, std::initializer_list<double> list) {
            auto listTag = std::make_shared<ListTag>(TagType::TAG_Double);
            for (double i : list) {
                listTag->add(std::make_shared<DoubleTag>(i));
            }
            put(name, listTag);
        }

        void putFloatList(const std::string& name, std::initializer_list<float> list) {
            auto listTag = std::make_shared<ListTag>(TagType::TAG_Float);
            for (float i : list) {
                listTag->add(std::make_shared<FloatTag>(i));
            }
            put(name, listTag);
        }

        void putByte(const std::string& name, int8_t value) {
            put(name, std::make_shared<ByteTag>(value));
        }

        void putShort(const std::string& name, int16_t value) {
            put(name, std::make_shared<ShortTag>(value));
        }

        void putInt(const std::string& name, int32_t value) {
            put(name, std::make_shared<IntTag>(value));
        }

        void putLong(const std::string& name, int64_t value) {
            put(name, std::make_shared<LongTag>(value));
        }

        void putDouble(const std::string& name, double value) {
            put(name, std::make_shared<DoubleTag>(value));
        }

        void putFloat(const std::string& name, float value) {
            put(name, std::make_shared<FloatTag>(value));
        }

        void putBoolean(const std::string& name, bool value) {
            putByte(name, value);
        }

        void putString(const std::string& name, std::string value) {
            put(name, std::make_shared<StringTag>(value));
        }
    };