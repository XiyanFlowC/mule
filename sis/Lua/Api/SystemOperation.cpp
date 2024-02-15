#include "SystemOperation.h"

#include <clocale>
#include <iostream>
#include <xystring.h>
#include <Mule.h>
#include "../LuaHost.h"
#include <Configuration.h>

mule::Logger mule::Lua::Api::logger {"<LuaSystemApi>", LoggerConfig::GetInstance().GetLogLevel()};

int mule::Lua::Api::Confirm(std::u8string word)
{
    std::wstring cmp;
    std::wcin >> cmp;
    return word == xybase::string::to_utf8(cmp) ? 0 : -1;
}

int mule::Lua::Api::PrintPlugins()
{
    mule::Mule::GetInstance().PrintPlugins();
    return 0;
}

int mule::Lua::Api::Log(std::u8string msg)
{
    logger.Info(xybase::string::to_wstring(((char8_t *)msg.c_str())));
    return 0;
}

int mule::Lua::Api::LoadPlugin(std::u8string path)
{
    Mule::GetInstance().LoadPlugin(xybase::string::to_utf16(path).c_str());
    return 0;
}

mule::Data::Basic::MultiValue mule::Lua::Api::Configuration(std::u8string name, mule::Data::Basic::MultiValue mv)
{
    if (mv.IsType(mule::Data::Basic::MultiValue::MVT_NULL)) return mule::Configuration::GetInstance().GetVariable(xybase::string::to_utf16(name).c_str());
    mule::Configuration::GetInstance().SetVariable(xybase::string::to_utf16(name).c_str(), mv);
    return 0;
}

void mule::Lua::Api::RegisterSystemOperations()
{
    LuaHost::GetInstance().RegisterFunction("confirm", Confirm);
    LuaHost::GetInstance().RegisterFunction("pplugin", PrintPlugins);
    LuaHost::GetInstance().RegisterFunction("lplugin", LoadPlugin);
    LuaHost::GetInstance().RegisterFunction("log", Log);
    LuaHost::GetInstance().RegisterFunction("config", Configuration);
}
