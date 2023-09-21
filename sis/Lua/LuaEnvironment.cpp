#include "LuaEnvironment.h"

using namespace mule::Lua;

int regLuaHandler_c(lua_State *L)
{
	return 0;
}

mule::Lua::LuaEnvironment::LuaEnvironment()
{
	Init();
}

LuaEnvironment &mule::Lua::LuaEnvironment::GetInstance()
{
	static LuaEnvironment _inst;
	return _inst;
}

void mule::Lua::LuaEnvironment::Init()
{
	auto &lh = LuaHost::GetInstance();
}

