#pragma once

#ifndef XY_STREAM_H__
#define XY_STREAM_H__

#include <cstdint>
#include <string>

#include "xyapi.h"

namespace xybase
{
	const char endianTester[] = { '\xFF', 0};

	extern XY_API bool bigEndianSystem;

	/*
	* 处理文件I/O的抽象类。用于平台隔离。
	* 避免使用C/C++默认的流式I/O来正确处理BE/LE
	* 确保上层不需要处理BE/LE、寻址差异等。
	*/
	class XY_API Stream
	{
	protected:

		bool isBigEndian = false;

	public:

		virtual void Flush() = 0;

		virtual std::u16string GetName() const = 0;

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

		virtual void ReadBytes(char* buffer, size_t limit) = 0;

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

		virtual size_t Tell() const = 0;

		/**
		 * @brief 寻址方式（基准）
		*/
		enum SeekMode
		{
			SM_BEGIN,
			SM_CURRENT,
			SM_END
		};
		
		/**
		 * @brief 寻址
		 * @param offset 偏移
		 * @param mode 寻址模式
		*/
		virtual void Seek(long long offset, SeekMode mode) = 0;

		virtual void Close() = 0;
	};
}

#endif