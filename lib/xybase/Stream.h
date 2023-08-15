#pragma once

#ifndef XY_STREAM_H__
#define XY_STREAM_H__

#include <cstdint>
#include <string>

namespace xybase
{
	/*
	* Abstract class to handle the file I/O. Platform isolation.
	* Avoid to using C/C++ default streaming I/O to handle the BE/LE correctly and
	* ensure the upper layer need not handle BE/LE, addressing, etc.
	*/
	class Stream
	{
	protected:
#ifndef BIG_ENDIAN_SYSTEM
		static const bool bigEndianSystem = false;
#else
		static const bool bigEndianSystem = true;
#endif
		bool isBigEndian = false;

	public:
		virtual ~Stream();

		virtual uint8_t ReadUInt8() = 0;

		virtual int8_t ReadInt8() = 0;

		virtual uint16_t ReadUInt16() = 0;

		virtual int16_t ReadInt16() = 0;

		virtual uint32_t ReadUInt32() = 0;

		virtual int32_t ReadInt32() = 0;

		virtual uint64_t ReadUInt64() = 0;

		virtual int64_t ReadInt64() = 0;

		virtual float ReadFloat() = 0;

		virtual double ReadDouble() = 0;

		virtual std::string ReadString() = 0;

		virtual void ReadBytes(char* buffer, int limit) = 0;

		virtual void Write(uint8_t value) = 0;

		virtual void Write(int8_t value) = 0;

		virtual void Write(uint16_t value) = 0;

		virtual void Write(int16_t value) = 0;

		virtual void Write(uint32_t value) = 0;

		virtual void Write(int32_t value) = 0;

		virtual void Write(uint64_t value) = 0;
		
		virtual void Write(int64_t value) = 0;

		virtual void Write(float value) = 0;

		virtual void Write(double value) = 0;

		virtual void Write(const std::string& value) = 0;

		virtual void Write(const char* buffer, size_t size) = 0;

		virtual size_t Tell() = 0;

		// mode: 0 SET 1 CUR 2 END
		virtual void Seek(long long offset, int mode) = 0;

		virtual void Close() = 0;
	};
}

#endif