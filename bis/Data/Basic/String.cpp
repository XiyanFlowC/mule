#include "String.h"

#include <xystring.h>

using namespace mule::Data::Basic;

void String::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	MultiValue tmp(xybase::string::to_utf16(stream->ReadString()));
	lastSize = tmp.value.stringValue->length();
	dataHandler->OnDataRead(tmp);
}

void String::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	stream->Write(xybase::string::to_string(*(dataHandler->OnDataWrite().value.stringValue)));
}

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
