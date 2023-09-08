#include "luaenv.h"

#include <Data/Storage/DataManager.h>
#include <Cpp/Environment.h>
#include <TextStream.h>
#include <cstdio>

using namespace mule::Data::Basic;
using namespace mule::Data;
using namespace mule::Data::Storage;

std::map<int, xybase::Stream *> streams;
int streamd = 0;
std::map<int, MultiValue> values;
int valued = 0;
std::map<std::string, xybase::FileContainer *> containers;
std::list<Table *> tables;

int mountContainer(int id, std::string type, std::string code)
{
    if (type.empty() || code.empty() || id < 0)
    {
        return -1;
    }

    auto &&stream = streams.find(id);
    if (stream == streams.end())
    {
        return -2;
    }

    if (containers.contains(code))
    {
        return -3;
    }

    auto cont = mule::Cpp::Environment::GetInstance().GetFileContainer(xybase::string::to_utf16(type), stream->second);

    if (cont == nullptr)
    {
        return -4;
    }
    else
    {
        containers[code] = (new mule::Container::IsoContainer(stream->second));
        return 0;
    }
}

int unmountContainer(std::string code)
{
    if (!containers.contains(code))
    {
        return -2;
    }

    delete containers[code];
    containers.erase(code);
    return 0;
}

std::string defaultContainer = "";

int selectContainer(std::string code)
{
    defaultContainer = code;
    return 0;
}

std::string getDefaultContainer()
{
    return defaultContainer;
}

int openFile(std::string name)
{
    xybase::FileContainer *con;
    size_t devIndex = name.find_first_of(':');
    if (devIndex == std::string::npos)
    {
        auto &&itr = containers.find(defaultContainer);
        if (itr == containers.end()) return -2;

        con = itr->second;
    }
    else
    {
        auto &&itr = containers.find(name.substr(0, devIndex));
        if (itr == containers.end()) return -3;

        con = itr->second;
        name = name.substr(devIndex + 1);
    }

    auto ret = con->Open(xybase::string::to_utf16(name), xybase::FileContainer::FOM_READ_WRITE);
    if (ret == nullptr)
    {
        return -1;
    }
    streams[streamd] = ret;

    return streamd++;
}

int closeFile(int fd)
{
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

size_t tellFile(int fd)
{
    auto &&itr = streams.find(fd);
    if (itr == streams.end())
    {
        return -1;
    }

    return itr->second->Tell();
}

int seekFile(int fd, long long offset, int mode)
{
    auto &&itr = streams.find(fd);
    if (itr == streams.end())
    {
        return -1;
    }

    itr->second->Seek(offset, mode);

    return 0;
}

MultiValue readFile(int fd, int size)
{
    auto &&itr = streams.find(fd);
    if (itr == streams.end())
    {
        return -1;
    }

    return -1;
}

std::string writeFile(int fd, MultiValue mv)
{
    return "Not allowed.";
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
    catch (xybase::Exception &ex)
    {
        fprintf(stderr, "[EX] loadDefine: %s\n", ex.what());
        return -2;
    }

    return 0;
}

std::map<int, std::map<std::string, Table *>> tbls;

int registerTable(int fd, std::string name, std::string type, size_t offset, int size)
{
    if (size <= 0 || offset < 0 || name.empty() || type.empty() || fd < 0)
    {
        return -1;
    }

    auto &&itr = streams.find(fd);
    if (itr == streams.end())
    {
        return -1;
    }

    auto structure = TypeManager::GetInstance().GetType(xybase::string::to_utf16(type));
    if (structure == nullptr)
    {
        return -2;
    }

    auto &&it = tbls.find(fd);
    if (it == tbls.end())
    {
        tbls[fd];
    }

    auto &&i = tbls[fd].find(name);
    if (i != tbls[fd].end()) return -3;

    tbls[fd][name] = new Table(structure, xybase::string::to_utf16(name), size, offset);

    return 0;
}

int readTable(int fd, std::string handler)
{
    auto &&titr = tbls.find(fd);
    auto &&sitr = streams.find(fd);
    if (titr == tbls.end() || sitr == streams.end())
    {
        return -2;
    }

    if (handler == "mappifier")
    {
        MultiValue v{ MultiValue::MVT_MAP };
        for (auto &&pair : titr->second)
        {
            Mappifier m;
            std::wcout << L"Now processing " << xybase::string::to_wstring(pair.first) << std::endl;
            pair.second->Read(sitr->second, &m);
            (*v.value.mapValue)[xybase::string::to_utf16(pair.first)] = m.GetMap();
        }
        values[valued++] = v;
    }
    else
    {
        auto proc = mule::Cpp::Environment::GetInstance().GetHandler(xybase::string::to_utf16(handler));

        if (proc == nullptr) return -3;

        for (auto &&pair : titr->second)
        {
            xybase::Stream *stream = new xybase::TextStream(Configuration::GetInstance().SheetsDir + pair.first + "." + handler, 1);
            proc->SetStream(stream);
            std::wcout << L"Processing " << xybase::string::to_wstring(pair.first) << std::endl;
            pair.second->Read(sitr->second, proc);
            stream->Close();
            delete stream;
        }
    }

    return valued - 1;
}

int writeTable(int fd, std::string handler)
{
    auto &&titr = tbls.find(fd);
    auto &&sitr = streams.find(fd);
    if (titr == tbls.end() || sitr == streams.end())
    {
        return -2;
    }

    auto proc = mule::Cpp::Environment::GetInstance().GetHandler(xybase::string::to_utf16(handler));

    if (proc == nullptr) return -3;

    for (auto &&pair : titr->second)
    {
        xybase::Stream *stream = new xybase::TextStream(Configuration::GetInstance().SheetsDir + pair.first + "." + handler, 0);
        proc->SetStream(stream);
        std::wcout << L"Processing " << xybase::string::to_wstring(pair.first) << std::endl;
        pair.second->Write(sitr->second, proc);
        stream->Close();
        delete stream;
    }

    return 0;
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
    for (auto &&sheet : *itr->second.value.mapValue)
    {
        std::string xml = generator.ToXml(mule::Xml::MvXmlNode(u"Sheet", sheet.second));
        ResourceManager::GetInstance().SaveSheet(sheetName + xybase::string::to_string(*sheet.first.value.stringValue), xml);
    }

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

MultiValue listfs(MultiValue dev)
{
    xybase::FileContainer *con {};
    if (dev.IsType(MultiValue::MVT_NULL))
    {
        auto &&itr = containers.find(defaultContainer);
        if (itr == containers.end()) return -2;

        con = itr->second;
    }
    else
    {
        if (!dev.IsType(MultiValue::MVT_STRING))
        {
            return -1;
        }

        auto &&itr = containers.find(xybase::string::to_string(*dev.value.stringValue));
        if (itr == containers.end()) return -2;

        con = itr->second;
    }

    auto &&lst = con->List();

    MultiValue mv{ MultiValue::MVT_MAP };

    int64_t i = 1;
    for (auto &&item : lst)
    {
        (*mv.value.mapValue)[MultiValue(i++)] = MultiValue(item);
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

    auto ret = mule::Cpp::Environment::GetInstance().ApplyStream(xybase::string::to_utf16(x), stream->second);
    if (ret == nullptr) return -3;

    streams[streamd] = ret;
    return streamd;
}

int updateSheet(int vd, MultiValue mv)
{
    if (vd >= valued)
    {
        return -1;
    }

    values[vd] = mv;

    return 0;
}

int newSheet()
{
    values[valued] = MultiValue{};
    return valued++;
}

unsigned int exportFile(int fd)
{
    auto &&it = streams.find(fd);
    if (it == streams.end())
    {
        return 0;
    }

    auto &&stream = it->second;
    stream->Seek(0, SEEK_END);
    size_t length = stream->Tell();
    stream->Seek(0, SEEK_SET);
    char *buffer = new char[length];
    stream->ReadBytes(buffer, length);
    BinaryData bd{ buffer, length, false };
    return DataManager::GetInstance().SaveData(bd);
}

int importFile(int fd, unsigned int id)
{
    auto &&it = streams.find(fd);
    if (it == streams.end())
    {
        return -1;
    }

    BinaryData bd = DataManager::GetInstance().LoadData(id);

    auto &&stream = it->second;
    stream->Seek(0, 0);
    stream->Write(bd.GetData(), bd.GetLength());

    return 0;
}

int SaveMemory()
{
    ShiftableReferrence::MemoryManager::GetInstance().SaveFreeSpace();
    return 0;
}

std::string setLocale(std::string loc)
{
    char * ret = setlocale(LC_ALL, loc.c_str());
    if (ret == nullptr) throw xybase::InvalidParameterException(u"loc", u"Failed to set locale.", 0x1000);
    return std::string{ret};
}

void InitialiseLuaEnvironment(xybase::Stream *stream)
{
    streams[streamd++] = stream;
    auto &lua = mule::Lua::LuaHost::GetInstance();
    // configuration
    lua.RegisterFunction("loaddef", loadDefine);

    // virtual filesystem control
    lua.RegisterFunction("mount", mountContainer);
    lua.RegisterFunction("seldev", selectContainer);
    lua.RegisterFunction("pdev", getDefaultContainer);
    lua.RegisterFunction("applyfile", applyStream);
    lua.RegisterFunction("unmount", unmountContainer);
    lua.RegisterFunction("ls", listfs);

    // virtual filesystem file op
    lua.RegisterFunction("open", openFile);
    lua.RegisterFunction("close", closeFile);
    lua.RegisterFunction("tell", tellFile);
    lua.RegisterFunction("seek", seekFile);
    lua.RegisterFunction("read", readFile);
    lua.RegisterFunction("write", writeFile);

    // table
    lua.RegisterFunction("regtbl", registerTable);
    lua.RegisterFunction("readtbl", readTable);
    lua.RegisterFunction("writetbl", writeTable);

    // direct sheet handle
    //lua.RegisterFunction("extbl", exportFileHndlr);
    //lua.RegisterFunction("imtbl", importFileHndlr);

    // file dump/resume
    lua.RegisterFunction("export", exportFile);
    lua.RegisterFunction("import", importFile);

    // data management
    lua.RegisterFunction("loadsheet", loadSheet);
    lua.RegisterFunction("savesheet", saveSheet);
    lua.RegisterFunction("getsheet", getSheet);
    lua.RegisterFunction("updatesheet", updateSheet);
    lua.RegisterFunction("newsheet", newSheet);

    lua.RegisterFunction("savemem", SaveMemory);
    lua.RegisterFunction("setlocale", setLocale);

    generator.indent = 2;
}
