#include "Float.h"

using namespace mule::Data::Basic;

mule::Data::Basic::Float::Float() { }


std::u16string mule::Data::Basic::Float::GetTypeName() const
{
	return std::u16string(u"float");
}

size_t mule::Data::Basic::Float::Size() const
{
	return size_t(sizeof(float));
}

MultiValue mule::Data::Basic::Float::DoRead(xybase::Stream *stream)
{
	return stream->ReadFloat();
}

void mule::Data::Basic::Float::DoWrite(xybase::Stream *stream, const MultiValue &value)
{
	stream->Write((float)(value.value.realValue));
}
