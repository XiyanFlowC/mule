#include "LuaEnvironment.h"

#include <Mule.h>
#include <VirtualFileSystem.h>

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
	//mule::VirtualFileSystem::GetInstance().Shutdown();

	for (auto ritr = streams.rbegin(); ritr != streams.rend(); ++ritr)
	{
		ritr->second->Close();
		delete ritr->second;
	}
}

xybase::Stream *mule::Lua::LuaEnvironment::GetStream(int idx)
{
	logger.Debug(L"请求流{}", idx);
	if (streams.contains(idx))
	{
		logger.Debug(L"有效流->{}", xybase::string::to_wstring(streams[idx]->GetName()));
		return streams[idx];
	}
	else
		return nullptr;
}

int mule::Lua::LuaEnvironment::SetStream(xybase::Stream *stream)
{
	if (stream == nullptr)
		throw xybase::InvalidParameterException(L"stream", L"Null Pointer!!!", 95585);
	int sd = streamd++;
	streams[sd] = stream;
#ifndef NDEBUG
	logger.Debug(L"登录流{}->{}", xybase::string::to_wstring(stream->GetName()), sd);
	stream->OnClose += [](xybase::Stream *sender) -> void 
		{
			LoggerBase::GetInstance().Debug(L"LUA 宿主环境：关闭流监听事件触发");
			LoggerBase::GetInstance().Debug(xybase::string::to_wstring(sender->GetName()));
		};
#endif // !NDEBUG

	return sd;
}

void mule::Lua::LuaEnvironment::CloseStream(int idx)
{
	if (!streams.contains(idx)) throw xybase::InvalidParameterException(L"idx", L"Specified fd not found.", 95594);
	auto &&stream = streams[idx];
	logger.Debug(L"注销流{}->{}", idx, xybase::string::to_wstring(stream->GetName()));
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

