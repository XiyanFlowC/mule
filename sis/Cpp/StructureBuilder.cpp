#include "StructureBuilder.h"

using namespace mule::Data::Basic;
using namespace mule::Data;

mule::Cpp::StructureBuilder::StructureBuilder(std::u16string name)
	: name(name)
{
}

mule::Data::Structure *mule::Cpp::StructureBuilder::Build()
{
	mule::Data::Structure *ret = new mule::Data::Structure(name);
	for (std::tuple<std::u16string, std::u16string> &def : defList)
	{
		ret->AppendField(std::get<1>(def), TypeManager::GetInstance().GetOrCreateObject( std::get<0>(def) ));
	}
	TypeManager::GetInstance().RegisterObject(ret, name);
	return ret;
}

void mule::Cpp::StructureBuilder::NewStruct(std::u16string name)
{
	this->name = name;
	defList.clear();
}

void mule::Cpp::StructureBuilder::AppendField(std::u16string type, std::u16string name)
{
	defList.push_back(std::tuple<std::u16string, std::u16string>(type, name));
}
