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

void mule::Data::Basic::Type::DataHandler::AppendMetadata(std::map<std::string, MultiValue> metadata)
{
	for (auto &datum : metadata)
	{
		AppendMetadatum(datum.first, datum.second);
	}
}

void mule::Data::Basic::Type::DataHandler::AppendMetadatum(std::string name, const MultiValue &value)
{
}

