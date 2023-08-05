#include "LuaApi.h"

using namespace mule::Lua;

int regLuaHandler_c(lua_State *L)
{
	return 0;
}

void mule::Lua::LuaApi::Init()
{
	LuaHost::GetInstance().RegisterFunction("regLuaHandler_c", regLuaHandler_c);
}

