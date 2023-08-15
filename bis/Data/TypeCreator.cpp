#include "TypeCreator.h"
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

Type* TypeCreator::GetOrCreateObject(std::u16string info)
{
	Type* result = DoCreateObject(info);
	if (result != nullptr) return result;

	if (nextCreator != nullptr) return nextCreator->GetOrCreateObject(info);
	return nullptr;
}

Type *mule::Data::BasicFieldCreator::DoCreateObject(std::u16string info)
{
	Type *ret = nullptr;
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
	return ret;
}
