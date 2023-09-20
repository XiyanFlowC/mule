#include "ShiftableString.h"

#include <xystring.h>
#include <xyutils.h>
#include <Storage/DataManager.h>
#include <Stream/ElfStream.h>

#define SRMM_DATAFILE_ID (0x0A00'3939)

using namespace mule::Data::Basic;
using namespace mule::Data;
using namespace mule::Stream;

bool ShiftableString::doShift = false;
int ShiftableString::align = 0;
#include <iostream>
int ShiftableString::GetAlign(size_t loc, xybase::Stream *stream)
{
	if (this->align == 0)
	{
		ElfStream *elf = dynamic_cast<ElfStream *>(stream);
		return elf == nullptr ? 1 : (int)elf->GetAlign(loc);
	}
	else
		return this->align;
}

void ShiftableString::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	int ptr = stream->ReadInt32();
	if (ptr == 0)
	{
		dataHandler->OnDataRead(MultiValue::MV_NULL);
		return;
	}
	
	size_t loc = stream->Tell();
	stream->Seek(ptr, xybase::Stream::SM_BEGIN);
	auto raw = stream->ReadString();
	dataHandler->OnDataRead(MultiValue{ xybase::string::to_utf16(raw) });
	dataHandler->AppendMetadatum(u"ptr", ptr);
	dataHandler->AppendMetadatum(u"size", raw.size());
	if (raw != "")
		MemoryManager::GetInstance().GetMemory(stream).RegisterFragment(ptr, XY_ALIGN(raw.size() + 1, GetAlign(ptr, stream))));
	stream->Seek(loc, xybase::Stream::SM_BEGIN);
}

void ShiftableString::Write(xybase::Stream *stream, FileHandler * fileHandler)
{
	const MultiValue &mv = fileHandler->OnDataWrite();
	// 分配空间
	if (mv.IsType(MultiValue::MVT_STRING))
	{
		size_t ptr;
		auto str = xybase::string::to_string(*fileHandler->OnDataWrite().value.stringValue);
		/*if (str == "")
			ptr = 0xA79750;
		else*/
			ptr = MemoryManager::GetInstance().AssignFor(stream, *mv.value.stringValue, str.size() + 1);

		stream->Write((int32_t)ptr);
		size_t loc = stream->Tell();
		stream->Seek(ptr, xybase::Stream::SM_BEGIN);
		// 实际执行写入
		stream->Write(str.c_str(), str.size() + 1);
		stream->Seek(loc, xybase::Stream::SM_BEGIN);
	}
	else if (mv.IsType(MultiValue::MVT_NULL))
	{
		stream->Write((uint32_t)0);
		return;
	}
	else
		throw xybase::InvalidParameterException(L"mv", L"Not a string, unable to shift!", __LINE__);

}

size_t ShiftableString::Size() const
{
	return size_t(4);
}

std::u16string ShiftableString::GetDataType() const
{
	return u"sstring";
}

void ShiftableString::StreamDispose(xybase::Stream *stream)
{
	MemoryManager::GetInstance().DisposeStream(stream);
}

Type *ShiftableString::ShiftableStringCreator::DoCreateObject(std::u16string info)
{
	if (info == u"sstring")
	{
		ShiftableString *ret = new ShiftableString();
		return ret;
	}
	return nullptr;
}

ShiftableString::MemoryManager::MemoryManager()
{
	FILE *cache = mule::Storage::DataManager::GetInstance().OpenRaw(SRMM_DATAFILE_ID);

	if (cache == nullptr) return;

	size_t size;
	fscanf(cache, "%zu\n", &size);
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
		fscanf(cache, "%zu", &count);
		fprintf(stderr, "File %s On Reg...", buffer);
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

void ShiftableString::MemoryManager::DisposeStream(xybase::Stream *stream)
{
	if (memories.contains(stream->GetName()))
		memories.erase(stream->GetName());
}

ShiftableString::MemoryManager &ShiftableString::MemoryManager::GetInstance()
{
	static ShiftableString::MemoryManager _inst;
	return _inst;
}

mule::Data::Space::FragmentManager &ShiftableString::MemoryManager::GetMemory(xybase::Stream *stream)
{
	return memories[stream->GetName()];
}

size_t ShiftableString::MemoryManager::AssignFor(xybase::Stream *stream, const std::u16string &str, size_t size)
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
			return cache->second[str] = GetMemory(stream).Alloc(size, 16);
		}
	}

	return assign[name][str] = GetMemory(stream).Alloc(size, 16);
}

void ShiftableString::MemoryManager::SaveFreeSpace()
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
			fprintf(file, "%X %u\n", frag->GetBegining(), frag->GetSize());
		}
	}
	fclose(file);
}
