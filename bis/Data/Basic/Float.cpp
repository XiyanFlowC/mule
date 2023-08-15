#include "Float.h"

using namespace mule::Data::Basic;

mule::Data::Basic::Float::Float() { }

void mule::Data::Basic::Float::Read(xybase::Stream *stream, DataHandler *handler)
{
	handler->OnDataRead(stream->ReadFloat());
}

void mule::Data::Basic::Float::Write(xybase::Stream *stream, DataHandler *handler)
{
	stream->Write((float)(handler->OnDataWrite().value.realValue));
}

size_t mule::Data::Basic::Float::Size() const
{
	return size_t(sizeof(float));
}

std::u16string mule::Data::Basic::Float::GetTypeName() const
{
	return std::u16string(u"float");
}
