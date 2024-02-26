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

mule::Lua::LuaEnvironment::~LuaEnvironment()
{
	for (auto ritr = streams.rbegin(); ritr != streams.rend(); ++ritr)
	{
		ritr->second->Close();
		delete ritr->second;
	}
}

xybase::Stream *mule::Lua::LuaEnvironment::GetStream(int idx)
{
	if (streams.contains(idx))
		return streams[idx];
	else
		return nullptr;
}

int mule::Lua::LuaEnvironment::SetStream(xybase::Stream *stream)
{
	if (stream == nullptr)
		throw xybase::InvalidParameterException(L"stream", L"Null Pointer!!!", 95585);
	int sd = streamd++;
	streams[sd] = stream;
	return sd;
}

void mule::Lua::LuaEnvironment::CloseStream(int idx)
{
	if (!streams.contains(idx)) throw xybase::InvalidParameterException(L"idx", L"Specified fd not found.", 95594);
	auto &&stream = streams[idx];
	stream->Close();
	delete stream;
	streams.erase(idx);
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

