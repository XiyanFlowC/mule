#include "Dummy.h"
#include <xystring.h>

using namespace mule::Data::Basic;

mule::Data::Basic::Dummy::Dummy(int size)
{
	this->size = size;
}

size_t Dummy::Size() const
{
	return size >> 3;
}

std::u16string mule::Data::Basic::Dummy::GetDataType() const
{
	return std::u16string(u"dummy/") + u"bit" + xybase::string::to_utf16(std::to_string(size));
}

MultiValue mule::Data::Basic::Dummy::DoRead(xybase::Stream *stream)
{
	stream->Seek(size >> 3, xybase::Stream::SM_CURRENT);

	return { u"" };
}

void mule::Data::Basic::Dummy::DoWrite(xybase::Stream *stream, const MultiValue &value)
{
	stream->Seek(size >> 3, xybase::Stream::SM_CURRENT);
}
