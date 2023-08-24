#include "ShiftableReferrence.h"

#include <xyutils.h>
#include <Stream/ElfStream.h>

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
	size_t loc = stream->Tell();
	referent->Write(stream, dataHandler);
	const MultiValue &mv = dataHandler->OnDataWrite();
	stream->Write((int32_t)MemoryManager::GetInstance().GetMemory(stream).Alloc(referent->EvalSize(mv), 4));
	stream->Seek(loc, 0);
}

size_t ShiftableReferrence::Size() const
{
	return size_t(4);
}

std::u16string ShiftableReferrence::GetTypeName() const
{
	return referent->GetTypeName() + u'&';
}

void ShiftableReferrence::StreamDispose(xybase::Stream *stream)
{
	MemoryManager::GetInstance().DisposeStream(stream);
}

Type *ShiftableReferrence::ShiftableReferrenceCreator::DoCreateObject(std::u16string info)
{
	if (info.ends_with(u"&"))
	{
		Type *inner = TypeManager::GetInstance().GetOrCreateType(info.substr(info.length() - 1));
		if (inner == nullptr) return nullptr;

		ShiftableReferrence *ret = new ShiftableReferrence();
		ret->referent = inner;
		return ret;
	}
	return nullptr;
}

void ShiftableReferrence::MemoryManager::DisposeStream(xybase::Stream *stream)
{
	if (memories.contains(stream))
		memories.erase(stream);
}

ShiftableReferrence::MemoryManager &ShiftableReferrence::MemoryManager::GetInstance()
{
	static ShiftableReferrence::MemoryManager _inst;
	return _inst;
}

mule::Data::Space::FragmentManager ShiftableReferrence::MemoryManager::GetMemory(xybase::Stream *stream)
{
	return memories[stream];
}
