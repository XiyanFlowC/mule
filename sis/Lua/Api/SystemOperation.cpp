#include "SystemOperation.h"

#include <clocale>
#include <iostream>
#include <xystring.h>
#include <Mule.h>
#include "../LuaHost.h"
#include <Configuration.h>

mule::Logger mule::Lua::Api::logger {"<LuaSystemApi>", 0};

std::string mule::Lua::Api::SetLocale(std::string locale)
{
	/*logger.Info(L"Set global locale to {}...", xybase::string::to_wstring(locale));
    char *ret = setlocale(LC_ALL, locale.c_str());
    if (ret == nullptr)
    {
        logger.Error(L"Failed to set locale.");
        return "!Failed!";
    }
	logger.Info(L"Success.");*/
    logger.Warn(L"SetLocale was muted. No changes will be made.");
    auto ret = setlocale(LC_ALL, nullptr);
    if (ret == nullptr) return "";
    return ret;
}

int mule::Lua::Api::Confirm(std::string word)
{
    std::string cmp;
    std::cin >> cmp;
    return word == cmp ? 0 : -1;
}

int mule::Lua::Api::PrintPlugins()
{
    mule::Mule::GetInstance().PrintPlugins();
    return 0;
}

int mule::Lua::Api::Log(std::string msg)
{
    logger.Info(xybase::string::to_wstring(msg));
    return 0;
}

int mule::Lua::Api::LoadPlugin(std::string path)
{
    Mule::GetInstance().LoadPlugin(xybase::string::to_utf16(path).c_str());
    return 0;
}

mule::Data::Basic::MultiValue mule::Lua::Api::Configuration(std::string name, mule::Data::Basic::MultiValue mv)
{
    if (mv.IsType(mule::Data::Basic::MultiValue::MVT_NULL)) return mule::Configuration::GetInstance().GetVariable(xybase::string::to_utf16(name).c_str());
    mule::Configuration::GetInstance().SetVariable(xybase::string::to_utf16(name).c_str(), mv);
    return 0;
}

void mule::Lua::Api::RegisterSystemOperations()
{
    LuaHost::GetInstance().RegisterFunction("setlocale", SetLocale);
    LuaHost::GetInstance().RegisterFunction("confirm", Confirm);
    LuaHost::GetInstance().RegisterFunction("pplugin", PrintPlugins);
    LuaHost::GetInstance().RegisterFunction("lplugin", LoadPlugin);
    LuaHost::GetInstance().RegisterFunction("log", Log);
    LuaHost::GetInstance().RegisterFunction("config", Configuration);
}
