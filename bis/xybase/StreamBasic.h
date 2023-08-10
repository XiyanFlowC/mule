#pragma once

#ifndef XY_STREAM_BASIC_H__
#define XY_STREAM_BASIC_H__

#include "StringBuilder.h"
#include "Stream.h"

namespace xybase
{
	/**
	 * @brief 提供基于ReadBytes和Write的其他函数实现，简化Stream实现类需编写的方法。
	*/
	class StreamBasic : public Stream
	{
	public:
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
		virtual void Write(const char *buffer, size_t limit) override = 0;
	};
}

#endif