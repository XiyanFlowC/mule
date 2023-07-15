#include "String.h"

using namespace mule::Data::Basic;

void String::Read(xybase::Stream *stream, DataHandler *dataHandler) const
{
	dataHandler->OnDataRead(MultiValue(stream->ReadString()));
}

void String::Write(xybase::Stream *stream, DataHandler *dataHandler) const
{
	stream->Write(*(dataHandler->OnDataWrite().value.stringValue));
}

size_t String::Size() const
{
	return size_t(4);
}

std::string mule::Data::Basic::String::GetTypeName() const
{
	return std::string("string");
}
