#include "Type.h"

void mule::Data::Basic::Type::DataHandler::OnSheetReadStart()
{
}

void mule::Data::Basic::Type::DataHandler::OnSheetReadEnd()
{
}

void mule::Data::Basic::Type::DataHandler::OnSheetWriteStart()
{
}

void mule::Data::Basic::Type::DataHandler::OnSheetWriteEnd()
{
}

mule::Data::Basic::Type::~Type()
{
}

size_t mule::Data::Basic::Type::GetLastSize() const
{
	return Size();
}

size_t mule::Data::Basic::Type::EvalSize(const MultiValue &obj) const
{
	return Size();
}

bool mule::Data::Basic::Type::IsComposite() const
{
	return false;
}

void mule::Data::Basic::Type::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	dataHandler->OnDataRead(DoRead(stream));
}

void mule::Data::Basic::Type::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	MultiValue value = dataHandler->OnDataWrite();
	if (MultiValue::MV_NULL == value)
	{
		size_t size = Size();
		if (size == static_cast<size_t>(-1)) return;
		stream->Seek(size, 1);
	}
	DoWrite(stream, value);
}

void mule::Data::Basic::Type::DataHandler::AppendMetadata(std::map<std::u16string, MultiValue> metadata)
{
	for (auto &datum : metadata)
	{
		AppendMetadatum(datum.first, datum.second);
	}
}

void mule::Data::Basic::Type::DataHandler::AppendMetadatum(std::u16string name, const MultiValue &value)
{
}

