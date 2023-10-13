#include "luaenv.h"

#include <Storage/DataManager.h>
#include <TextStream.h>
#include <cstdio>
#include <Mule.h>
#include <VirtualFileSystem.h>

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

void InitialiseLuaEnvironment(xybase::Stream *stream)
{
    auto &lua = mule::Lua::LuaHost::GetInstance();
    // configuration
    lua.RegisterFunction("loaddef", loadDefine);
    lua.RegisterFunction("savemem", SaveMemory);
}
