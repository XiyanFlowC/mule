#pragma once

#ifndef BINARY_DATA_H__
#define BINARY_DATA_H__

#include <memory>

#include "../mulert_api.h"

namespace mule
{
	namespace Storage
	{
		class BinaryData
		{
			std::shared_ptr<char[]> data;
			size_t length;

		public:

			/**
			 * @brief 初始化二进制数据。
			 * @param data 数据所在缓冲区。
			 * @param length 数据长度。
			 * @param duplicate 是否复制，若为否，则将传入的data直接托管（shared_ptr）。
			*/
			MULERT_API BinaryData(char *data, size_t length, bool duplicate = true);

			MULERT_API ~BinaryData();

			MULERT_API const char *GetData() const noexcept;

			MULERT_API size_t GetLength() const noexcept;

			/**
			 * @brief 管理对应数据
			 * @param data 数据
			 * @param length 数据长度
			 * @param duplicate 是否复制（不对源操作）
			*/
			MULERT_API void SetData(char *data, size_t length, bool duplicate = true);

			/**
			 * @brief 管理对应数据
			 * @param data 数据
			 * @param length 数据长度
			*/
			MULERT_API void SetData(const char *data, size_t length);
		};
	}
}

#endif
