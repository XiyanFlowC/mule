#include "String.h"

#include <xystring.h>
#include <Configuration.h>

using namespace mule::Data::Basic;

size_t String::Size() const
{
	return size_t(-1);
}

std::u16string mule::Data::Basic::String::GetDataType() const
{
	return u"string";
}

size_t mule::Data::Basic::String::GetLastSize() const
{
	return lastSize + 1;
}

size_t mule::Data::Basic::String::EvalSize(const MultiValue &obj) const
{
	if (obj.type != MultiValue::MVT_STRING)
		return 0;

	return obj.value.stringValue->size() + 1;
}

MultiValue mule::Data::Basic::String::DoRead(xybase::Stream *stream)
{
	auto rawString = stream->ReadString();
	MultiValue tmp(xybase::string::to_utf16(rawString));
	lastSize = rawString.size();
	tmp.metadata[u"size"] = lastSize;
	return tmp;
}

void mule::Data::Basic::String::DoWrite(xybase::Stream *stream, const MultiValue &value)
{
	if (!value.IsType(MultiValue::MVT_STRING)) throw xybase::InvalidParameterException(L"value", L"Type mismatch!", 19004);

	auto data = xybase::string::to_string(*(value.value.stringValue));

	auto &&itr = value.metadata.find(u"size");
	if (itr != value.metadata.end())
	{
		if (data.size() > itr->second.value.unsignedValue)
			throw xybase::InvalidParameterException(
				L"value",
				std::format(
					L"Too large (size {} exceeded {}), string={}",
					data.size(),
					itr->second.value.unsignedValue,
					xybase::string::to_wstring(*value.value.stringValue)),
				19010);
	}
	else
	{
		if (Configuration::GetInstance().GetSigned(u"mule.data.basic.string.read-for-size"))
		{
			auto cur = stream->Tell();
			auto size = stream->ReadString().size();
			if (data.size() > size)
			{
				throw xybase::InvalidParameterException(
					L"value",
					std::format(
						L"String too large! (size {} exceeded {}), string={}",
						data.size(),
						size,
						value.ToString()),
					19011
				);
			}

			stream->Seek(cur);
		}
	}

	if (value.value.stringValue->size() && data.size() == 0)
	{
		logger.Warn(L"Failed to encode string.");
		logger.Note(L"String={}", xybase::string::to_wstring(*value.value.stringValue));
	}

	stream->Write(data);
	lastSize = data.size();
}
