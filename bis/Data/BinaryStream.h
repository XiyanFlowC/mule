#pragma once

#ifndef BINARY_STREAM_H__
#define BINARY_STREAM_H__

#include <cstdint>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include "../xybase/Stream.h"
#include "../Exception/Exception.h"
#include "../Exception/InvalidOperationException.h"

namespace mule
{
	namespace Data
	{

		class BinaryStream : public xybase::Stream
		{
		protected:
#ifndef BIG_ENDIAN_SYSTEM
			static const bool bigEndianSystem = false;
#else
			static const bool bigEndianSystem = true;
#endif
			FILE *stream;
			bool isOpen, isBigEndian;

		public:
			BinaryStream(const char *path, bool isBigEndian = false);
			~BinaryStream();

			// Í¨¹ý Stream ¼Ì³Ð
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
			virtual std::string ReadString() override;
			virtual void ReadBytes(char *buffer, int limit) override;
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
			virtual void Write(const char *buffer, size_t size) override;
			virtual size_t Tell() override;
			virtual void Seek(long long offset, int mode = SEEK_SET) override;
			virtual void Close() override;
		};
	}
}

#endif
