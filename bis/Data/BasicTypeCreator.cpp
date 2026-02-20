#include "BasicTypeCreator.h"
#include "Basic/Integer.h"
#include "Basic/Dummy.h"
#include "Basic/Double.h"
#include "Basic/Float.h"
#include "Basic/String.h"
#include "Basic/UString.h"
#include "Basic/Align.h"

#include <xystring.h>

using namespace mule::Data::Basic;

Type *mule::Data::BasicTypeCreator::DoCreateObject(const std::u16string &info)
{
	return DoCreateObject(info, {});
}

Type *mule::Data::BasicTypeCreator::DoCreateObject(const std::u16string &info, const std::map<std::u16string, std::u16string> &metainfo)
{
	BasicType *ret = nullptr;
	if (info.starts_with(u"int"))
	{
		int size = (int)xybase::string::stoi(info.substr(3));
		if (size != 8 && size != 16 && size != 32 && size != 64)
		{
			return nullptr;
		}

		ret = new Integer(size, false);
	}
	else if (info.starts_with(u"dum"))
	{
		int size = (int)xybase::string::stoi(info.substr(3));
		if (size != 8 && size != 16 && size != 32 && size != 64)
		{
			return nullptr;
		}

		ret = new Dummy(size);
	}
	else if (info.starts_with(u"uint"))
	{
		int size = (int)xybase::string::stoi(info.substr(4));
		if (size != 8 && size != 16 && size != 32 && size != 64)
		{
			return nullptr;
		}

		ret = new Integer(size, true);
	}
	else if (info == u"align")
	{
		int size = (int)xybase::string::stoi(info.substr(5));
		if (size & (size - 1))
		{
			return nullptr;
		}
			
		ret = new Align(size);
	}
	else if (info == u"float")
	{
		ret = new Float();
	}
	else if (info == u"double")
	{
		ret = new Double();
	}
	else if (info == u"string")
	{
		ret = new String();
	}
	else if (info == u"u8string")
	{
		ret = new UString<char8_t>();
	}
	else if (info == u"u16string")
	{
		ret = new UString<char16_t>();
	}
	else if (info == u"u32string")
	{
		ret = new UString<char32_t>();
	}
	else
		return ret;

	auto &&itr = metainfo.find(u"cache");
	if (itr != metainfo.end())
	{
		ret->cacheVariableName = itr->second;
	}

	itr = metainfo.find(u"constraint");
	if (itr != metainfo.end())
	{
		auto &&cons = itr->second;
		auto condition = cons.substr(0, cons.find(':'));
		ret->comparator = MultiValue::Parse(cons.substr(cons.find(':') + 1));
		if (condition == u"lt") ret->constraintType = BasicType::BTCT_LT;
		if (condition == u"nlt") ret->constraintType = BasicType::BTCT_NLT;
		if (condition == u"gt") ret->constraintType = BasicType::BTCT_GT;
		if (condition == u"ngt") ret->constraintType = BasicType::BTCT_NGT;
		if (condition == u"eq") ret->constraintType = BasicType::BTCT_EQ;
		if (condition == u"neq") ret->constraintType = BasicType::BTCT_NEQ;
	}

	return ret;
}
