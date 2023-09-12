#pragma once

#ifndef BINARY_DATA_H__
#define BINARY_DATA_H__

#include <memory>

#include "../../mulert_api.h"

namespace mule
{
	namespace Data
	{
		namespace Storage
		{
			class MULERT_API BinaryData
			{
				std::shared_ptr<char[]> data;
				size_t length;

			public:

				BinaryData(char *data, size_t length, bool duplicate = true);

				const char *GetData() const noexcept;

				size_t GetLength() const noexcept;

				/**
				 * @brief 管理对应数据
				 * @param data 数据
				 * @param length 数据长度
				 * @param duplicate 是否复制（不对源操作）
				*/
				void SetData(char *data, size_t length, bool duplicate = true);

				/**
				 * @brief 管理对应数据
				 * @param data 数据
				 * @param length 数据长度
				*/
				void SetData(const char *data, size_t length);
			};
		}
	}
}

#endif
