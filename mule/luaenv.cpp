#include "luaenv.h"

#include <Storage/DataManager.h>
#include <TextStream.h>
#include <cstdio>
#include <Mule.h>
#include <Lua/LuaEnvironment.h>
#include <VirtualFileSystem.h>
#include "codepage.h"
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

    size_t reppos = utbl.find('/');
    while (reppos != std::u16string::npos)
    {
        utbl = utbl.replace(reppos, 1, u"-");
        reppos = utbl.find('/', reppos);
    }

    xybase::Stream *stream = LuaEnvironment::GetInstance().GetStream(streamId);
    if (stream == nullptr) return -10;

    mule::Data::Basic::Type::DataHandler *hnd = Mule::GetInstance().GetDataHandler(xybase::string::to_utf16(handler).c_str());
    if (hnd == nullptr) return -11;
    auto outStream = new xybase::TextStream(Configuration::GetInstance().SheetsDir + tableName + "." + handler, std::ios::out);
    hnd->SetOutStream(outStream);

    mule::Data::Table *target = new mule::Data::Table(mule::Data::TypeManager::GetInstance().GetOrCreateType(utype), utbl, length, offset);
    if (target == nullptr) return -12;

    luaenvLogger.Info(L"Exporting {}", xybase::string::to_wstring(tableName));
    target->Read(stream, hnd);
    delete target;
    delete hnd;
    delete outStream;
    return 0;
}

int ImportSheet(int streamId, std::string handler, std::string type, std::string tableName, size_t offset, int length)
{
    std::u16string utype = xybase::string::to_utf16(type), utbl = xybase::string::to_utf16(tableName);

    size_t reppos = utbl.find('/');
    while (reppos != std::u16string::npos)
    {
        utbl = utbl.replace(reppos, 1, u"-");
        reppos = utbl.find('/', reppos);
    }

    xybase::Stream *stream = LuaEnvironment::GetInstance().GetStream(streamId);
    if (stream == nullptr) return -10;

    mule::Data::Basic::Type::FileHandler *hnd = Mule::GetInstance().GetFileHandler(xybase::string::to_utf16(handler).c_str());
    if (hnd == nullptr) return -11;
    auto inStream = new xybase::TextStream(Configuration::GetInstance().SheetsDir + tableName + "." + handler, std::ios::in);
    hnd->SetInStream(inStream);

    mule::Data::Table *target = new mule::Data::Table(mule::Data::TypeManager::GetInstance().GetOrCreateType(utype), utbl, length, offset);
    if (target == nullptr) return -12;

    luaenvLogger.Info(L"Importing {}", xybase::string::to_wstring(tableName));
    target->Write(stream, hnd);
    delete target;
    delete hnd;
    delete inStream;
    return 0;
}

int loadMemory(int streamId, int fileId)
{
    xybase::Stream *stream = LuaEnvironment::GetInstance().GetStream(streamId);
    if (stream == nullptr) return -10;
    auto &memory = ShiftableString::MemoryManager::GetInstance().GetMemory(stream);

    FILE *file = DataManager::GetInstance().OpenRaw(fileId);
    if (file == nullptr) return -11;
    int cnt = 0;
    if (!fscanf(file, "%d", &cnt))
    {
        fclose(file);
        return -12;
    }
    for (int i = 0; i < cnt; ++i)
    {
        unsigned long long str, len;
        if (!fscanf(file, "%llX %llu", &str, &len))
        {
            fclose(file);
            return -13;
        }
        memory.RegisterFragment(str, len);
    }
    fclose(file);
    return 0;
}

int cvttxt(int stream, std::string cvt, std::string output, std::string param)
{
    luaenvLogger.Info(L"Converting to {}...", xybase::string::to_wstring(output));
    xybase::TextStream out(Configuration::GetInstance().ResourcesDir + output, std::ios::out);
    Mule::GetInstance().ConvertToText(
        LuaEnvironment::GetInstance().GetStream(stream),
        xybase::string::to_utf16(cvt).c_str(),
        &out,
        xybase::string::to_utf16(param).c_str());
    return 0;
}

int cvtbin(std::string text, std::string cvt, int stream, std::string param)
{
    luaenvLogger.Info(L"Converting from {}...", xybase::string::to_wstring(text));
    xybase::TextStream in(Configuration::GetInstance().ResourcesDir + text, std::ios::in);
    Mule::GetInstance().ConvertToBinary(
        &in,
        xybase::string::to_utf16(cvt).c_str(),
        LuaEnvironment::GetInstance().GetStream(stream),
        xybase::string::to_utf16(param).c_str());
    return 0;
}

int loadcodepage(int code)
{
    if (code == 0)
        xybase::string::set_string_cvt(nullptr, nullptr);

    CodeCvt::GetInstance().Init(DataManager::GetInstance().LoadData(code).GetData());
    return 0;
}

void InitialiseLuaEnvironment()
{
    auto &lua = mule::Lua::LuaHost::GetInstance();
    // configuration
    lua.RegisterFunction("loaddef", loadDefine);
    lua.RegisterFunction("savemem", SaveMemory);
    lua.RegisterFunction("ldmem", loadMemory);

    lua.RegisterFunction("exportsht", ExportSheet);
    lua.RegisterFunction("importsht", ImportSheet);

    lua.RegisterFunction("cvttxt", cvttxt);
    lua.RegisterFunction("cvtbin", cvtbin);

    lua.RegisterFunction("codemap", loadcodepage);
}
