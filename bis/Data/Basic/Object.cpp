#include "Object.h"

void mule::Data::Basic::Object::DataHandler::OnSheetReadStart()
{
}

void mule::Data::Basic::Object::DataHandler::OnSheetReadEnd()
{
}

void mule::Data::Basic::Object::DataHandler::OnSheetWriteStart()
{
}

void mule::Data::Basic::Object::DataHandler::OnSheetWriteEnd()
{
}

size_t mule::Data::Basic::Object::GetLastSize() const
{
	return Size();
}

size_t mule::Data::Basic::Object::EvalSize(const MultiValue &obj) const
{
	return Size();
}

void mule::Data::Basic::Object::DataHandler::AppendMetadata(std::map<std::string, MultiValue> metadata)
{
	for (auto &datum : metadata)
	{
		AppendMetadatum(datum.first, datum.second);
	}
}

void mule::Data::Basic::Object::DataHandler::AppendMetadatum(std::string name, const MultiValue &value)
{
}

