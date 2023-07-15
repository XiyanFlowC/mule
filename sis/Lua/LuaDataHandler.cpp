#include "LuaDataHandler.h"

using namespace mule::Lua;
using namespace mule::Data::Basic;

void mule::Lua::LuaDataHandler::OnRealmEnter(Object *realm, std::string name)
{
}

void mule::Lua::LuaDataHandler::OnRealmExit(Object *realm, std::string name)
{
}

void mule::Lua::LuaDataHandler::OnDataRead(const MultiValue &value)
{
}

MultiValue mule::Lua::LuaDataHandler::OnDataWrite()
{
	return MultiValue();
}

void mule::Lua::LuaDataHandler::OnRealmEnter(Object *realm, int idx)
{
}

void mule::Lua::LuaDataHandler::OnRealmExit(Object *realm, int idx)
{
}
