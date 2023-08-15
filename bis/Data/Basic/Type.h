#pragma once

#ifndef OBJECT_H__
#define OBJECT_H__

#include <map>
#include <string>
#include <Stream.h>
#include "MultiValue.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class Type
			{
			public:

				class DataHandler
				{
				public:

					virtual void OnSheetReadStart();

					virtual void OnSheetReadEnd();

					virtual void OnSheetWriteStart();

					virtual void OnSheetWriteEnd();

					/**
					 * @brief 进入具名领域
					 * @param realm 领域名
					 * @param name 
					*/
					virtual void OnRealmEnter(Type *realm, std::u16string name) = 0;

					/**
					 * @brief 离开具名领域
					 * @param realm 
					 * @param name 
					*/
					virtual void OnRealmExit(Type *realm, std::u16string name) = 0;

					/**
					 * @brief 进入索引领域
					 * @param realm 
					 * @param idx 
					*/
					virtual void OnRealmEnter(Type *realm, int idx) = 0;

					/**
					 * @brief 离开索引领域
					 * @param realm 
					 * @param idx 
					*/
					virtual void OnRealmExit(Type *realm, int idx) = 0;

					virtual void OnDataRead(const MultiValue &value) = 0;

					virtual MultiValue OnDataWrite() = 0;

					virtual void AppendMetadata(std::map<std::u16string, MultiValue> metadata);

					virtual void AppendMetadatum(std::u16string name, const MultiValue &value);
				};
				virtual ~Type();

				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) = 0;

				virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) = 0;

				virtual size_t Size() const = 0;

				/**
				 * @brief 获得类型名（运行期反射协助用）
				 * @return 类型名
				*/
				virtual std::u16string GetTypeName() const = 0;

				/**
				 * @brief 获得上一次读取的大小
				 * @return 上一次读取操作读取的字节数
				*/
				virtual size_t GetLastSize() const;

				/**
				 * @brief 评估给定数据如要写入，需要使用的空间
				 * @param obj 要评估的数据
				 * @return 评估所得的大小结果
				*/
				virtual size_t EvalSize(const MultiValue &obj) const;

				/**
				 * @brief 指示该类型是否为复合类型
				 * @return 
				*/
				virtual bool IsComposite() const;
			};
		}
	}
}

#endif
