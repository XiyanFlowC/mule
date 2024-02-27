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
	class BinaryStream : public xybase::StreamBasic
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
		XY_API BinaryStream(std::wstring path, const wchar_t *mode = L"rb+", bool isBigEndian = false);
		XY_API virtual ~BinaryStream();

		XY_API virtual void Flush() override;
		XY_API virtual std::u16string GetName() const override;
		XY_API virtual void ReadBytes(char *buffer, size_t limit) override;
		XY_API virtual void Write(const char *buffer, size_t size) override;
		XY_API virtual size_t Tell() const override;
		XY_API virtual void Seek(long long offset, SeekMode mode = SM_BEGIN) override;
		XY_API virtual void Close() override;
		XY_API virtual bool IsEof() const noexcept override;
	};
}

#endif
