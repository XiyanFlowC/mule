#include "ResourceManager.h"

using namespace mule::Xml;
using namespace mule::Data;

ResourceManager &ResourceManager::GetInstance()
{
    static ResourceManager _inst;
    return _inst;
}

BinaryData ResourceManager::LoadData(std::string name)
{
    auto &&it = dataNameMap.find(name);
    if (it == dataNameMap.end())
    {
        throw mule::Exception::Exception("Unkown id for data " + name, __FILE__, __LINE__);
    }
    return LoadData(it->second);
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
    unsigned int id = assignId == 0 ? crc32_eval((uint8_t *) data.LoadData(), data.GetLength()) : assignId;

    // 冲突避免
    while (IsExist(id)) ++id;

    sprintf(path, "%02X/%02X/%02X/%02X.dat", id >> 24, (id >> 16) & 0xFF, (id >> 8) & 0xFF, id & 0xFF);
    FILE *f = fopen((Configuration::GetInstance().DataDir + path).c_str(), "rb");
    f = fopen((Configuration::GetInstance().DataDir + path).c_str(), "wb");
    if (f == NULL)
    {
        throw mule::Exception::Exception(std::string("Unable to open file to write ") + path, __FILE__, __LINE__);
    }

    fwrite(data.LoadData(), data.GetLength(), 1, f);
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
    auto &&it = dataNameMap.find(name);
    if (it == dataNameMap.end())
    {
        return false;
    }
    return IsExist(it->second);
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
    auto &&it = dataNameMap.find(name);
    if (it == dataNameMap.end())
    {
        throw mule::Exception::Exception("Unkown id for data " + name, __FILE__, __LINE__);
    }
    return OpenData(it->second, creator);
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

BinaryData::BinaryData(char *data, size_t length, bool duplicate)
{
    SaveData(data, length, duplicate);
}

const char *BinaryData::LoadData()
{
    return data.get();
}

size_t BinaryData::GetLength()
{
    return length;
}

void BinaryData::SaveData(char *data, size_t length, bool duplicate)
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

void BinaryData::SaveData(const char *data, size_t length)
{
    SaveData((char *) data, length, true);
}
