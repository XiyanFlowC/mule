#pragma once

#ifndef XY_TEXT_STREAM
#define XY_TEXT_STREAM

#include <cstdio>
#include <string>

#include "Stream.h"

namespace xybase
{
	class TextStream : public Stream
	{
		std::u16string name;
	public:
		FILE *stream;

		TextStream(std::string path, int mode);

		virtual ~TextStream();

		virtual void Flush() override;

		void Close() override;

		virtual std::u16string GetName() const override;

		virtual std::string ReadLine();

		virtual char ReadChar();
		virtual uint8_t ReadUInt8() override;
		virtual int8_t ReadInt8() override;
		virtual uint16_t ReadUInt16() override;
		virtual int16_t ReadInt16() override;
		virtual uint32_t ReadUInt32() override;
		virtual int32_t ReadInt32() override;
		virtual uint64_t ReadUInt64() override;
		virtual int64_t ReadInt64() override;
		virtual float ReadFloat() override;
		virtual double ReadDouble() override;
		virtual void Write(char value);
		virtual void Write(uint8_t value) override;
		virtual void Write(int8_t value) override;
		virtual void Write(uint16_t value) override;
		virtual void Write(int16_t value) override;
		virtual void Write(uint32_t value) override;
		virtual void Write(int32_t value) override;
		virtual void Write(uint64_t value) override;
		virtual void Write(int64_t value) override;
		virtual void Write(float value) override;
		virtual void Write(double value) override;
		virtual void Write(const std::string &value) override;
		virtual void Write(const char *value);
		virtual size_t Tell() const override;
		virtual void Seek(long long offset, int mode) override;
	private:
		virtual std::string ReadString() override;

		virtual void ReadBytes(char *buffer, int limit) override;

		virtual void Write(const char *buffer, size_t size) override;

		bool open;
	};
}

#endif
