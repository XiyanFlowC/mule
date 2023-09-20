#include "Type.h"

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

mule::Data::Basic::Type::DataHandler::~DataHandler()
{
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

void mule::Data::Basic::Type::DataHandler::SetOutStream(xybase::TextStream *stream)
{
	outstream = stream;
}

mule::Data::Basic::Type::Handler::~Handler()
{
}

mule::Data::Basic::Type::FileHandler::~FileHandler()
{
}

void mule::Data::Basic::Type::FileHandler::SetInStream(xybase::TextStream *stream)
{
	instream = stream;
}
