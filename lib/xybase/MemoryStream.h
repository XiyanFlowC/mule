#pragma once

#ifndef XY_MEMORY_STREAM_H__
#define XY_MEMORY_STREAM_H__

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
		virtual ~MemoryStream() {}
		MemoryStream(void *memoryBasePointer, size_t size, const std::u16string &name);

		void Flush() override;
		std::u16string GetName() const override;
		void ReadBytes(char *buffer, size_t limit) override;
		size_t Tell() const override;
		bool IsEof() const noexcept override;
		void Seek(long long offset, SeekMode mode) override;
		void Close() override;
		void Write(const char *buffer, size_t limit) override;
	};
}

#endif
