#include "Table.h"

#include <iostream>

using namespace mule::Data::Basic;
using namespace mule::Data;

Table::Table(Structure *structure, const std::u16string &name, int length, size_t offset)
{
	this->structure = structure;
	this->length = length;
	this->offset = offset;
	this->name = name;
}

void Table::Read(xybase::Stream *stream, Type::DataHandler *dataHandler)
{
	stream->Seek(offset, SEEK_SET);
	dataHandler->OnSheetReadStart();
	dataHandler->OnRealmEnter((Type *)this, name);
	for (int i = 0; i < length; ++i) {
		try
		{
			dataHandler->OnRealmEnter(structure, i);
			structure->Read(stream, dataHandler);
			dataHandler->OnRealmExit(structure, i);
		}
		catch (xybase::Exception& ex)
		{
			std::cerr << "Error when read, abort for now, at " << i << std::endl;
			std::cerr << ex.what() << std::endl;
			break;
		}
	}
	dataHandler->AppendMetadatum(u"_type", MultiValue(u"array"));
	dataHandler->OnRealmExit((Type *) this, name);
	dataHandler->OnSheetReadEnd();
}

void Table::Write(xybase::Stream *stream, Type::DataHandler *dataHandler)
{
	stream->Seek(offset, SEEK_SET);
	dataHandler->OnSheetWriteStart();
	dataHandler->OnRealmEnter((Type *)this, name);
	for (int i = 0; i < length; ++i) {
		dataHandler->OnRealmEnter(structure, i);
		structure->Write(stream, dataHandler);
		dataHandler->OnRealmExit(structure, i);
	}
	dataHandler->OnRealmExit((Type *)this, name);
	dataHandler->OnSheetWriteEnd();
}

size_t mule::Data::Table::Size() const
{
	return length * structure->Size();
}

int mule::Data::Table::Length() const
{
	return length;
}

std::u16string mule::Data::Table::GetTypeName() const
{
	return u"table";
}

bool mule::Data::Table::IsComposite() const
{
	return true;
}

MultiValue mule::Data::Table::DoRead(xybase::Stream *stream)
{
	return MultiValue();
}

void mule::Data::Table::DoWrite(xybase::Stream *stream, const MultiValue &value)
{
}
