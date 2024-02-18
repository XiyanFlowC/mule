#include "SmartReference.h"

#include "TypeManager.h"
#include "../Storage/DataManager.h"
#include "ReferenceManager.h"
#include "../Configuration.h"
#include <xystring.h>
#include <xyutils.h>

using namespace mule::Data::Basic;
using namespace mule::Data;

#define SRMM_DATAFILE_ID (0x0A003939)

int mule::Data::SmartReference::GetAlign() const
{
	if (mule::Configuration::GetInstance().IsExist(u"mule.data.smart-reference.align"))
		return mule::Configuration::GetInstance().GetSigned(u"mule.data.smart-reference.align");
	else
		return 1;
}

mule::Data::SmartReference::SmartReference(Basic::BasicType *referent)
	: referent(referent)
{
}

size_t mule::Data::SmartReference::Size() const
{
	return size_t(4);
}

std::u16string mule::Data::SmartReference::GetDataType() const
{
	return referent->GetDataType();
}

MultiValue mule::Data::SmartReference::DoRead(xybase::Stream *stream)
{
	auto ptr = stream->ReadInt32();
	auto cur = stream->Tell();
	stream->Seek(ptr);
	auto value = referent->DoRead(stream);
	auto size = referent->GetLastSize();
	MemoryManager::GetInstance().GetMemory(stream).RegisterFragment(ptr, XY_ALIGN(size, GetAlign()));
	stream->Seek(cur);
	return value;
}

void mule::Data::SmartReference::DoWrite(xybase::Stream *stream, const MultiValue &value)
{
	auto size = referent->EvalSize(value);
	auto ptr = MemoryManager::GetInstance().AssignFor(stream, value, size, GetAlign());
	stream->Write((int32_t)ptr);
	auto cur = stream->Tell();
	referent->DoWrite(stream, value);
	stream->Seek(cur);
}

bool mule::Data::SmartReference::IsComposite() const
{
	return referent->IsComposite();
}

mule::Data::Basic::Type *mule::Data::SmartReference::SmartReferenceCreator::DoCreateObject(const std::u16string &info)
{
	return DoCreateObject(info, {});
}

mule::Data::Basic::Type *mule::Data::SmartReference::SmartReferenceCreator::DoCreateObject(const std::u16string &info, const std::map<std::u16string, std::u16string> &metainfo)
{
	if (!info.ends_with(u"^"))
	{
		return nullptr;
	}

	Basic::BasicType *innerType = dynamic_cast<Basic::BasicType *>(TypeManager::GetInstance().GetOrCreateType(info.substr(0, info.size() - 1), metainfo));
	if (innerType == nullptr) return nullptr;

	SmartReference *referrer = new SmartReference(innerType);
	return referrer;
}


SmartReference::MemoryManager::MemoryManager()
{
	logger.Info(L"Starting up...");
	FILE *cache = mule::Storage::DataManager::GetInstance().OpenRaw(SRMM_DATAFILE_ID);

	if (cache == nullptr) return;

	logger.Info(L"Memory specification file(id:{}) found, loading...", SRMM_DATAFILE_ID);
	size_t size;
	fscanf(cache, "%zu\n", &size);
	logger.Info(L"Found {} cache(s) in total.", size);
	for (size_t i = 0; i < size; ++i)
	{
		char buffer[1024];
		if (!fscanf(cache, "%[^\n]", buffer))
		{
			fclose(cache);
			logger.Error(L"Failed to load cache, unable to read, errno={}.", errno);
			return;
		}
		size_t count;
		fscanf(cache, "%zu", &count);
		logger.Info(L"Loading cache for {}({} fragments contained)...", xybase::string::to_wstring(buffer), count);
		auto &fm = memories[xybase::string::to_utf16(buffer)];
		for (size_t j = 0; j < count; ++j)
		{
			unsigned int pos, len;
			fscanf(cache, "%X %u\n", &pos, &len);
			fm.RegisterFragment(pos, len);
		}
		fm.Defragment();
	}

	fclose(cache);
}

void SmartReference::MemoryManager::DisposeStream(xybase::Stream *stream)
{
	if (memories.contains(stream->GetName()))
		memories.erase(stream->GetName());
}

SmartReference::MemoryManager &SmartReference::MemoryManager::GetInstance()
{
	static SmartReference::MemoryManager _inst;
	return _inst;
}

xybase::Fragment::FragmentManager &SmartReference::MemoryManager::GetMemory(xybase::Stream *stream)
{
	return memories[stream->GetName()];
}

size_t SmartReference::MemoryManager::AssignFor(xybase::Stream *stream, const mule::Data::Basic::MultiValue &str, size_t size, int align)
{
	auto &&name = stream->GetName();
	auto &&cache = assign.find(name);
	if (cache != assign.end())
	{
		auto &&loc = cache->second.find(str);
		if (loc != cache->second.end())
		{
			return loc->second;
		}
		else
		{
			return cache->second[str] = GetMemory(stream).Alloc(size, align);
		}
	}

	return assign[name][str] = GetMemory(stream).Alloc(size, align);
}

void SmartReference::MemoryManager::SaveFreeSpace()
{
	FILE *file = mule::Storage::DataManager::GetInstance().OpenRaw(SRMM_DATAFILE_ID, true);
	fprintf(file, "%llu\n", static_cast<unsigned long long>(memories.size()));
	for (auto &&item : memories)
	{
		item.second.Defragment();
		fprintf(file, "%s\n", xybase::string::to_string(item.first).c_str());
		auto &frags = item.second.GetFragments();
		fprintf(file, "%lld\n", static_cast<long long>(frags.size()));
		for (auto &frag : frags)
		{
			fprintf(file, "%X %u\n", frag->GetBeginning(), frag->GetSize());
		}
	}
	fclose(file);
}