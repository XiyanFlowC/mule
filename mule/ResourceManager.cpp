#include "ResourceManager.h"

#include <xyutils.h>
#include <Lua/LuaHost.h>

using namespace mule::Xml;
using namespace mule::Data;

bool ResourceManager::rejectOnConflict = true;

ResourceManager::ResourceManager()
{
    auto data = LoadData(Configuration::GetInstance().dataInfoFile);
    auto xml = xmlParser.Parse(data.GetData());
    if (!xmlParser.error.empty())
    {
        fputs("Failed to load infofile.\n", stderr);
        fputs(xmlParser.error.c_str(), stderr);
        return;
    }

    if (xml.name != u"info")
    {
        fputs("Info file format error: root node mismatch.\n", stderr);
    }
    for (auto &&file : xml.children)
    {
        if (file.name != u"file" || file.children.size() != 1 || !file.children.begin()->IsTextNode())
        {
            fputs("Info file format error: file node mismatch.\n", stderr);
            continue;
        }
        std::string filename = xybase::string::to_string(file.children.begin()->text);

        fileInfos[filename] = FileInfo{ (unsigned int)xybase::string::stoi(file.GetAttribute(u"id")) , (unsigned int)xybase::string::stoi(file.GetAttribute(u"crc32"))};
    }
}

int ResourceManager::CreateDirectoryRecursively(std::string path, size_t index)
{
    size_t endIndex = path.find_first_of('/', index);

    if (endIndex == std::string::npos)
    {
        if (xybase::io::access(path.c_str(), xybase::io::PM_READWRITE))
        {
            return xybase::io::mkdir(path.c_str());
        }
    }

    std::string curPath = path.substr(0, endIndex);
    if (xybase::io::access(curPath.c_str(), xybase::io::PM_READWRITE))
    {
        xybase::io::mkdir(curPath.c_str());
    }
    return CreateDirectoryRecursively(path, endIndex + 1);
}

ResourceManager &ResourceManager::GetInstance()
{
    static ResourceManager _inst;
    return _inst;
}

BinaryData ResourceManager::LoadData(std::string name)
{
    auto &&it = fileInfos.find(name);
    if (it == fileInfos.end())
    {
        throw xybase::RuntimeException(u"Unkown id for data " + xybase::string::to_utf16(name), __LINE__);
    }
    return LoadData(it->second.id);
}

BinaryData ResourceManager::LoadData(unsigned int id)
{
    char path[32];
    sprintf(path, "%02X/%02X/%02X/%02X.dat", id >> 24, (id >> 16) & 0xFF, (id >> 8) & 0xFF, id & 0xFF);
    
    FILE *f = fopen((Configuration::GetInstance().DataDir + path).c_str(), "rb");
    if (f == NULL) throw xybase::IOException(xybase::string::to_utf16(path), u"Unable to open data file.");

    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = new char[length];
    fread(buffer, length, 1, f);
    fclose(f);

    return BinaryData(buffer, length, false);
}

unsigned int ResourceManager::SaveData(BinaryData &data, unsigned int assignId)
{
    char path[32];
    unsigned int id = assignId == 0 ? crc32_eval((uint8_t *) data.GetData(), data.GetLength()) : assignId;

    // 冲突避免
    if (rejectOnConflict)
    {
        if (IsExist(id)) throw xybase::RuntimeException(u"Conflict on saving.", __LINE__);
    } else while (IsExist(id)) ++id;

    sprintf(path, "%02X/%02X/%02X/%02X.dat", id >> 24, (id >> 16) & 0xFF, (id >> 8) & 0xFF, id & 0xFF);
    std::string pp = Configuration::GetInstance().DataDir + path;
    if (xybase::io::access(pp.substr(0, pp.find_last_of('/')).c_str(), xybase::io::PM_READWRITE)) CreateDirectoryRecursively(pp.substr(0, pp.find_last_of('/')));
    FILE *f = fopen(pp.c_str(), "wb");
    if (f == NULL)
    {
        throw xybase::IOException(xybase::string::to_utf16(path), u"Unable to open file to write ");
    }

    fwrite(data.GetData(), data.GetLength(), 1, f);
    fclose(f);

    return id;
}

bool ResourceManager::IsExist(unsigned int id)
{
    char path[32];
    sprintf(path, "%02X/%02X/%02X/%02X.dat", id >> 24, (id >> 16) & 0xFF, (id >> 8) & 0xFF, id & 0xFF);

    FILE *f = fopen((Configuration::GetInstance().DataDir + path).c_str(), "rb");
    if (f != NULL)
    {
        fclose(f);
        return true;
    }
    return false;
}

bool ResourceManager::IsExist(std::string name)
{
    auto &&it = fileInfos.find(name);
    if (it == fileInfos.end())
    {
        return false;
    }
    return IsExist(it->second.id);
}

BinaryData ResourceManager::LoadResource(std::string path)
{
    FILE *f = fopen((Configuration::GetInstance().ResourcesDir + path).c_str(), "rb");
    if (f == NULL)
    {
        if (f == NULL) throw xybase::IOException(xybase::string::to_utf16(path), u"Unable to open resource file.");
    }

    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = new char[length];
    fread(buffer, length, 1, f);
    fclose(f);

    return BinaryData(buffer, length, false);
}

xybase::Stream *ResourceManager::OpenResource(std::string path, std::function<xybase::Stream *(std::string path)> creator)
{
    return creator(Configuration::GetInstance().ResourcesDir + path);
}

xybase::Stream *ResourceManager::OpenData(unsigned int id, std::function<xybase::Stream *(std::string path)> creator)
{
    char path[32];
    sprintf(path, "%02X/%02X/%02X/%02X.dat", id >> 24, (id >> 16) & 0xFF, (id >> 8) & 0xFF, id & 0xFF);

    return creator(Configuration::GetInstance().DataDir + path);
}

xybase::Stream *ResourceManager::OpenData(std::string name, std::function<xybase::Stream *(std::string path)> creator)
{
    auto &&it = fileInfos.find(name);
    if (it == fileInfos.end())
    {
        throw xybase::RuntimeException(u"Conflict on saving.", __LINE__);
    }
    return OpenData(it->second.id, creator);
}

void ResourceManager::LoadDefinition(std::string def)
{
    std::string path = Configuration::GetInstance().DefinitionsDir + def + ".xml";
    FILE *f = fopen(path.c_str(), "rb");

    if (f == NULL)
    {
        if (f == NULL) throw xybase::IOException(xybase::string::to_utf16(path), u"Unable to open definition file.");
    }

    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = new char[length];
    fread(buffer, length, 1, f);
    fclose(f);

    XmlNode root = xmlParser.Parse(buffer);
    if (xmlParser.error != "")
    {
        fputs(xmlParser.error.c_str(), stderr);
        return;
    }

    // 处理结构
    if (root.GetName() != u"def")
    {
        fputs("XML Definition Format Incorrect: Root node (signation) mismatch.\n", stderr);
        return;
    }

    for (auto &&child : root.children)
    {
        if (child.IsTextNode())
        {
            fputs("XML Definition Format Incorrect: No text allowed in def tag.\n", stderr);
            return;
        }

        if (child.GetName() == u"struct")
        {
            Structure *structure = new Structure(child.GetAttribute(u"name"));
            for (auto &&field : child.GetChildren())
            {
                if (field.GetName() != u"field")
                {
                    fputs("XML Definition Format Incorrect: Only feild tag is allowed in struct.\n", stderr);
                    return;
                }

                auto metadata = field.GetChildren();
                std::map<std::u16string, std::u16string> metainfo;
                if (!metadata.empty())
                {
                    for (auto &&metadatum : metadata)
                    {
                        if (metadatum.IsTextNode())
                        {
                            fputs("XML Definition Format Incorrect: No text allowed in tag field.\n", stderr);
                            return;
                        }

                        metainfo[metadatum.GetName()] = metadatum.GetChildren().begin()->GetText();
                    }
                }

                auto ret = TypeManager::GetInstance().GetOrCreateType(field.GetAttribute(u"type"), metainfo);
                if (ret == nullptr)
                {
                    fprintf(stderr, "Invalid type: %s in definition of type %s\n", xybase::string::to_string(field.GetAttribute(u"type")).c_str(), xybase::string::to_string(child.name).c_str());
                }
                structure->AppendField(field.GetAttribute(u"name"), ret);
            }
            TypeManager::GetInstance().RegisterObject(structure, child.GetAttribute(u"name"));
        }
        else if (child.GetName() == u"script")
        {
            xybase::StringBuilder<char16_t> sb;
            for (auto &&text : child.GetChildren())
            {
                sb.Append(text.GetText().c_str());
            }
            mule::Lua::LuaHost::GetInstance().RunString(xybase::string::to_string(sb.ToString()).c_str());
        }
        else
            fprintf(stderr, "Invalid tag: %s", xybase::string::to_string(child.GetName()).c_str());
    }

    delete[] buffer;
}

std::string ResourceManager::LoadSheet(std::string sheetName)
{
    FILE *f = fopen((Configuration::GetInstance().SheetsDir + sheetName + ".xml").c_str(), "r");
    if (f == NULL)
    {
        if (f == NULL) throw xybase::IOException(xybase::string::to_utf16(Configuration::GetInstance().SheetsDir + sheetName + ".xml"), u"Unable to open sheet.");
    }

    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = new char[length];
    fread(buffer, length, 1, f);
    std::string ret {buffer};
    fclose(f);
    delete[] buffer;

    return ret;
}

void ResourceManager::SaveSheet(std::string sheetName, std::string sheet)
{
    FILE *f = fopen((Configuration::GetInstance().SheetsDir + sheetName + ".xml").c_str(), "w");
    if (f == NULL)
    {
        throw xybase::IOException(xybase::string::to_utf16(Configuration::GetInstance().SheetsDir + sheetName + ".xml"), u"Unable to open sheet for write.");
    }

    fwrite(sheet.c_str(), sheet.size(), 1, f);
    fclose(f);
}

BinaryData::BinaryData(char *data, size_t length, bool duplicate)
{
    SetData(data, length, duplicate);
}

const char *BinaryData::GetData()
{
    return data.get();
}

size_t BinaryData::GetLength()
{
    return length;
}

void BinaryData::SetData(char *data, size_t length, bool duplicate)
{
    if (duplicate)
    {
        this->data = std::shared_ptr<char[]>(new char[length]);
        memcpy(this->data.get(), data, length);
    }
    else
    {
        this->data = std::shared_ptr<char[]>(data);
    }
    this->length = length;
}

void BinaryData::SetData(const char *data, size_t length)
{
    SetData((char *) data, length, true);
}
