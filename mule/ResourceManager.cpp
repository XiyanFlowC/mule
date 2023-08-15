#include "ResourceManager.h"

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

    if (xml.name != "info")
    {
        fputs("Info file format error: root node mismatch.\n", stderr);
    }
    for (auto &&file : xml.children)
    {
        if (file.name != "file" || file.children.size() != 1 || !file.children.begin()->IsTextNode())
        {
            fputs("Info file format error: file node mismatch.\n", stderr);
            continue;
        }
        std::string filename = file.children.begin()->text;

        fileInfos[filename] = FileInfo{ (unsigned int)xybase::string::stoi(file.GetAttribute("id")) , (unsigned int)xybase::string::stoi(file.GetAttribute("crc32"))};
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
        throw mule::Exception::Exception("Unkown id for data " + name, __FILE__, __LINE__);
    }
    return LoadData(it->second.id);
}

BinaryData ResourceManager::LoadData(unsigned int id)
{
    char path[32];
    sprintf(path, "%02X/%02X/%02X/%02X.dat", id >> 24, (id >> 16) & 0xFF, (id >> 8) & 0xFF, id & 0xFF);
    
    FILE *f = fopen((Configuration::GetInstance().DataDir + path).c_str(), "rb");
    if (f == NULL) throw mule::Exception::Exception(std::string("Unable to open data file ") + path, __FILE__, __LINE__);

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
        if (IsExist(id)) throw mule::Exception::Exception(std::string("Conflict on saving."), __FILE__, __LINE__);
    } else while (IsExist(id)) ++id;

    sprintf(path, "%02X/%02X/%02X/%02X.dat", id >> 24, (id >> 16) & 0xFF, (id >> 8) & 0xFF, id & 0xFF);
    std::string pp = Configuration::GetInstance().DataDir + path;
    if (xybase::io::access(pp.substr(0, pp.find_last_of('/')).c_str(), xybase::io::PM_READWRITE)) CreateDirectoryRecursively(pp.substr(0, pp.find_last_of('/')));
    FILE *f = fopen(pp.c_str(), "wb");
    if (f == NULL)
    {
        throw mule::Exception::Exception(std::string("Unable to open file to write ") + path, __FILE__, __LINE__);
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
        if (f == NULL) throw mule::Exception::Exception(std::string("Unable to open resource file ") + path, __FILE__, __LINE__);
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
        throw mule::Exception::Exception("Unkown id for data " + name, __FILE__, __LINE__);
    }
    return OpenData(it->second.id, creator);
}

void ResourceManager::LoadDefinition(std::string def)
{
    std::string path = Configuration::GetInstance().DefinitionsDir + def + ".xml";
    FILE *f = fopen(path.c_str(), "rb");

    if (f == NULL)
    {
        if (f == NULL) throw mule::Exception::Exception(std::string("Unable to open resource file ") + path, __FILE__, __LINE__);
    }

    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = new char[length];
    fread(buffer, length, 1, f);
    fclose(f);

    XmlNode node = xmlParser.Parse(buffer);
    if (xmlParser.error != "")
    {
        fputs(xmlParser.error.c_str(), stderr);
        return;
    }

    // 处理结构
    if (node.GetName() != "def")
    {
        fputs("XML Definition Format Incorrect.\n", stderr);
        return;
    }

    for (auto &&child : node.children)
    {
        if (child.IsTextNode())
        {
            fputs("XML Definition Format Incorrect.\n", stderr);
            return;
        }

        Structure *structure = new Structure(child.name);
        for (auto &&field : child.children)
        {
            auto ret = TypeManager::GetInstance().GetOrCreateObject(field.GetAttribute("type"));
            if (ret == nullptr)
            {
                fprintf(stderr, "Invalid type: %s in definition of type %s\n", field.GetAttribute("type").c_str(), child.name.c_str());
            }
            structure->AppendField(field.name, ret);
        }
        TypeManager::GetInstance().RegisterObject(structure, child.name);
    }

    delete[] buffer;
}

std::string ResourceManager::LoadSheet(std::string sheetName)
{
    FILE *f = fopen((Configuration::GetInstance().SheetsDir + sheetName + ".xml").c_str(), "r");
    if (f == NULL)
    {
        if (f == NULL) throw mule::Exception::Exception(std::string("Unable to open sheet ") + sheetName, __FILE__, __LINE__);
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
        throw mule::Exception::Exception(std::string("Unable to open file to write ") + sheetName, __FILE__, __LINE__);
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
        this->data = std::make_shared<char[]>(length);
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
