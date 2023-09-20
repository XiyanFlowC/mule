#include "BasicType.h"

void mule::Data::Basic::BasicType::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	auto val = DoRead(stream);
	if (!cacheVariableName.empty())
	{
		ContextManager::GetInstance().SetVariable(cacheVariableName, val);
	}
	dataHandler->OnDataRead(val);
}

void mule::Data::Basic::BasicType::Write(xybase::Stream *stream, FileHandler * fileHandler)
{
	MultiValue value = fileHandler->OnDataWrite();
	if (MultiValue::MV_NULL == value)
	{
		size_t size = Size();
		if (size == static_cast<size_t>(-1)) return;
		stream->Seek(size, xybase::Stream::SM_CURRENT);
		return;
	}

	if (!cacheVariableName.empty())
	{
		ContextManager::GetInstance().SetVariable(cacheVariableName, value);
	}
	DoWrite(stream, value);
}
