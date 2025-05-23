#include "SystemOperation.h"

#include <clocale>
#include <iostream>
#include <xystring.h>
#include <Data/Structure.h>
#include <Data/TypeManager.h>
#include <Mule.h>
#include "../LuaHost.h"
#include <Configuration.h>

mule::Logger mule::Lua::Api::logger {"<LuaSystemApi>"};

int mule::Lua::Api::Confirm(std::u8string word)
{
    std::wstring cmp;
    std::wcin >> cmp;
    return word == xybase::string::to_utf8(cmp) ? 0 : -1;
}

int mule::Lua::Api::PrintPlugins()
{
    logger.Info(mule::Mule::GetInstance().ShowPlugins());
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
    if (mv.IsType(mule::Data::Basic::MultiValue::MVT_NULL))
    {
        try
        {
            return mule::Configuration::GetInstance().GetVariable(xybase::string::to_utf16(name).c_str());
        }
        catch (Configuration::ConfigurationNotFoundException)
        {
            return mule::Data::Basic::MultiValue::MV_NULL;
        }
    }
    mule::Configuration::GetInstance().SetVariable(xybase::string::to_utf16(name).c_str(), mv);

    return 0;
}

int mule::Lua::Api::EraseConfiguration(std::u8string name)
{
    mule::Configuration::GetInstance().ResetVariable(xybase::string::to_utf16(name).c_str());
    return 0;
}

int mule::Lua::Api::DefineStructure(std::u8string name, mule::Data::Basic::MultiValue def)
{
    using mule::Data::Basic::MultiValue;
    if (!def.IsType(MultiValue::MVT_MAP)) return -10;

    mule::Data::Structure *str = new mule::Data::Structure(xybase::string::to_utf16(name));
    for (auto &pair : *def.value.mapValue)
    {
        if (!pair.second.IsType(MultiValue::MVT_STRING))
        {
            delete str;
            return -11;
        }
        size_t split = pair.second.value.stringValue->find_first_of(':');
        std::u16string fldName = pair.second.value.stringValue->substr(0, split);
        std::u16string type = pair.second.value.stringValue->substr(split + 1);
        mule::Data::Basic::Type *fldType = mule::Data::TypeManager::GetInstance().GetOrCreateType(type);
        if (fldType == nullptr)
        {
            logger.Error(L"Failed to create type {}.", xybase::string::to_wstring(type));
            delete str;
            return -12;
        }
        str->AppendField(fldName, fldType);
    }
    mule::Data::TypeManager::GetInstance().RegisterObject(str, xybase::string::to_utf16(name));
    return 0;
}

void mule::Lua::Api::RegisterSystemOperations()
{
    LuaHost::GetInstance().RegisterFunction("confirm", Confirm);
    LuaHost::GetInstance().RegisterFunction("pplugin", PrintPlugins);
    LuaHost::GetInstance().RegisterFunction("lplugin", LoadPlugin);
    LuaHost::GetInstance().RegisterFunction("log", Log);
    LuaHost::GetInstance().RegisterFunction("config", Configuration);
    LuaHost::GetInstance().RegisterFunction("config_erase", EraseConfiguration);
    LuaHost::GetInstance().RegisterFunction("define", DefineStructure);
}
