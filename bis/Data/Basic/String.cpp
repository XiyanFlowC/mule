#include "String.h"

#include <xystring.h>

using namespace mule::Data::Basic;

size_t String::Size() const
{
	return size_t(-1);
}

std::u16string mule::Data::Basic::String::GetTypeName() const
{
	return u"string";
}

size_t mule::Data::Basic::String::GetLastSize() const
{
	return lastSize;
}

size_t mule::Data::Basic::String::EvalSize(const MultiValue &obj) const
{
	if (obj.type != MultiValue::MVT_STRING)
		return 0;

	return obj.value.stringValue->size() + 1;
}

MultiValue mule::Data::Basic::String::DoRead(xybase::Stream *stream)
{
	MultiValue tmp(xybase::string::to_utf16(stream->ReadString()));
	lastSize = tmp.value.stringValue->length();
	return tmp;
}

void mule::Data::Basic::String::DoWrite(xybase::Stream *stream, const MultiValue &value)
{
	stream->Write(xybase::string::to_string(*(value.value.stringValue)));
}
