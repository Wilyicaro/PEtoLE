#include "DataLayer.hpp"

DataLayer::DataLayer(uint8_t* data, int size) : m_pData(data), m_size(size)
{

}

DataLayer::DataLayer(int size) : m_pData(new uint8_t[size]), m_size(size)
{
	memset(m_pData, 0, size);
}

DataLayer::~DataLayer()
{
	SAFE_DELETE_ARRAY(m_pData);
}

int DataLayer::operator[](const ChunkTilePos& pos) const
{
	return get(pos);
}

int DataLayer::get(const ChunkTilePos& pos) const
{
	int index = MakeBlockDataIndex(pos);

	uint8_t data = m_pData[index >> 1];
	if (index & 1)
		return data >> 4;
	return data & 0xF;
}

void DataLayer::set(const ChunkTilePos& pos, int data)
{
	assert((data & ~0xF) == 0);
	data &= 0xF;

	int index = MakeBlockDataIndex(pos);

	uint8_t& xdata = m_pData[index >> 1];
	if (index & 1)
		xdata = (xdata & 0x0F) | (data << 4);
	else
		xdata = (xdata & 0xF0) | (data);
}

bool DataLayer::isValid() const
{
	return m_pData != nullptr;
}
