#include "BasicTypeCreator.h"

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
	else if (info.starts_with(u"uint"))
	{
		int size = (int)xybase::string::stoi(info.substr(4));
		if (size != 8 && size != 16 && size != 32 && size != 64)
		{
			return nullptr;
		}

		ret = new Integer(size, true);
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
	else
		return ret;

	auto &&itr = metainfo.find(u"cache");
	if (itr != metainfo.end())
	{
		ret->cacheVariableName = itr->second;
	}

	return ret;
}
