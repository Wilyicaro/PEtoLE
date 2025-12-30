#include "SynchedEntityData.hpp"


#define MAP(cType, typeEnum, value) template<> SynchedEntityData::DataType SynchedEntityData::DataTypeMap::typeFor<cType>() { return SynchedEntityData::typeEnum; } \
		template<> cType SynchedEntityData::DataTypeMap::defaultValueFor<cType>() { return value; }

MAP(int8_t, TYPE_INT8, 0)
MAP(int16_t, TYPE_INT16, 0)
MAP(int32_t, TYPE_INT32, 0)
MAP(float, TYPE_FLOAT, 0.0f) // float because VS2010 was bitching about float_t not existing despite including math.h or cmath
MAP(std::string, TYPE_STRING, Util::EMPTY_STRING)
MAP(ItemInstance, TYPE_ITEMINSTANCE, ItemInstance())
MAP(Vec3i, TYPE_VEC3I, Vec3i())
MAP(int64_t, TYPE_INT64, 0)
MAP(Vec3, TYPE_VEC3, Vec3())

#undef MAP

SynchedEntityData::SynchedEntityData()
{
    m_itemsArray = ItemsArray();
    m_minIdxDirty = -1;
    m_maxIdxDirty = 0;
    m_bDirty = false;
}

void SynchedEntityData::_updateMinMaxIdxDirty(DataID id)
{
    if (id < m_minIdxDirty)
        m_minIdxDirty = id;

    if (id > m_maxIdxDirty)
        m_maxIdxDirty = id;
}

SynchedEntityData::DataItem* SynchedEntityData::_get(DataID id) const
{
    // Data item not found
    assert(hasData(id));
    return m_itemsArray[id];
}

void SynchedEntityData::_resizeToContain(DataID id)
{
    for (int i = m_itemsArray.size(); i <= id; i++)
    {
        m_itemsArray.push_back(nullptr);
    }
}

bool SynchedEntityData::hasData(DataID id) const
{
    return id < m_itemsArray.size() && m_itemsArray[id] != nullptr;
}

static void WriteStringToBitStream(RakNet::BitStream& bs, const std::string& s) {
    uint16_t len = static_cast<uint16_t>(s.size());
    bs.Write(len);
    if (len > 0) bs.WriteAlignedBytes((const unsigned char*)s.data(), len);
}

static std::string ReadStringFromBitStream(RakNet::BitStream& bs) {
    uint16_t len = 0;
    bs.Read(len);
    std::string out;
    if (len > 0) {
        out.resize(len);
        bs.ReadAlignedBytes((unsigned char*)out.data(), len);
    }
    return out;
}

void SynchedEntityData::pack(const std::vector<DataItem*>& items, RakNet::BitStream& bs)
{
    if (!items.empty())
    {
        for (auto item : items)
        {
            uint8_t header = static_cast<uint8_t>((static_cast<uint8_t>(item->getType()) << C_ENTITYDATA_TYPE_SHIFT) | (item->getId() & C_ENTITYDATA_MAX_ID_VALUE));
            bs.Write(header);

            switch (item->getType())
            {
            case TYPE_INT8: {
                auto di = static_cast<DataHolder<int8_t>*>(item);
                int8_t v = di->getData();
                bs.Write(v);
                break;
            }
            case TYPE_INT16: {
                auto di = static_cast<DataHolder<int16_t>*>(item);
                int16_t v = di->getData();
                bs.Write(v);
                break;
            }
            case TYPE_INT32: {
                auto di = static_cast<DataHolder<int32_t>*>(item);
                int32_t v = di->getData();
                bs.Write(v);
                break;
            }
            case TYPE_FLOAT: {
                auto di = static_cast<DataHolder<float>*>(item);
                float v = di->getData();
                bs.Write(v);
                break;
            }
            case TYPE_STRING: {
                auto di = static_cast<DataHolder<std::string>*>(item);
                WriteStringToBitStream(bs, di->getData());
                break;
            }
            case TYPE_ITEMINSTANCE: {
                auto di = static_cast<DataHolder<ItemInstance>*>(item);
                const ItemInstance& ii = di->getData();
                int16_t itemId = static_cast<int16_t>(ii.getItem()->m_itemID);
                int8_t count = static_cast<int8_t>(ii.m_count);
                int16_t aux = static_cast<int16_t>(ii.getAuxValue());
                bs.Write(itemId);
                bs.Write(count);
                bs.Write(aux);
                break;
            }
            case TYPE_VEC3: {
                auto di = static_cast<DataHolder<Vec3>*>(item);
                const Vec3& v = di->getData();
                int32_t x = v.x, y = v.y, z = v.z;
                bs.Write(x); bs.Write(y); bs.Write(z);
                break;
            }
            default:
                break;
            }
        }
    }

    uint8_t eof = static_cast<uint8_t>(C_ENTITYDATA_EOF_MARKER);
    bs.Write(eof);
}

std::vector<SynchedEntityData::DataItem*> SynchedEntityData::packDirty()
{
    std::vector<DataItem*> result;
    if (m_bDirty)
    {
        for (size_t i = 0; i < m_itemsArray.size(); ++i)
        {
            DataItem* it = m_itemsArray[i];
            if (!it) continue;
            if (it->isDirty())
            {
                it->setDirty(false);
                result.push_back(it);
            }
        }
        m_minIdxDirty = -1;
        m_maxIdxDirty = 0;
        m_bDirty = false;
    }
    return result;
}

void SynchedEntityData::packAll(RakNet::BitStream& bs)
{
    for (size_t i = 0; i < m_itemsArray.size(); ++i)
    {
        DataItem* it = m_itemsArray[i];
        if (!it) continue;

        uint8_t header = static_cast<uint8_t>((static_cast<uint8_t>(it->getType()) << C_ENTITYDATA_TYPE_SHIFT) | (it->getId() & C_ENTITYDATA_MAX_ID_VALUE));
        bs.Write(header);

        switch (it->getType())
        {
        case TYPE_INT8:
        {
            auto di = static_cast<DataHolder<int8_t>*>(it);
            int8_t v = di->getData();
            bs.Write(v);
            break;
        }
        case TYPE_INT16:
        {
            auto di = static_cast<DataHolder<int16_t>*>(it);
            int16_t v = di->getData();
            bs.Write(v);
            break;
        }
        case TYPE_INT32:
        {
            auto di = static_cast<DataHolder<int32_t>*>(it);
            int32_t v = di->getData();
            bs.Write(v);
            break;
        }
        case TYPE_FLOAT:
        {
            auto di = static_cast<DataHolder<float>*>(it);
            float v = di->getData();
            bs.Write(v);
            break;
        }
        case TYPE_STRING:
        {
            auto di = static_cast<DataHolder<std::string>*>(it);
            WriteStringToBitStream(bs, di->getData());
            break;
        }
        case TYPE_ITEMINSTANCE:
        {
            auto di = static_cast<DataHolder<ItemInstance>*>(it);
            const ItemInstance& ii = di->getData();
            int16_t itemId = static_cast<int16_t>(ii.getItem()->m_itemID);
            int8_t count = static_cast<int8_t>(ii.m_count);
            int16_t aux = static_cast<int16_t>(ii.getAuxValue());
            bs.Write(itemId);
            bs.Write(count);
            bs.Write(aux);
            break;
        }
        case TYPE_VEC3I:
        {
            auto di = static_cast<DataHolder<Vec3i>*>(it);
            const Vec3i& v = di->getData();
            bs.Write(v.x); bs.Write(v.y); bs.Write(v.z);
            break;
        }
        case TYPE_VEC3:
        {
            auto di = static_cast<DataHolder<Vec3>*>(it);
            const Vec3& v = di->getData();
            bs.Write(v.x); bs.Write(v.y); bs.Write(v.z);
            break;
        }
        default:
            break;
        }
    }

    uint8_t eof = static_cast<uint8_t>(C_ENTITYDATA_EOF_MARKER);
    bs.Write(eof);
}

std::vector<std::unique_ptr<SynchedEntityData::DataItem>> SynchedEntityData::unpack(RakNet::BitStream& bs)
{
    std::vector<std::unique_ptr<DataItem>> list;

    while (true)
    {
        uint8_t header = 0;
        if (!bs.Read(header)) break;

        if (header == static_cast<uint8_t>(C_ENTITYDATA_EOF_MARKER))
            break;

        int type = (header & C_ENTITYDATA_TYPE_MASK) >> C_ENTITYDATA_TYPE_SHIFT;
        int id = header & C_ENTITYDATA_MAX_ID_VALUE;

        switch (type)
        {
        case TYPE_INT8:
        {
            int8_t v = 0; bs.Read(v);
            list.emplace_back(new DataHolder<int8_t>(TYPE_INT8, static_cast<DataID>(id), v));
            break;
        }
        case TYPE_INT16:
        {
            int16_t v = 0; bs.Read(v);
            list.emplace_back(new DataHolder<int16_t>(TYPE_INT16, static_cast<DataID>(id), v));
            break;
        }
        case TYPE_INT32:
        {
            int32_t v = 0; bs.Read(v);
            list.emplace_back(new DataHolder<int32_t>(TYPE_INT32, static_cast<DataID>(id), v));
            break;
        }
        case TYPE_FLOAT:
        {
            float v = 0.0f; bs.Read(v);
            list.emplace_back(new DataHolder<float>(TYPE_FLOAT, static_cast<DataID>(id), v));
            break;
        }
        case TYPE_STRING:
        {
            std::string s = ReadStringFromBitStream(bs);
            list.emplace_back(new DataHolder<std::string>(TYPE_STRING, static_cast<DataID>(id), s));
            break;
        }
        case TYPE_ITEMINSTANCE:
        {
            int16_t itemId = 0; int8_t count = 0; int16_t aux = 0;
            bs.Read(itemId); bs.Read(count); bs.Read(aux);
            ItemInstance ii(itemId, count, aux);
            list.emplace_back(new DataHolder<ItemInstance>(TYPE_ITEMINSTANCE, static_cast<DataID>(id), ii));
            break;
        }
        case TYPE_VEC3I:
        {
            int32_t x = 0, y = 0, z = 0;
            bs.Read(x); bs.Read(y); bs.Read(z);
            Vec3i v(x, y, z);
            list.emplace_back(new DataHolder<Vec3>(TYPE_VEC3I, static_cast<DataID>(id), v));
            break;
        }
        case TYPE_VEC3:
        {
            real x = 0, y = 0, z = 0;
            bs.Read(x); bs.Read(y); bs.Read(z);
            Vec3 v(x, y, z);
            list.emplace_back(new DataHolder<Vec3>(TYPE_VEC3, static_cast<DataID>(id), v));
            break;
        }
        default:
            break;
        }
    }

    return list;
}

void SynchedEntityData::assignValues(const std::vector<std::unique_ptr<DataItem>>& values)
{
    for (const auto& ptr : values)
    {
        DataItem* incoming = ptr.get();
        DataID id = incoming->getId();
        if (id < m_itemsArray.size() && m_itemsArray[id] != nullptr)
        {
            DataItem* existing = m_itemsArray[id];
            if (existing->getType() == incoming->getType())
            {
                switch (existing->getType())
                {
                case TYPE_INT8:
                {
                    auto dest = static_cast<DataHolder<int8_t>*>(existing);
                    auto src = static_cast<DataHolder<int8_t>*>(incoming);
                    dest->setData(src->getData());
                    break;
                }
                case TYPE_INT16:
                {
                    auto dest = static_cast<DataHolder<int16_t>*>(existing);
                    auto src = static_cast<DataHolder<int16_t>*>(incoming);
                    dest->setData(src->getData());
                    break;
                }
                case TYPE_INT32:
                {
                    auto dest = static_cast<DataHolder<int32_t>*>(existing);
                    auto src = static_cast<DataHolder<int32_t>*>(incoming);
                    dest->setData(src->getData());
                    break;
                }
                case TYPE_FLOAT:
                {
                    auto dest = static_cast<DataHolder<float>*>(existing);
                    auto src = static_cast<DataHolder<float>*>(incoming);
                    dest->setData(src->getData());
                    break;
                }
                case TYPE_STRING:
                {
                    auto dest = static_cast<DataHolder<std::string>*>(existing);
                    auto src = static_cast<DataHolder<std::string>*>(incoming);
                    dest->setData(src->getData());
                    break;
                }
                case TYPE_ITEMINSTANCE:
                {
                    auto dest = static_cast<DataHolder<ItemInstance>*>(existing);
                    auto src = static_cast<DataHolder<ItemInstance>*>(incoming);
                    dest->setData(src->getData());
                    break;
                }
                case TYPE_VEC3I:
                {
                    auto dest = static_cast<DataHolder<Vec3i>*>(existing);
                    auto src = static_cast<DataHolder<Vec3i>*>(incoming);
                    dest->setData(src->getData());
                    break;
                }
                case TYPE_VEC3:
                {
                    auto dest = static_cast<DataHolder<Vec3>*>(existing);
                    auto src = static_cast<DataHolder<Vec3>*>(incoming);
                    dest->setData(src->getData());
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
}

bool SynchedEntityData::isDirty() const
{
    return m_bDirty;
}
