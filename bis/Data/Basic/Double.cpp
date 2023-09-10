#include "Double.h"

using namespace mule::Data::Basic;

mule::Data::Basic::Double::Double() { }

size_t mule::Data::Basic::Double::Size() const
{
	return sizeof(double);
}

std::u16string mule::Data::Basic::Double::GetDataType() const
{
	return std::u16string(u"double");
}

MultiValue mule::Data::Basic::Double::DoRead(xybase::Stream *stream)
{
	return stream->ReadDouble();
}

void mule::Data::Basic::Double::DoWrite(xybase::Stream *stream, const MultiValue &value)
{
	if (!value.IsType(MultiValue::MVT_REAL)) throw xybase::InvalidParameterException(L"value", L"Type mismatch!", 19001);
	stream->Write((double)value.value.realValue);
}
