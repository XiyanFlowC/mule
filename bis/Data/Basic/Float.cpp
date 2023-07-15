#include "Float.h"

using namespace mule::Data::Basic;

mule::Data::Basic::Float::Float() { }

void mule::Data::Basic::Float::Read(xybase::Stream *stream, DataHandler *handler) const
{
	handler->OnDataRead(stream->ReadFloat());
}

void mule::Data::Basic::Float::Write(xybase::Stream *stream, DataHandler *handler) const
{
	stream->Write((float)(handler->OnDataWrite().value.realValue));
}

size_t mule::Data::Basic::Float::Size() const
{
	return size_t(sizeof(float));
}

std::string mule::Data::Basic::Float::GetTypeName() const
{
	return std::string("float");
}
