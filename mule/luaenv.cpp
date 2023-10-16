#include "luaenv.h"

#include <Storage/DataManager.h>
#include <TextStream.h>
#include <cstdio>
#include <Mule.h>
#include <Lua/LuaEnvironment.h>
#include <VirtualFileSystem.h>

using mule::Lua::LuaEnvironment;
using mule::Mule;

using namespace mule::Data::Basic;
using namespace mule::Data;
using namespace mule::Storage;

int loadDefine(std::string define)
{
    if (define.empty())
    {
        return -1;
    }

    try
    {
        ResourceManager::GetInstance().LoadDefinition(define);
    }
    catch (xybase::Exception &ex)
    {
        fprintf(stderr, "[EX] loadDefine: %s\n", ex.what());
        return -2;
    }

    return 0;
}

int SaveMemory()
{
    ShiftableString::MemoryManager::GetInstance().SaveFreeSpace();
    return 0;
}

#include <Logger.h>

mule::Logger luaenvLogger{"<luaenv>", 0};

int ExportSheet(int streamId, std::string handler, std::string type, std::string tableName, size_t offset, int length)
{
    std::u16string utype = xybase::string::to_utf16(type), utbl = xybase::string::to_utf16(tableName);

    xybase::Stream *stream = LuaEnvironment::GetInstance().GetStream(streamId);
    if (stream == nullptr) return -10;

    mule::Data::Basic::Type::DataHandler *hnd = Mule::GetInstance().GetDataHandler(xybase::string::to_utf16(handler).c_str());
    if (hnd == nullptr) return -11;
    auto outStream = new xybase::TextStream(Configuration::GetInstance().SheetsDir + tableName + "." + handler, std::ios::out);
    hnd->SetOutStream(outStream);

    mule::Data::Table *target = new mule::Data::Table(mule::Data::TypeManager::GetInstance().GetType(utype), utbl, length, offset);
    if (target == nullptr) return -12;

    luaenvLogger.Info(L"Exporting {}", xybase::string::to_wstring(tableName));
    target->Read(stream, hnd);
    delete target;
    delete hnd;
    delete outStream;
}

int ImportSheet(int streamId, std::string handler, std::string type, std::string tableName, size_t offset, int length)
{
    std::u16string utype = xybase::string::to_utf16(type), utbl = xybase::string::to_utf16(tableName);

    xybase::Stream *stream = LuaEnvironment::GetInstance().GetStream(streamId);
    if (stream == nullptr) return -10;

    mule::Data::Basic::Type::FileHandler *hnd = Mule::GetInstance().GetFileHandler(xybase::string::to_utf16(handler).c_str());
    if (hnd == nullptr) return -11;
    auto inStream = new xybase::TextStream(Configuration::GetInstance().SheetsDir + tableName + "." + handler, std::ios::in);
    hnd->SetInStream(inStream);

    mule::Data::Table *target = new mule::Data::Table(mule::Data::TypeManager::GetInstance().GetType(utype), utbl, length, offset);
    if (target == nullptr) return -12;

    luaenvLogger.Info(L"Importing {}", xybase::string::to_wstring(tableName));
    target->Write(stream, hnd);
    delete target;
    delete hnd;
    delete inStream;
}

void InitialiseLuaEnvironment()
{
    auto &lua = mule::Lua::LuaHost::GetInstance();
    // configuration
    lua.RegisterFunction("loaddef", loadDefine);
    lua.RegisterFunction("savemem", SaveMemory);

    lua.RegisterFunction("exportsht", ExportSheet);
    lua.RegisterFunction("importsht", ImportSheet);
}
