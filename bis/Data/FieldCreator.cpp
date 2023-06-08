#include "FieldCreator.h"

using namespace mule::Data;
using namespace mule::Data::Basic;

FieldCreator::FieldCreator()
{
	nextCreator = nullptr;
}

FieldCreator::FieldCreator(FieldCreator* next)
{
	nextCreator = next;
}

FieldCreator::FieldCreator(FieldCreator& pattern)
{
	this->nextCreator = pattern.nextCreator;
}

Field* FieldCreator::CreateField(FieldCreatingInfo& info)
{
	Field* result = DoCreateField(info);
	if (result != nullptr) return result;

	if (nextCreator != nullptr) return nextCreator->CreateField(info);
	return nullptr;
}

Field *mule::Data::BasicFieldCreator::DoCreateField(FieldCreatingInfo &info)
{
	Field *ret = nullptr;
	if (info.typeDescriptor.starts_with("int"))
	{
		int size = atoi(info.typeDescriptor.substr(3).c_str());
		if (size != 8 && size != 16 && size != 32 && size != 64)
		{
			return nullptr;
		}

		ret = new IntegerField(size, false);
	}
	else if (info.typeDescriptor.starts_with("uint"))
	{
		int size = atoi(info.typeDescriptor.substr(4).c_str());
		if (size != 8 && size != 16 && size != 32 && size != 64)
		{
			return nullptr;
		}

		ret = new IntegerField(size, true);
	}
	else if (info.typeDescriptor == "float")
	{
		ret = new FloatField();
	}
	else if (info.typeDescriptor == "double")
	{
		ret = new DoubleField();
	}
	return ret;
}
