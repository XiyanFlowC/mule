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

Type* TypeCreator::CreateType(const std::u16string & info)
{
	Type* result = DoCreateObject(info);
	if (result != nullptr) return result;

	if (nextCreator != nullptr) return nextCreator->CreateType(info);
	return nullptr;
}

Type *TypeCreator::CreateType(const std::u16string & info, const std::map<std::u16string, std::u16string> & metainfo)
{
	Type *result = DoCreateObject(info, metainfo);
	if (result != nullptr) return result;

	if (nextCreator != nullptr) return nextCreator->CreateType(info, metainfo);
	return nullptr;
}

Basic::Type *mule::Data::TypeCreator::DoCreateObject(const std::u16string &info, const std::map<std::u16string, std::u16string> & metainfo)
{
	return DoCreateObject(info);
}
