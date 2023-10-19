#include "ResourceManager.h"

#include <xyutils.h>
#include <Storage/BinaryData.h>
#include <Lua/LuaHost.h>

using namespace mule::Xml;
using namespace mule::Data;
using namespace mule::Storage;

ResourceManager &ResourceManager::GetInstance()
{
    static ResourceManager _inst;
    return _inst;
}

BinaryData ResourceManager::LoadResource(std::string path)
{
    FILE *f = fopen((Configuration::GetInstance().ResourcesDir + path).c_str(), "rb");
    if (f == NULL)
    {
        if (f == NULL) throw xybase::IOException(xybase::string::to_wstring(path), L"Unable to open resource file.");
    }

    logger.Info(L"Opened resource file {}", xybase::string::to_wstring(path));

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

void ResourceManager::LoadDefinition(std::string def)
{
    std::string path = Configuration::GetInstance().DefinitionsDir + def + ".xml";
    FILE *f = fopen(path.c_str(), "rb");

    if (f == NULL)
    {
        if (f == NULL) throw xybase::IOException(xybase::string::to_wstring(path), L"Unable to open definition file.");
    }

    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = new char[length + 1];
    fread(buffer, length, 1, f);
    fclose(f);
    buffer[length] = '\0';

    XmlNode root = xmlParser.Parse(buffer);
    if (xmlParser.error != "")
    {
        logger.Fatal(xybase::string::to_wstring(xmlParser.error).c_str());
        return;
    }

    // 处理结构
    if (root.GetName() != u"def")
    {
        logger.Fatal(L"XML Definition Format Incorrect: Root node (signation) mismatch.");
        return;
    }

    for (auto &&child : root.children)
    {
        if (child.IsTextNode())
        {
            logger.Fatal(L"XML Definition Format Incorrect: No text allowed in def tag.");
            return;
        }

        if (child.GetName() == u"struct")
        {
            Structure *structure = new Structure(child.GetAttribute(u"name"));
            for (auto &&field : child.GetChildren())
            {
                if (field.GetName() != u"field")
                {
                    logger.Fatal(L"XML Definition Format Incorrect: Only feild tag is allowed in struct.");
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
                            logger.Fatal(L"XML Definition Format Incorrect: No text allowed in tag field.");
                            return;
                        }

                        metainfo[metadatum.GetName()] = metadatum.GetChildren().begin()->GetText();
                    }
                }

                auto ret = TypeManager::GetInstance().GetOrCreateType(field.GetAttribute(u"type"), metainfo);
                if (ret == nullptr)
                {
                    logger.Error(L"Invalid type: {} in definition of type {}", xybase::string::to_wstring(field.GetAttribute(u"type")), xybase::string::to_wstring(child.name));
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
        else if (child.GetName() == u"")
            continue;
        else
            logger.Warn(L"Invalid tag: {}, ignoring...", xybase::string::to_wstring(child.GetName()));
    }

    delete[] buffer;
}

std::string ResourceManager::LoadSheet(std::string sheetName)
{
    FILE *f = fopen((Configuration::GetInstance().SheetsDir + sheetName + ".xml").c_str(), "r");
    if (f == NULL)
    {
        if (f == NULL) throw xybase::IOException(xybase::string::to_wstring(Configuration::GetInstance().SheetsDir + sheetName + ".xml"), L"Unable to open sheet.");
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
        throw xybase::IOException(xybase::string::to_wstring(Configuration::GetInstance().SheetsDir + sheetName + ".xml"), L"Unable to open sheet for write.");
    }

    fwrite(sheet.c_str(), sheet.size(), 1, f);
    fclose(f);
}
