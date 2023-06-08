#include "IntegerField.h"

using namespace mule::Data::Basic;
using namespace mule::Exception;

mule::Data::Basic::IntegerField::IntegerField(int size, bool isUnsigned)
{
	this->size = size;
	this->isUnsigned = isUnsigned;
}

size_t IntegerField::Size() const
{
	if (next != nullptr) return next->Size() + (size >> 3);

	return size >> 3;
}

size_t IntegerField::FieldSize() const
{
	return size >> 3;
}

MultiValue IntegerField::DoRead(FieldReadWriteContext& context)
{
	MultiValue value(0LL);
	switch (size >> 3)
	{
	case 1:
		value.value.unsignedValue = context.stream->ReadUInt8();
		break;
	case 2:
		value.value.unsignedValue = context.stream->ReadUInt16();
		break;
	case 4:
		value.value.unsignedValue = context.stream->ReadUInt32();
		break;
	case 8:
		value.value.unsignedValue = context.stream->ReadUInt64();
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

	return value;
}

int IntegerField::DoWrite(FieldReadWriteContext& context, const MultiValue &value) const
{
	switch (size >> 3)
	{
	case 1:
		context.stream->Write((uint8_t)(value.value.unsignedValue & 0xFF));
		break;
	case 2:
		context.stream->Write((uint16_t)(value.value.unsignedValue & 0xFFFF));
		break;
	case 4:
		context.stream->Write((uint32_t)(value.value.unsignedValue & 0xFFFFFFFF));
		break;
	case 8:
		context.stream->Write((uint64_t)(value.value.unsignedValue));
		break;
	}
	return 0;
}
