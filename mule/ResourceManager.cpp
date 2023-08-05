#include "ResourceManager.h"

ResourceManager &ResourceManager::GetInstance()
{
    static ResourceManager _inst;
    return _inst;
}

BinaryData ResourceManager::GetData(std::string name)
{
    auto &&it = dataNameMap.find(name);
    if (it == dataNameMap.end())
    {
        throw mule::Exception::Exception("Unkown id for data " + name, __FILE__, __LINE__);
    }
    return GetData(it->second);
}

BinaryData ResourceManager::GetData(unsigned int id)
{
    char path[32];
    sprintf(path, "%02X/%02X/%02X/%02X.dat", id >> 48, (id >> 32) & 0xFF, (id >> 16) & 0xFF, id & 0xFF);
    
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

unsigned int ResourceManager::SetData(BinaryData &data)
{
    char path[32];
    unsigned int crc = crc32_eval((uint8_t *) data.GetData(), data.GetLength());
    sprintf(path, "%02X/%02X/%02X/%02X.dat", crc >> 48, (crc >> 32) & 0xFF, (crc >> 16) & 0xFF, crc & 0xFF);

    // 确认文件不存在
    FILE *f = fopen((Configuration::GetInstance().DataDir + path).c_str(), "rb");
    if (f != NULL)
    {
        fclose(f);
        throw mule::Exception::Exception("Data save clash of crc " + std::to_string(crc), __FILE__, __LINE__);
    }

    f = fopen((Configuration::GetInstance().DataDir + path).c_str(), "wb");
    if (f == NULL)
    {
        throw mule::Exception::Exception(std::string("Unable to open file to write ") + path, __FILE__, __LINE__);
    }

    fwrite(data.GetData(), data.GetLength(), 1, f);
    fclose(f);

    return crc;
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
