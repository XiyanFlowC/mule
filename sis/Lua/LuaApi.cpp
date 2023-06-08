#include "LuaApi.h"

using namespace mule::Lua;

int regLuaHandler_c(lua_State *L)
{
	return 0;
}


LuaApi::LuaApi()
{
}

LuaApi &mule::Lua::LuaApi::GetInstance()
{
	static LuaApi _inst;
	return _inst;
}

void mule::Lua::LuaApi::Init()
{
	LuaHost::GetInstance().RegisterFunction("regLuaHandler_c", regLuaHandler_c);
}

