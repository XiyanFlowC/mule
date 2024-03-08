#include "SmartReference.h"

#include "TypeManager.h"
#include "../Storage/DataManager.h"
#include "../SheetReference.h"
#include "../Configuration.h"
#include <xystring.h>
#include <xyutils.h>

using namespace mule::Data::Basic;
using namespace mule::Data;

#define SRMM_DATAFILE_ID (0x00003939)

int mule::Data::SmartReference::GetAlign() const
{
	if (mule::Configuration::GetInstance().IsExist(u"mule.data.smart-reference.align"))
		return (int)mule::Configuration::GetInstance().GetSigned(u"mule.data.smart-reference.align");
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
	if (ptr == 0)
		return MultiValue::MV_NULL;
	stream->Seek(ptr);
	auto value = referent->DoRead(stream);
	auto size = referent->GetLastSize();
	MemoryManager::GetInstance().GetMemory(stream).RegisterFragment(ptr, XY_ALIGN(size, GetAlign()));
	stream->Seek(cur);
	value.metadata[u"ptr"] = ptr;
	return value;
}

void mule::Data::SmartReference::DoWrite(xybase::Stream *stream, const MultiValue &value)
{
	if (value.IsType(MultiValue::MVT_NULL))
	{
		stream->Write((uint32_t)0);
		return;
	}
	auto size = referent->EvalSize(value);
	MultiValue resizedValue = value;
	resizedValue.metadata[u"size"] = size;
	auto ptr = MemoryManager::GetInstance().AssignFor(stream, resizedValue, referent, size, GetAlign());
	stream->Write((int32_t)ptr);
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

void SmartReference::MemoryManager::DisposeStreamRecords(xybase::Stream *stream)
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

size_t SmartReference::MemoryManager::AssignFor(xybase::Stream *stream, const mule::Data::Basic::MultiValue &val, mule::Data::Basic::BasicType *type, size_t size, int align)
{
	auto &&name = stream->GetName();
	// 不存在的项
	if (!assignedAddresses.contains(name) 
		|| !assignedAddresses[name].contains(type)
		|| !assignedAddresses[name][type].contains(val))
	{
		// 分配空间
		auto loc = GetMemory(stream).Alloc(size, align);
		// 写入
		auto cur = stream->Tell();
		stream->Seek(loc);
		type->DoWrite(stream, val);
		stream->Seek(cur);
		// 登录并返回
		return assignedAddresses[name][type][val] = loc;
	}

	// 已存在的项，返回上次分配的地址
	return assignedAddresses[name][type][val];
}

bool mule::Data::SmartReference::MemoryManager::IsAssigned(xybase::Stream *stream, const mule::Data::Basic::MultiValue &value, mule::Data::Basic::BasicType *type)
{
	if (!assignedAddresses.contains(stream->GetName())) return false;
	if (!assignedAddresses[stream->GetName()].contains(type)) return false;
	return assignedAddresses[stream->GetName()][type].contains(value);
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
			fprintf(file, "%zX %zu\n", frag->GetBeginning(), frag->GetSize());
		}
	}
	fclose(file);
}
