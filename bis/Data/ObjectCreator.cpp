#include "ObjectCreator.h"

using namespace mule::Data;
using namespace mule::Data::Basic;

ObjectCreator::ObjectCreator()
{
	nextCreator = nullptr;
}

ObjectCreator::ObjectCreator(ObjectCreator* next)
{
	nextCreator = next;
}

ObjectCreator::ObjectCreator(ObjectCreator& pattern)
{
	this->nextCreator = pattern.nextCreator;
}

Object* ObjectCreator::GetOrCreateObject(std::string info)
{
	Object* result = DoCreateObject(info);
	if (result != nullptr) return result;

	if (nextCreator != nullptr) return nextCreator->GetOrCreateObject(info);
	return nullptr;
}

Object *mule::Data::BasicFieldCreator::DoCreateObject(std::string info)
{
	Object *ret = nullptr;
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
