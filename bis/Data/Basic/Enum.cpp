#include "Enum.h"
#include <Data/TypeManager.h>
#include <xystring.h>

using namespace mule::Data::Basic;

mule::Data::Basic::Enum::Enum(const std::u16string &name, const std::u16string &baseType)
	: name(name)
{
	this->baseType = dynamic_cast<BasicType *>(mule::Data::TypeManager::GetInstance().GetOrCreateType(baseType));
	if (this->baseType == nullptr)
		throw xybase::InvalidParameterException(L"baseType", L"Invalid base type!", 499548);
}

void mule::Data::Basic::Enum::AddEnum(std::u16string name, MultiValue value)
{
	mapS2V[name] = value;
	mapV2S[value] = name;
}

size_t mule::Data::Basic::Enum::Size() const
{
	return baseType->Size();
}

std::u16string mule::Data::Basic::Enum::GetDataType() const
{
	return u"enum/" + name;
}

MultiValue mule::Data::Basic::Enum::DoRead(xybase::Stream *stream)
{
	auto val = baseType->DoRead(stream);
	return mapV2S.contains(val) ? mapV2S[val] : val;
}

void mule::Data::Basic::Enum::DoWrite(xybase::Stream *stream, const MultiValue &value)
{
	if (value.IsType(MultiValue::MVT_STRING))
	{
		if (!mapS2V.contains(*value.value.stringValue))
			throw xybase::InvalidOperationException(
				std::format(L"Unknown enum value {} for {}.", 
					value.ToString(), xybase::string::to_wstring(name)), 499549);

		baseType->DoWrite(stream, mapS2V[*value.value.stringValue]);
	}
	else baseType->DoWrite(stream, value);
}
