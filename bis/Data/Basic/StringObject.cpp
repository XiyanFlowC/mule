#include "StringObject.h"

using namespace mule::Data::Basic;

void StringObject::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	dataHandler->AfterCellRead(MultiValue(stream->ReadString()));
}

void StringObject::Write(xybase::Stream *stream, DataHandler *dataHandler) const
{
	stream->Write(*(dataHandler->BeforeCellWrite().value.stringValue));
}

size_t StringObject::Size() const
{
	return size_t(0);
}
