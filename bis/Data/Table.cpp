#include "Table.h"

#include <iostream>

using namespace mule::Data::Basic;
using namespace mule::Data;

Table::Table(Type *structure, const std::u16string &name, int length, size_t offset)
{
	if (structure == nullptr)
		throw xybase::InvalidParameterException(L"structure", L"should not be null!", 25200);

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
	
	if (length == 1)
	{
		infraType->Read(stream, dataHandler);
	}
	else for (int i = 0; i < length; ++i) {
		try
		{
			dataHandler->OnRealmEnter(infraType, i);
			infraType->Read(stream, dataHandler);
			dataHandler->OnRealmExit(infraType, i);
		}
		catch (xybase::Exception& ex)
		{
			logger.Error(L"Error when reading {} (0x{:08X}). Aborted.", i, ex.GetErrorCode());
			logger.Note(L"Exception: {}", ex.GetMessage());
			break;
		}
	}
	// dataHandler->AppendMetadatum(u"_type", MultiValue(u"array"));
	dataHandler->OnRealmExit((Type *) this, name);
	dataHandler->OnSheetReadEnd();
}

void Table::Write(xybase::Stream *stream, FileHandler * fileHandler)
{
	stream->Seek(offset, xybase::Stream::SM_BEGIN);
	fileHandler->OnSheetWriteStart();
	fileHandler->OnRealmEnter((Type *)this, name);
	if (length == 1)
	{
		infraType->Write(stream, fileHandler);
	}
	else for (int i = 0; i < length; ++i) {
		fileHandler->OnRealmEnter(infraType, i);
		infraType->Write(stream, fileHandler);
		fileHandler->OnRealmExit(infraType, i);
	}
	fileHandler->OnRealmExit((Type *)this, name);
	fileHandler->OnSheetWriteEnd();
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
