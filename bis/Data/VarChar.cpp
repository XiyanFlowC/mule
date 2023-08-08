#include "VarChar.h"

mule::Data::VarChar::VarChar(size_t length)
	: length(length) 
{
	buffer = new char[length];
}

mule::Data::VarChar::~VarChar()
{
	delete[] buffer;
}

void mule::Data::VarChar::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	stream->ReadBytes(buffer, length);
	dataHandler->OnDataRead(std::string(buffer));
}

void mule::Data::VarChar::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	auto &&data = dataHandler->OnDataWrite();
	if (data.GetType() != Basic::MultiValue::MVT_STRING)
	{
		stream->Seek(Size(), 1);
		return ;
	}
	auto &&str = *data.value.stringValue;
	if (str.size() < length - 1)
	{
		memset(buffer, 0, length);
		strcpy(buffer, str.c_str());
		stream->Write(buffer, length);
	}
	else
	{
		stream->Write(str.substr(0, length - 1));
	}
}

size_t mule::Data::VarChar::Size() const
{
	return length;
}

std::string mule::Data::VarChar::GetTypeName() const
{
	return "varchar(" + std::to_string(length) + ')';
}

mule::Data::Basic::Type *mule::Data::VarChar::VarCharCreator::DoCreateObject(std::string info)
{
	if (!info.starts_with("varchar(") || !info.ends_with(')'))
	{
		return nullptr;
	}
	size_t endIndex = info.find_first_of(")");
	if (endIndex == std::string::npos)
	{
		return nullptr;
	}
	unsigned long long length = xybase::string::stoi(info.substr(8, endIndex - 8));
	VarChar *ret = new VarChar(length);

	return ret;
}
