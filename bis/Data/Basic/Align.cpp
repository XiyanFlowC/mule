#include "Align.h"
#include <xystring.h>

using namespace mule::Data::Basic;

mule::Data::Basic::Align::Align(int alignment)
{
	this->alignment = alignment;
}

size_t Align::Size() const
{
	return -1;
}

std::u16string mule::Data::Basic::Align::GetDataType() const
{
	return std::u16string(u"dummy/align");
}

MultiValue mule::Data::Basic::Align::DoRead(xybase::Stream *stream)
{
	auto cur = stream->Tell();
	auto aligned = (cur + alignment - 1) / alignment * alignment;
	stream->Seek(aligned, xybase::Stream::SM_BEGIN);

	return { 0 };
}

void mule::Data::Basic::Align::DoWrite(xybase::Stream *stream, const MultiValue &value)
{
	auto cur = stream->Tell();
	auto aligned = (cur + alignment - 1) / alignment * alignment;
	auto padding = aligned - cur;
	while (padding--)
	{
		stream->Write(static_cast<uint8_t>(0));
	}
}
