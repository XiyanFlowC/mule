#include "TypeCreator.h"

using namespace mule::Data;
using namespace mule::Data::Basic;

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

Type* TypeCreator::GetOrCreateObject(std::string info)
{
	Type* result = DoCreateObject(info);
	if (result != nullptr) return result;

	if (nextCreator != nullptr) return nextCreator->GetOrCreateObject(info);
	return nullptr;
}

Type *mule::Data::BasicFieldCreator::DoCreateObject(std::string info)
{
	Type *ret = nullptr;
	if (info.starts_with("int"))
	{
		int size = atoi(info.substr(3).c_str());
		if (size != 8 && size != 16 && size != 32 && size != 64)
		{
			return nullptr;
		}

		ret = new Integer(size, false);
	}
	else if (info.starts_with("uint"))
	{
		int size = atoi(info.substr(4).c_str());
		if (size != 8 && size != 16 && size != 32 && size != 64)
		{
			return nullptr;
		}

		ret = new Integer(size, true);
	}
	else if (info == "float")
	{
		ret = new Float();
	}
	else if (info == "double")
	{
		ret = new Double();
	}
	else if (info == "string")
	{
		ret = new String();
	}
	return ret;
}
