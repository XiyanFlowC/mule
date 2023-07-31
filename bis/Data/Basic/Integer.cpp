#include "Integer.h"

using namespace mule::Data::Basic;
using namespace mule::Exception;

mule::Data::Basic::Integer::Integer(int size, bool isUnsigned)
{
	this->size = size;
	this->isUnsigned = isUnsigned;
}

size_t Integer::Size() const
{
	return size >> 3;
}

void mule::Data::Basic::Integer::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	MultiValue value(0LL);
	switch (size >> 3)
	{
	case 1:
		value.value.unsignedValue = stream->ReadUInt8();
		break;
	case 2:
		value.value.unsignedValue = stream->ReadUInt16();
		break;
	case 4:
		value.value.unsignedValue = stream->ReadUInt32();
		break;
	case 8:
		value.value.unsignedValue = stream->ReadUInt64();
		break;
	}

	if (!isUnsigned && value.value.unsignedValue & (1ULL << (size - 1)))
	{
		uint64_t leading = 0xFFFFFFFFFFFFFFFFULL;
		leading <<= size;
		value.value.unsignedValue |= leading;
	}
	else
	{
		value.type = MultiValue::MVT_UINT;
	}

	dataHandler->OnDataRead(value);
}

void mule::Data::Basic::Integer::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	switch (size >> 3)
	{
	case 1:
		stream->Write((uint8_t)(dataHandler->OnDataWrite().value.unsignedValue & 0xFF));
		break;
	case 2:
		stream->Write((uint16_t)(dataHandler->OnDataWrite().value.unsignedValue & 0xFFFF));
		break;
	case 4:
		stream->Write((uint32_t)(dataHandler->OnDataWrite().value.unsignedValue & 0xFFFFFFFF));
		break;
	case 8:
		stream->Write((uint64_t)(dataHandler->OnDataWrite().value.unsignedValue));
		break;
	}
}

std::string mule::Data::Basic::Integer::GetTypeName() const
{
	return std::string("int") + std::to_string(size);
}
