#include "String.h"

using namespace mule::Data::Basic;

void String::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	MultiValue tmp(stream->ReadString());
	lastSize = tmp.value.stringValue->length();
	dataHandler->OnDataRead(tmp);
}

void String::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	stream->Write(*(dataHandler->OnDataWrite().value.stringValue));
}

size_t String::Size() const
{
	return size_t(-1);
}

std::string mule::Data::Basic::String::GetTypeName() const
{
	return std::string("string");
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
