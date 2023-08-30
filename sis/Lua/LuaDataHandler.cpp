#include "LuaDataHandler.h"

using namespace mule::Lua;
using namespace mule::Data::Basic;

void mule::Lua::LuaDataHandler::OnRealmEnter(Type *realm, const std::u16string& name)
{
}

void mule::Lua::LuaDataHandler::OnRealmExit(Type *realm, const std::u16string& name)
{
}

void mule::Lua::LuaDataHandler::OnDataRead(const MultiValue &value)
{
}

MultiValue mule::Lua::LuaDataHandler::OnDataWrite()
{
	return MultiValue();
}

void mule::Lua::LuaDataHandler::OnRealmEnter(Type *realm, int idx)
{
}

void mule::Lua::LuaDataHandler::OnRealmExit(Type *realm, int idx)
{
}
