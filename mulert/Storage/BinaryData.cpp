#include "BinaryData.h"
#include <cstring>
using namespace mule::Storage;

BinaryData::BinaryData(char *data, size_t length, bool duplicate)
{
    SetData(data, length, duplicate);
}

mule::Storage::BinaryData::~BinaryData()
{
}

const char *BinaryData::GetData() const noexcept
{
    return data.get();
}

size_t BinaryData::GetLength() const noexcept
{
    return length;
}

void BinaryData::SetData(char *data, size_t length, bool duplicate)
{
    if (duplicate)
    {
        this->data = std::shared_ptr<char[]>(new char[length]);
        memcpy(this->data.get(), data, length);
    }
    else
    {
        this->data = std::shared_ptr<char[]>(data);
    }
    this->length = length;
}

void BinaryData::SetData(const char *data, size_t length)
{
    SetData((char *)data, length, true);
}
