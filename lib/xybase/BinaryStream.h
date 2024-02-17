#pragma once

#ifndef XY_BINARY_STREAM_H__
#define XY_BINARY_STREAM_H__

#include <cstdint>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include "StreamBasic.h"
#include "Exception/IOException.h"
#include "Exception/InvalidOperationException.h"
#include "StringBuilder.h"

#include "xyapi.h"

namespace xybase
{
	class XY_API BinaryStream : public xybase::StreamBasic
	{
	protected:
		FILE *stream;
		bool isOpen;
		std::wstring name;

	public:
		/**
		 * @brief 创建二进制流用于读写
		 * @param path 目标文件路径
		 * @param mode 打开模式
		 * @param isBigEndian 是否为大端序文件
		*/
		BinaryStream(std::wstring path, const wchar_t *mode = L"rb+", bool isBigEndian = false);
		~BinaryStream();

		virtual void Flush() override;
		virtual std::u16string GetName() const override;
		virtual void ReadBytes(char *buffer, size_t limit) override;
		virtual void Write(const char *buffer, size_t size) override;
		virtual size_t Tell() const override;
		virtual void Seek(long long offset, SeekMode mode = SM_BEGIN) override;
		virtual void Close() override;
		virtual bool IsEof() const noexcept override;
	};
}

#endif
