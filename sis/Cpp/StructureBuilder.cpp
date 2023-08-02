#include "StructureBuilder.h"

using namespace mule::Data::Basic;
using namespace mule::Data;

mule::Cpp::StructureBuilder::StructureBuilder(std::string name)
	: name(name)
{
}

mule::Data::Basic::Structure *mule::Cpp::StructureBuilder::Build()
{
	mule::Data::Basic::Structure *ret = new mule::Data::Basic::Structure(name);
	for (std::tuple<std::string, std::string> &def : defList)
	{
		ret->AppendField(std::get<1>(def), ObjectManager::GetInstance().GetOrCreateObject( std::get<0>(def) ));
	}
	ObjectManager::GetInstance().RegisterObject(ret, name);
	return ret;
}

void mule::Cpp::StructureBuilder::AppendField(std::string type, std::string name)
{
	defList.push_back(std::tuple<std::string, std::string>(type, name));
}
