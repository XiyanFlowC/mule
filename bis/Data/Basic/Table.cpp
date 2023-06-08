#include "Table.h"

using namespace mule::Data::Basic;

Table::Table(Basic::Structure *structure, const std::string &name, int length, size_t offset)
{
	this->structure = structure;
	this->length = length;
	this->offset = offset;
	this->name = name;
}

void Table::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	stream->Seek(offset, 0);
	dataHandler->BeforeSheetRead(name);
	for (int i = 0; i < length; ++i) {
		structure->Read(stream, dataHandler);
	}
	dataHandler->AfterSheetRead();
}

void Table::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	stream->Seek(offset, 0);
	dataHandler->BeforeSheetWrite(name);
	for (int i = 0; i < length; ++i) {
		structure->Write(stream, dataHandler);
	}
	dataHandler->AfterSheetRead();
}
