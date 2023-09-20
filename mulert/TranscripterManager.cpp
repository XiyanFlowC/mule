#include "TranscripterManager.h"

using namespace mule;

TranscripterManager &mule::TranscripterManager::GetInstance()
{
    static TranscripterManager _inst;
    return _inst;
}

xybase::Stream *mule::TranscripterManager::GetTranscripter(const char16_t *name, xybase::Stream *infraStream)
{
    for (auto &&transcripter : streamCreators)
    {
        auto ret = transcripter(name, infraStream);
        if (ret == nullptr) continue;
        return ret;
    }
    return nullptr;
}

xybase::Stream *mule::TranscripterManager::GetTranscripter(int count, const char16_t **parameter)
{
    for (auto &&transcripter : advancedStreamCreators)
    {
        auto ret = transcripter(count, parameter);
        if (ret == nullptr) continue;
        return ret;
    }
    return nullptr;
}

void mule::TranscripterManager::RegisterCreator(xybase::Stream *(*creator)(const char16_t *name, xybase::Stream *infraStream))
{
    streamCreators.push_back(creator);
}

void mule::TranscripterManager::RegisterCreator(xybase::Stream *(*creator)(int count, const char16_t **parameter))
{
    advancedStreamCreators.push_back(creator);
}
