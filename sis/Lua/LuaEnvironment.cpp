#include "LuaEnvironment.h"

#include <Mule.h>

#include "Api/ContainerOperation.h"
#include "Api/StreamOperation.h"

using namespace mule::Lua;

int regLuaHandler_c(lua_State *L)
{
	return 0;
}

mule::Lua::LuaEnvironment::LuaEnvironment()
{
	Init();
}

xybase::Stream *mule::Lua::LuaEnvironment::GetStream(int idx)
{
	return streams[idx];
}

int mule::Lua::LuaEnvironment::SetStream(xybase::Stream *stream)
{
	streams[streamd++] = stream;
	return 0;
}

void mule::Lua::LuaEnvironment::CloseStream(int idx)
{
	auto &&stream = streams[idx];
	stream->Close();
	delete stream;
}

LuaEnvironment &mule::Lua::LuaEnvironment::GetInstance()
{
	static LuaEnvironment _inst;
	return _inst;
}

void mule::Lua::LuaEnvironment::Init()
{
	Api::RegisterContainerOperationFunctions();
	Api::RegisterStreamOperationFunctions();
}

