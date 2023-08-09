#include "LuaApi.h"

using namespace mule::Lua;

int regLuaHandler_c(lua_State *L)
{
	return 0;
}

mule::Lua::LuaApi::LuaApi()
{
	Init();
}

void mule::Lua::LuaApi::Init()
{
	auto &lh = LuaHost::GetInstance();
}

