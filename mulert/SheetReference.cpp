#include "SheetReference.h"
#include "Data/Basic/ContextManager.h"
#include "Data/TypeManager.h"
#include "Configuration.h"
#include <xystring.h>

std::u16string mule::SheetReference::GenerateName(size_t offset, std::u16string streamName)
{
	auto name = namePattern;
	if (Configuration::GetInstance().IsExist(u"mule.sheet-reference.name-pattern"))
	{
		name = Configuration::GetInstance().GetString(u"mule.sheet-reference.name-pattern");
	}
	xybase::string::replace_in_place<char16_t>(streamName, u"/", u"_");
	xybase::string::replace_in_place<char16_t>(streamName, u"\\", u"_");
	xybase::string::replace_in_place<char16_t>(streamName, u":", u"_");
	xybase::string::replace_in_place<char16_t>(streamName, u"?", u"_");
	xybase::string::replace_in_place<char16_t>(streamName, u"|", u"_");
	xybase::string::replace_in_place<char16_t>(streamName, u"*", u"_");
	xybase::string::replace_in_place<char16_t>(streamName, u"\"", u"_");
	xybase::string::replace_in_place<char16_t>(streamName, u"<", u"_");
	xybase::string::replace_in_place<char16_t>(streamName, u">", u"_");
	name = xybase::string::replace<char16_t>(name, u"${offset}", xybase::string::itos<char16_t>(offset, 16));
	name = xybase::string::replace<char16_t>(name, u"${stream}", streamName);

	// 变量替换
	auto startPosition = name.find(u"${");
	while (startPosition != std::u16string::npos)
	{
		auto endPosition = name.find(u"}", startPosition);
		auto variableName = name.substr(startPosition + 2, endPosition - startPosition - 2);
		auto value = xybase::string::to_utf16(Configuration::GetInstance().GetVariable(variableName.c_str()).ToString());
		name.replace(startPosition, endPosition - startPosition + 1, value);
		startPosition = name.find(u"${");
	}

	return name;
}

mule::SheetReference::~SheetReference()
{
}

size_t mule::SheetReference::Size() const
{
	return size_t(4);
}

std::u16string mule::SheetReference::GetDataType() const
{
	return u"string/tag";
}

void mule::SheetReference::Read(xybase::Stream *stream, mule::Data::Basic::Type::DataHandler *dataHandler)
{
	auto &tloc = mule::Data::Basic::ContextManager::GetInstance().GetVariable(locCacheName);
	auto size = sizeDefined;
	if (sizeDefined == (size_t)-1)
	{
		auto tsiz = mule::Data::Basic::ContextManager::GetInstance().GetVariable(sizeCacheName);
		if ((!tloc.IsType(Data::Basic::MultiValue::MVT_UINT) && !tloc.IsType(Data::Basic::MultiValue::MVT_INT))
			|| (!tsiz.IsType(Data::Basic::MultiValue::MVT_UINT) && !tsiz.IsType(Data::Basic::MultiValue::MVT_INT))) return;
		size = tsiz.value.unsignedValue;
	}

	if (tloc.value.unsignedValue == 0 || size == 0)
	{
		dataHandler->OnDataRead(Data::Basic::MultiValue::MV_NULL);
		return;
	}
	auto name = GenerateName(tloc.value.unsignedValue, stream->GetName());
	Data::Sheet *sheet = new Data::Sheet(infraType, tloc.value.unsignedValue, size, name);
	SheetManager::GetInstance().RegisterSheet(stream, sheet);
	dataHandler->OnDataRead(sheet->GetName());
}

void mule::SheetReference::Write(xybase::Stream *stream, mule::Data::Basic::Type::FileHandler *fileHandler)
{
	auto &nameValue = fileHandler->OnDataWrite();
	auto tloc = mule::Data::Basic::ContextManager::GetInstance().GetVariable(locCacheName);
	auto size = sizeDefined;
	if (sizeDefined == (size_t)-1)
	{
		auto &tsiz = mule::Data::Basic::ContextManager::GetInstance().GetVariable(sizeCacheName);
		if ((!tloc.IsType(Data::Basic::MultiValue::MVT_UINT) && !tloc.IsType(Data::Basic::MultiValue::MVT_INT))
			|| (!tsiz.IsType(Data::Basic::MultiValue::MVT_UINT) && !tsiz.IsType(Data::Basic::MultiValue::MVT_INT))) return;
		size = tsiz.value.unsignedValue;
	}

	if (tloc.value.unsignedValue == 0 || size == 0)
	{
		return;
	}

	if (nameValue.IsType(Data::Basic::MultiValue::MVT_NULL))
	{
		if (size != 0)
		{
			logger.Warn(L"Null reference against non-zero size! ptr={}, size={}", tloc.value.unsignedValue, size);
			logger.Warn(L"Ill formed item is ignored! (When process {}[{}])",
				xybase::string::to_wstring(Configuration::GetInstance().GetString(u"mule.data.sheet.name")),
				Configuration::GetInstance().GetSigned(u"mule.data.sheet.index"));
		}
		return;
	}
	if (!nameValue.IsType(Data::Basic::MultiValue::MVT_STRING))
	{
		throw xybase::RuntimeException(L"Multivalue type does not correct.", 100);
	}
	auto &name = *nameValue.value.stringValue;

	Data::Sheet *sheet = new Data::Sheet(infraType, tloc.value.unsignedValue, size, name);
	SheetManager::GetInstance().RegisterSheet(stream, sheet);
}

mule::Data::Basic::Type *mule::SheetReference::SheetReferenceCreator::DoCreateObject(const std::u16string &info)
{
	return DoCreateObject(info, {});
}

mule::Data::Basic::Type *mule::SheetReference::SheetReferenceCreator::DoCreateObject(const std::u16string &info, const std::map<std::u16string, std::u16string> &metainfo)
{
	size_t splt = info.find_last_of('@');
	if (splt == std::u16string::npos)
		return nullptr;

	auto infra = Data::TypeManager::GetInstance().GetOrCreateType(info.substr(0, splt));
	if (infra == nullptr) return nullptr;

	SheetReference *ret = new SheetReference();

	ret->infraType = infra;

	// size cache:
	size_t sizeStart = info.find_first_of('(', splt);
	if (sizeStart == std::u16string::npos || !info.ends_with(')'))
	{
		// no size information, all str behind @ is loc cache name
		ret->locCacheName = info.substr(splt + 1);
		// if no size information specified, set it to 1 by default
		ret->sizeDefined = 1;
	}
	else
	{
		// size information found, loc cache name between @ and (
		ret->locCacheName = info.substr(splt + 1, sizeStart - splt - 1);
		// size information in ( and )
		ret->sizeCacheName = info.substr(sizeStart + 1, info.size() - sizeStart - 2);
		// start with digital, not a valid name. assume it is a constant number
		if (ret->sizeCacheName[0] >= '0' && ret->sizeCacheName[0] <= '9')
			ret->sizeDefined = xybase::string::stoi<char16_t>(ret->sizeCacheName);
		else
			ret->sizeDefined = -1;
	}

	if (metainfo.contains(u"naming"))
	{
		ret->namePattern = metainfo.find(u"naming")->second;
	}
	else ret->namePattern = u"${mule.data.sheet.name}/" + infra->GetDataType() + u"_${mule.data.sheet.index}";

	return ret;
}
