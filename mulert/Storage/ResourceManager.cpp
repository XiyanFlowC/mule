#include "ResourceManager.h"

#include "../Configuration.h"
#include <xyutils.h>
#include <Exception/IOException.h>

using namespace xybase::xml;
using namespace mule::Storage;

ResourceManager &ResourceManager::GetInstance()
{
    static ResourceManager _inst;
    return _inst;
}

BinaryData ResourceManager::LoadResource(std::string path)
{
    FILE *f = fopen((xybase::string::to_string(Configuration::GetInstance().GetString(u"mule.resource.basedir")) + path).c_str(), "rb");
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
