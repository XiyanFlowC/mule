#include "mule.h"

using namespace mule::Data::Basic;
using namespace mule::Data;

std::map<int, xybase::Stream *> streams;
int streamd = 0;
std::map<int, MultiValue> values;
int valued = 0;
std::stack<xybase::FileContainer *> containerStack;
std::list<Table *> tables;

int applyContainer(int id, std::string x)
{
    if (x.empty() || id < 0)
    {
        return -1;
    }

    auto &&stream = streams.find(id);
    if (stream == streams.end())
    {
        return -2;
    }

    if (x == "iso")
    {
        containerStack.push(new IsoContainer(stream->second));
        return 0;
    }
    else
    {
        return -3;
    }
}

int popContainer()
{
    if (containerStack.empty())
    {
        return -1;
    }
    if (!containerStack.empty()) containerStack.pop();
    return 0;
}

int openFile(std::string name)
{
    if (containerStack.empty() || name.empty())
    {
        return -1;
    }

    auto ret = containerStack.top()->Open(name, xybase::FileContainer::FOM_READ_WRITE);
    if (ret == nullptr)
    {
        return -1;
    }
    streams[streamd] = ret;

    return streamd++;
}

int closeFile(int fd)
{
    if (containerStack.empty())
    {
        return -1;
    }

    auto &&itr = streams.find(fd);
    if (itr == streams.end())
    {
        return -1;
    }

    itr->second->Close();
    delete itr->second;
    streams.erase(itr);

    return 0;
}

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
    catch (mule::Exception::Exception &ex)
    {
        fprintf(stderr, "[EX] loadDefine: %s\n", ex.what());
        return -2;
    }

    return 0;
}

int applyTable(int fd, std::string name, size_t offset, int size)
{
    if (size <= 0 || offset < 0 || name.empty() || fd < 0)
    {
        return -1;
    }

    auto &&itr = streams.find(fd);
    if (itr == streams.end())
    {
        return -1;
    }

    auto structure = TypeManager::GetInstance().GetObject(name);
    if (structure == nullptr || dynamic_cast<Structure *>(structure) == nullptr)
    {
        return -2;
    }

    Table tbl(dynamic_cast<Structure *>(structure), "", size, offset);
    Mappifier mp{};
    tbl.Read(itr->second, &mp);

    values[valued] = mp.GetMap();

    return valued++;
}

mule::Xml::XmlGenerator<mule::Xml::MvXmlNode> generator;

int saveSheet(int vd, std::string sheetName)
{
    if (sheetName.empty() || vd < 0)
    {
        return -1;
    }

    auto &&itr = values.find(vd);
    if (itr == values.end())
    {
        return -2;
    }
    std::string xml = generator.ToXml(mule::Xml::MvXmlNode(sheetName, itr->second));
    ResourceManager::GetInstance().SaveSheet(sheetName, xml);

    return 0;
}

mule::Xml::XmlParser<mule::Xml::MvXmlNode> sheetParser;

int loadSheet(std::string name)
{
    if (name.empty())
    {
        return -1;
    }

    auto ret = ResourceManager::GetInstance().LoadSheet(name);
    values[valued] = sheetParser.Parse(ret).mv;
    if (!sheetParser.error.empty())
    {
        fputs(sheetParser.error.c_str(), stderr);
    }

    return valued++;
}

MultiValue getSheet(int vd)
{
    if (vd < 0)
    {
        return -1;
    }

    auto &&itr = values.find(vd);
    if (itr == values.end())
    {
        return -1;
    }

    return itr->second;
}

MultiValue listfs()
{
    auto &&lst = containerStack.top()->List();

    MultiValue mv{ MultiValue::MVT_MAP };

    int i = 1;
    for (auto &&item : lst)
    {
        (*mv.value.mapValue)[MultiValue((long long)(i++))] = MultiValue(item);
    }

    return mv;
}

int applyStream(int id, std::string x)
{
    if (x.empty() || id < 0)
    {
        return -1;
    }

    auto &&stream = streams.find(id);
    if (stream == streams.end())
    {
        return -2;
    }

    if (x == "elf")
    {
        streams[streamd] = new ElfStream(stream->second);
        return streamd++;
    }
    else
    {
        return -3;
    }
}

void InitialiseLuaEnvironment(xybase::Stream *stream)
{
    streams[streamd++] = stream;
    auto &lua = mule::Lua::LuaHost::GetInstance();
    lua.RegisterFunction("loaddef", loadDefine);
    lua.RegisterFunction("applyfs", applyContainer);
    lua.RegisterFunction("popfs", popContainer);
    lua.RegisterFunction("open", openFile);
    lua.RegisterFunction("close", closeFile);
    lua.RegisterFunction("applytbl", applyTable);
    lua.RegisterFunction("loadsheet", loadSheet);
    lua.RegisterFunction("savesheet", saveSheet);
    lua.RegisterFunction("getsheet", getSheet);
    lua.RegisterFunction("ls", listfs);
    lua.RegisterFunction("applyfile", applyStream);
}
