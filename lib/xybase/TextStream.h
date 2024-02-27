#pragma once

#ifndef XY_TEXT_STREAM
#define XY_TEXT_STREAM

#include <fstream>
#include <string>

#include "Stream.h"

#include "xyapi.h"

namespace xybase
{
	class TextStream : public Stream
	{
		std::wstring name;
		std::fstream stream;
	public:

		XY_API TextStream(std::string path, std::ios::openmode mode, std::string localeStr = {});

		XY_API virtual ~TextStream();

		XY_API virtual void Flush() override;

		XY_API void Close() override;

		XY_API virtual std::u16string GetName() const override;

		XY_API virtual bool ReadLine(std::string &out);

		XY_API virtual char ReadChar();
		XY_API virtual uint8_t ReadUInt8() override;
		XY_API virtual int8_t ReadInt8() override;
		XY_API virtual uint16_t ReadUInt16() override;
		XY_API virtual int16_t ReadInt16() override;
		XY_API virtual uint32_t ReadUInt32() override;
		XY_API virtual int32_t ReadInt32() override;
		XY_API virtual uint64_t ReadUInt64() override;
		XY_API virtual int64_t ReadInt64() override;
		XY_API virtual float ReadFloat() override;
		XY_API virtual double ReadDouble() override;
		XY_API virtual void Write(char value);
		XY_API virtual void Write(uint8_t value) override;
		XY_API virtual void Write(int8_t value) override;
		XY_API virtual void Write(uint16_t value) override;
		XY_API virtual void Write(int16_t value) override;
		XY_API virtual void Write(uint32_t value) override;
		XY_API virtual void Write(int32_t value) override;
		XY_API virtual void Write(uint64_t value) override;
		XY_API virtual void Write(int64_t value) override;
		XY_API virtual void Write(float value) override;
		XY_API virtual void Write(double value) override;
		XY_API virtual void Write(const std::string &value) override;
		XY_API virtual void Write(const char *value);
		XY_API virtual size_t Tell() const override;
		XY_API virtual void Seek(long long offset, SeekMode mode) override;

		XY_API virtual bool IsEof() const noexcept override;
	private:
		XY_API virtual std::string ReadString() override;

		XY_API virtual void ReadBytes(char *buffer, size_t limit) override;

		XY_API virtual void Write(const char *buffer, size_t size) override;
	};
}

#endif
