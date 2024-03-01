#include "VarChar.h"

#include "../xybase/xystring.h"

using namespace mule::Data::Basic;

mule::Data::VarChar::VarChar(size_t length)
	: length(length) 
{
	buffer = new char[length];
}

mule::Data::VarChar::~VarChar()
{
	delete[] buffer;
}

size_t mule::Data::VarChar::Size() const
{
	return length;
}

std::u16string mule::Data::VarChar::GetDataType() const
{
	return u"string/text";
}

MultiValue mule::Data::VarChar::DoRead(xybase::Stream *stream)
{
	stream->ReadBytes(buffer, length);
	return xybase::string::to_utf16(buffer);
}

void mule::Data::VarChar::DoWrite(xybase::Stream *stream, const MultiValue &value)
{
	if (value.GetType() != Basic::MultiValue::MVT_STRING)
	{
		stream->Seek(Size(), xybase::Stream::SM_CURRENT);
		return;
	}
	auto &&str = *value.value.stringValue;
	if (str.size() < length - 1)
	{
		memset(buffer, 0, length);
		strcpy(buffer, xybase::string::to_string(str).c_str());
		stream->Write(buffer, length);
	}
	else
	{
		stream->Write(xybase::string::to_string(str.substr(0, length - 1)));
	}
}

mule::Data::Basic::Type *mule::Data::VarChar::VarCharCreator::DoCreateObject(const std::u16string &info)
{
	if (!info.starts_with(u"varchar(") || !info.ends_with(u')'))
	{
		return nullptr;
	}
	size_t endIndex = info.find_first_of(u")");
	if (endIndex == std::string::npos)
	{
		return nullptr;
	}
	unsigned long long length = xybase::string::stoi(info.substr(8, endIndex - 8));
	VarChar *ret = new VarChar(length);

	return ret;
}
