#include "Table.h"

#include <iostream>

using namespace mule::Data::Basic;
using namespace mule::Data;

Table::Table(Type *structure, const std::u16string &name, int length, size_t offset)
{
	this->infraType = structure;
	this->length = length;
	this->offset = offset;
	this->name = name;
}

void Table::Read(xybase::Stream *stream, Type::DataHandler *dataHandler)
{
	stream->Seek(offset, xybase::Stream::SM_BEGIN);
	dataHandler->OnSheetReadStart();
	dataHandler->OnRealmEnter((Type *)this, name);
	for (int i = 0; i < length; ++i) {
		try
		{
			dataHandler->OnRealmEnter(infraType, i);
			infraType->Read(stream, dataHandler);
			dataHandler->OnRealmExit(infraType, i);
		}
		catch (xybase::Exception& ex)
		{
			std::cerr << "Error when read, abort for now, at " << i << " (Error: " << ex.GetErrorCode() << std::endl;
			std::cerr << ex.what() << std::endl;
			break;
		}
	}
	// dataHandler->AppendMetadatum(u"_type", MultiValue(u"array"));
	dataHandler->OnRealmExit((Type *) this, name);
	dataHandler->OnSheetReadEnd();
}

void Table::Write(xybase::Stream *stream, Type::DataHandler *dataHandler)
{
	stream->Seek(offset, xybase::Stream::SM_BEGIN);
	dataHandler->OnSheetWriteStart();
	dataHandler->OnRealmEnter((Type *)this, name);
	for (int i = 0; i < length; ++i) {
		dataHandler->OnRealmEnter(infraType, i);
		infraType->Write(stream, dataHandler);
		dataHandler->OnRealmExit(infraType, i);
	}
	dataHandler->OnRealmExit((Type *)this, name);
	dataHandler->OnSheetWriteEnd();
}

size_t mule::Data::Table::Size() const
{
	return length * infraType->Size();
}

int mule::Data::Table::Length() const
{
	return length;
}

std::u16string mule::Data::Table::GetDataType() const
{
	return u"table";
}

bool mule::Data::Table::IsComposite() const
{
	return true;
}
