#include "Structure.h"

using namespace mule::Data;
using namespace mule::Data::Basic;

Structure::Structure()
{
	frozen = false;
	firstField = lastField = nullptr;
}

void Structure::AppendField(const std::string &name, Field * field)
{
	if (lastField == nullptr)
	{
		firstField = lastField = field;
	}
	else
	{
		lastField->next = field;
		lastField = field;
	}
	nameList.push_back(name);
}

void Structure::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	dataHandler->BeforeRecordRead();
	if (firstField != nullptr)
	{
		Field::FieldReadWriteContext context(stream, dataHandler);

		firstField->Read(context);
	}
	dataHandler->AfterRecordRead();
}

void Structure::Write(xybase::Stream *stream, DataHandler *dataHandler) const
{
	dataHandler->BeforeRecordWrite();
	if (firstField != nullptr)
	{
		Field::FieldReadWriteContext context(stream, dataHandler);

		firstField->Write(context);
	}
	dataHandler->AfterRecordWrite();
}

void Structure::Froze()
{
	frozen = true;
}

size_t Structure::Size() const
{
	return firstField == nullptr ? 0 : firstField->Size();
}
