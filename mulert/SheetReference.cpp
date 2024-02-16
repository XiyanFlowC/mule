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
	streamName = xybase::string::replace<char16_t>(streamName, u"/", u"_");
	streamName = xybase::string::replace<char16_t>(streamName, u"\\", u"_");
	streamName = xybase::string::replace<char16_t>(streamName, u":", u"_");
	streamName = xybase::string::replace<char16_t>(streamName, u"?", u"_");
	streamName = xybase::string::replace<char16_t>(streamName, u"|", u"_");
	streamName = xybase::string::replace<char16_t>(streamName, u"*", u"_");
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

size_t mule::SheetReference::Size() const
{
	return size_t(4);
}

std::u16string mule::SheetReference::GetDataType() const
{
	return u"string";
}

void mule::SheetReference::Read(xybase::Stream *stream, mule::Data::Basic::Type::DataHandler *dataHandler)
{
	auto loc = stream->Tell();
	auto tloc = mule::Data::Basic::ContextManager::GetInstance().GetVariable(locCacheName);
	auto tsiz = mule::Data::Basic::ContextManager::GetInstance().GetVariable(sizeCacheName);
	if ((!tloc.IsType(Data::Basic::MultiValue::MVT_UINT) && !tloc.IsType(Data::Basic::MultiValue::MVT_INT))
		|| (!tsiz.IsType(Data::Basic::MultiValue::MVT_UINT) && !tsiz.IsType(Data::Basic::MultiValue::MVT_INT))) return;

	Data::Sheet *sheet = new Data::Sheet(infraType, tloc.value.unsignedValue, tsiz.value.unsignedValue, GenerateName(tloc.value.unsignedValue, stream->GetName()));
	SheetManager::GetInstance().RegisterSheet(stream, sheet);
	dataHandler->OnDataRead(sheet->GetName());
}

void mule::SheetReference::Write(xybase::Stream *stream, mule::Data::Basic::Type::FileHandler *fileHandler)
{
	auto expectedName = fileHandler->OnDataWrite();
	auto loc = stream->Tell();
	auto tloc = mule::Data::Basic::ContextManager::GetInstance().GetVariable(locCacheName);
	auto tsiz = mule::Data::Basic::ContextManager::GetInstance().GetVariable(sizeCacheName);
	if ((!tloc.IsType(Data::Basic::MultiValue::MVT_UINT) && !tloc.IsType(Data::Basic::MultiValue::MVT_INT))
		|| (!tsiz.IsType(Data::Basic::MultiValue::MVT_UINT) && !tsiz.IsType(Data::Basic::MultiValue::MVT_INT))) return;

	auto name = stream->GetName();
	Data::Sheet *sheet = new Data::Sheet(infraType, tloc.value.unsignedValue, tsiz.value.unsignedValue, GenerateName(tloc.value.unsignedValue, name));
	if (name != expectedName)
	{
		throw xybase::InvalidParameterException(L"name", L"Sheet name mismatch.", __LINE__);
	}
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

	size_t sizeStart = info.find_first_of('(', splt);
	if (sizeStart == std::u16string::npos || !info.ends_with(')')) return nullptr;

	auto infra = Data::TypeManager::GetInstance().GetOrCreateType(info.substr(0, splt));
	if (infra == nullptr) return nullptr;

	SheetReference *ret = new SheetReference();

	ret->infraType = infra;
	ret->locCacheName = info.substr(splt + 1, sizeStart - splt - 1);
	ret->sizeCacheName = info.substr(sizeStart + 1, info.size() - sizeStart - 2);
	if (metainfo.contains(u"naming"))
	{
		ret->namePattern = metainfo.find(u"naming")->second;
	}
	else ret->namePattern = u"${mule.data.sheet.name}/" + infra->GetDataType() + u"_${mule.data.sheet.index}";

	return ret;
}
