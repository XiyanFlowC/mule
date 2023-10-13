#include "SystemOperation.h"

#include <clocale>
#include <xystring.h>
#include "../LuaHost.h"

mule::Logger mule::Lua::Api::logger {"<LuaSystemApi>", 0};

std::string mule::Lua::Api::SetLocale(std::string locale)
{
	logger.Info(L"Set global locale to {}...", xybase::string::to_wstring(locale));
    char *ret = setlocale(LC_ALL, locale.c_str());
    if (ret == nullptr)
    {
        logger.Error(L"Failed to set locale.");
        return "!Failed!";
    }
	logger.Info(L"Success.");
    return ret;
}

void mule::Lua::Api::RegisterSystemOperations()
{
    LuaHost::GetInstance().RegisterFunction("setlocale", SetLocale);
}
