#include "ResourceManager.h"

#include <xyutils.h>
#include <Data/Storage/BinaryData.h>
#include <Lua/LuaHost.h>

using namespace mule::Xml;
using namespace mule::Data;
using namespace mule::Data::Storage;

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
        else if (child.GetName() == u"")
            continue;
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
