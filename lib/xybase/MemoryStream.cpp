#include "MemoryStream.h"
#include <cstring>

xybase::MemoryStream::~MemoryStream()
{
}

xybase::MemoryStream::MemoryStream(void *memoryBasePointer, size_t size, const std::u16string &name)
{
	memoryPivot = memoryBasePointer;
	this->size = size;
	cursor = 0;
	this->name = name;
}

void xybase::MemoryStream::Flush()
{
	// Do nothing.
}

std::u16string xybase::MemoryStream::GetName() const
{
	return name;
}

void xybase::MemoryStream::ReadBytes(char *buffer, size_t limit)
{
	if (cursor + limit > size) throw xybase::InvalidParameterException(L"limit", L"Too large.", 440100);

	memcpy(buffer, ((char *)memoryPivot) + cursor, limit);
	cursor += limit;
}

size_t xybase::MemoryStream::Tell() const
{
	return cursor;
}

bool xybase::MemoryStream::IsEof() const noexcept
{
	return cursor == size;
}

void xybase::MemoryStream::Seek(long long offset, SeekMode mode)
{
	switch (mode)
	{
	case xybase::Stream::SM_BEGIN:
		if (offset > size) throw xybase::InvalidParameterException(L"offset", L"Exceeds stream size.", 440110);
		cursor = offset;
		break;
	case xybase::Stream::SM_CURRENT:
		if (offset + cursor > size) throw xybase::InvalidParameterException(L"offset", L"Exceeds stream size.", 440111);
		cursor += offset;
		break;
	case xybase::Stream::SM_END:
		if (offset > size) throw xybase::InvalidParameterException(L"offset", L"Exceeds stream size.", 440112);
		cursor = size + offset;
		break;
	default:
		if (offset > size) throw xybase::InvalidParameterException(L"mode", L"Invalid seek mode.", 440199);
		break;
	}
}

void xybase::MemoryStream::Close()
{
	// do nothing.
}

void xybase::MemoryStream::Write(const char *buffer, size_t limit)
{
	if (cursor + limit > size) throw xybase::InvalidParameterException(L"limit", L"Too large.", 440120);

	memcpy(((char *)memoryPivot) + cursor, buffer, limit);
	cursor += limit;
}
