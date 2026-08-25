#include "ResourceManager.h"

#include "../Configuration.h"
#include <memory>
#include <cstdio>
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

    std::unique_ptr<FILE, decltype(&fclose)> file(f, &fclose);

    fseek(file.get(), 0, SEEK_END);
    size_t length = ftell(file.get());
    fseek(file.get(), 0, SEEK_SET);

    auto buffer = std::make_unique<char[]>(length);
    fread(buffer.get(), length, 1, file.get());

    return BinaryData(buffer.release(), length, false);
}

MULERT_API void mule::Storage::ResourceManager::SaveResource(std::u16string path, const mule::Storage::BinaryData &data)
{
    FILE *f = fopen((xybase::string::to_string(Configuration::GetInstance().GetString(u"mule.resource.basedir") + path)).c_str(), "wb");
    if (f == NULL)
    {
        if (f == NULL) throw xybase::IOException(xybase::string::to_wstring(path), L"Unable to open resource file.");
    }

    logger.Info(L"Opened resource file {}", xybase::string::to_wstring(path));

    fwrite(data.GetData(), data.GetLength(), 1, f);

    fclose(f);
}
