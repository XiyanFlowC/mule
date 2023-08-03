#pragma once

#ifndef OBJECT_H__
#define OBJECT_H__

#include <map>
#include <string>
#include "MultiValue.h"
#include "../../xybase/Stream.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class Object
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
					virtual void OnRealmEnter(Object *realm, std::string name) = 0;

					/**
					 * @brief 离开具名领域
					 * @param realm 
					 * @param name 
					*/
					virtual void OnRealmExit(Object *realm, std::string name) = 0;

					/**
					 * @brief 进入索引领域
					 * @param realm 
					 * @param idx 
					*/
					virtual void OnRealmEnter(Object *realm, int idx) = 0;

					/**
					 * @brief 离开索引领域
					 * @param realm 
					 * @param idx 
					*/
					virtual void OnRealmExit(Object *realm, int idx) = 0;

					virtual void OnDataRead(const MultiValue &value) = 0;

					virtual MultiValue OnDataWrite() = 0;

					virtual void AppendMetadata(std::map<std::string, MultiValue> metadata);

					virtual void AppendMetadatum(std::string name, const MultiValue &value);
				};

				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) = 0;

				virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) = 0;

				virtual size_t Size() const = 0;

				/**
				 * @brief 获得类型名（运行期反射协助用）
				 * @return 类型名
				*/
				virtual std::string GetTypeName() const = 0;

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
			};
		}
	}
}

#endif
