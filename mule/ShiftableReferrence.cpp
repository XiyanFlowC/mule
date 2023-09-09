#include "ShiftableReferrence.h"

#include <xyutils.h>
#include <Data/Storage/DataManager.h>
#include <Stream/ElfStream.h>

#define SRMM_DATAFILE_ID (0x0A00'3939)

using namespace mule::Data::Basic;
using namespace mule::Data;
using namespace mule::Stream;

bool ShiftableReferrence::doShift = false;
int ShiftableReferrence::align = 0;

int ShiftableReferrence::GetAlign(size_t loc, xybase::Stream *stream)
{
	if (this->align == 0)
	{
		ElfStream *elf = dynamic_cast<ElfStream *>(stream);
		return elf == nullptr ? 1 : (int)elf->GetAlign(loc);
	}
	else
		return this->align;
}

void ShiftableReferrence::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	int ptr = stream->ReadInt32();
	size_t loc = stream->Tell();
	stream->Seek(ptr, 0);
	referent->Read(stream, dataHandler);
	MemoryManager::GetInstance().GetMemory(stream).RegisterFragment(ptr, XY_ALIGN(referent->GetLastSize(), GetAlign(ptr, stream))));
	stream->Seek(loc, 0);
}

void ShiftableReferrence::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	const MultiValue &mv = dataHandler->OnDataWrite();
	// 分配空间
	size_t ptr;
	if (mv.IsType(MultiValue::MVT_STRING))
		ptr = MemoryManager::GetInstance().AssignFor(stream, *mv.value.stringValue, referent->EvalSize(mv));
	else if (mv.IsType(MultiValue::MVT_NULL))
	{
		stream->Write((uint32_t)0);
		return;
	}
	else
		throw xybase::InvalidParameterException(u"mv", u"Not a string, unable to shift!", __LINE__);

	stream->Write((int32_t)ptr);
	size_t loc = stream->Tell();
	stream->Seek(ptr, SEEK_SET);
	// 实际执行写入
	referent->Write(stream, dataHandler);
	stream->Seek(loc, SEEK_SET);
}

size_t ShiftableReferrence::Size() const
{
	return size_t(4);
}

std::u16string ShiftableReferrence::GetTypeName() const
{
	return u"rstring";
}

void ShiftableReferrence::StreamDispose(xybase::Stream *stream)
{
	MemoryManager::GetInstance().DisposeStream(stream);
}

Type *ShiftableReferrence::ShiftableReferrenceCreator::DoCreateObject(std::u16string info)
{
	if (info.ends_with(u"rstring"))
	{
		Type *inner = TypeManager::GetInstance().GetOrCreateType(info.substr(info.length() - 1));
		if (inner == nullptr) return nullptr;

		ShiftableReferrence *ret = new ShiftableReferrence();
		ret->referent = inner;
		return ret;
	}
	return nullptr;
}

ShiftableReferrence::MemoryManager::MemoryManager()
{
	FILE *cache = mule::Data::Storage::DataManager::GetInstance().OpenRaw(SRMM_DATAFILE_ID);

	if (cache == nullptr) return;

	size_t size;
	fscanf(cache, "%llu\n", &size);
	for (size_t i = 0; i < size; ++i)
	{
		char buffer[1024];
		if (!fscanf(cache, "%[^\n]", buffer))
		{
			fclose(cache);
			fprintf(stderr, "Failed to load cache.");
			return;
		}
		size_t count;
		fscanf(cache, "%llu", &count);
		fprintf(stderr, "File %s On Reg...", buffer);
		auto fm = memories[xybase::string::to_utf16(buffer)];
		for (size_t j = 0; j < size; ++j)
		{
			unsigned int pos, len;
			fscanf(cache, "%u %u\n", &pos, &len);
			fm.RegisterFragment(pos, len);
		}
	}

	fclose(cache);
}

void ShiftableReferrence::MemoryManager::DisposeStream(xybase::Stream *stream)
{
	if (memories.contains(stream->GetName()))
		memories.erase(stream->GetName());
}

ShiftableReferrence::MemoryManager &ShiftableReferrence::MemoryManager::GetInstance()
{
	static ShiftableReferrence::MemoryManager _inst;
	return _inst;
}

mule::Data::Space::FragmentManager ShiftableReferrence::MemoryManager::GetMemory(xybase::Stream *stream)
{
	return memories[stream->GetName()];
}

size_t ShiftableReferrence::MemoryManager::AssignFor(xybase::Stream *stream, const std::u16string &str, size_t size)
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
			return cache->second[str] = GetMemory(stream).Alloc(size, 4);
		}
	}

	return assign[name][str] = GetMemory(stream).Alloc(size, 4);
}

void ShiftableReferrence::MemoryManager::SaveFreeSpace()
{
	FILE *file = mule::Data::Storage::DataManager::GetInstance().OpenRaw(SRMM_DATAFILE_ID, true);
	fprintf(file, "%llu\n", static_cast<unsigned long long>(memories.size()));
	for (auto &&item : memories)
	{
		fprintf(file, "%s\n", xybase::string::to_string(item.first).c_str());
		auto &frags = item.second.GetFragments();
		fprintf(file, "%lld\n", static_cast<long long>(frags.size()));
		for (auto &frag : frags)
		{
			fprintf(file, "%X %u\n", frag->GetBegining(), frag->GetSize());
		}
	}
	fclose(file);
}
