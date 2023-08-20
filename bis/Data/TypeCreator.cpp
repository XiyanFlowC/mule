#include "TypeCreator.h"

#include "Basic/BasicType.h"
#include "Basic/Integer.h"
#include "Basic/Double.h"
#include "Basic/Float.h"
#include "Basic/String.h"
#include <xystring.h>

using namespace mule::Data;
using namespace mule::Data::Basic;

mule::Data::TypeCreator::~TypeCreator()
{
}

TypeCreator::TypeCreator()
{
	nextCreator = nullptr;
}

TypeCreator::TypeCreator(TypeCreator* next)
{
	nextCreator = next;
}

TypeCreator::TypeCreator(TypeCreator& pattern)
{
	this->nextCreator = pattern.nextCreator;
}

Type* TypeCreator::CreateType(std::u16string info)
{
	Type* result = DoCreateObject(info);
	if (result != nullptr) return result;

	if (nextCreator != nullptr) return nextCreator->CreateType(info);
	return nullptr;
}

Type *TypeCreator::CreateType(std::u16string info, const std::map<std::u16string, std::u16string> & metainfo)
{
	Type *result = DoCreateObject(info);
	if (result != nullptr) return result;

	if (nextCreator != nullptr) return nextCreator->CreateType(info);
	return nullptr;
}

Basic::Type *mule::Data::TypeCreator::DoCreateObject(std::u16string info, const std::map<std::u16string, std::u16string> & metainfo)
{
	return DoCreateObject(info);
}

Type *mule::Data::BasicFieldCreator::DoCreateObject(std::u16string info)
{
	return DoCreateObject(info, {});
}

Basic::Type *mule::Data::BasicFieldCreator::DoCreateObject(std::u16string info, const std::map<std::u16string, std::u16string> &metainfo)
{
	BasicType *ret = nullptr;
	if (info.starts_with(u"int"))
	{
		int size = xybase::string::stoi(info.substr(3));
		if (size != 8 && size != 16 && size != 32 && size != 64)
		{
			return nullptr;
		}

		ret = new Integer(size, false);
	}
	else if (info.starts_with(u"uint"))
	{
		int size = xybase::string::stoi(info.substr(4));
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
