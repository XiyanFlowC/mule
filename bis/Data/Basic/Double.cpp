#include "Double.h"

using namespace mule::Data::Basic;

mule::Data::Basic::Double::Double() { }

void mule::Data::Basic::Double::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	dataHandler->OnDataRead(stream->ReadDouble());
}

void mule::Data::Basic::Double::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	stream->Write(dataHandler->OnDataWrite().value.realValue);
}

size_t mule::Data::Basic::Double::Size() const
{
	return sizeof(double);
}

std::string mule::Data::Basic::Double::GetTypeName() const
{
	return std::string("double");
}
