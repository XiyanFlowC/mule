#pragma once

#ifndef XY_MEMORY_STREAM_H__
#define XY_MEMORY_STREAM_H__
#include "xyapi.h"
#include "StreamBasic.h"
namespace xybase
{
	class MemoryStream : public StreamBasic
	{
	protected:
		void *memoryPivot;
		size_t size;
		size_t cursor;
		std::u16string name;

	public:
		XY_API virtual ~MemoryStream();
		XY_API MemoryStream(void *memoryBasePointer, size_t size, const std::u16string &name);

		XY_API void Flush() override;
		XY_API std::u16string GetName() const override;
		XY_API void ReadBytes(char *buffer, size_t limit) override;
		XY_API size_t Tell() const override;
		XY_API bool IsEof() const noexcept override;
		XY_API void Seek(long long offset, SeekMode mode) override;
		XY_API void Close() override;
		XY_API void Write(const char *buffer, size_t limit) override;
	};
}

#endif
