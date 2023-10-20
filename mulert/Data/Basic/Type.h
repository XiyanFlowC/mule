#pragma once

#include <map>
#include <string>
#include <TextStream.h>
#include <Stream.h>
#include "MultiValue.h"

#include "../../mulert_api.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class MULERT_API Type
			{
			public:

				class MULERT_API Handler
				{
				public:
					virtual ~Handler();

					/**
					 * @brief 进入具名领域
					 * @param realm 领域名
					 * @param name
					*/
					virtual void OnRealmEnter(Type *realm, const std::u16string &name) = 0;

					/**
					 * @brief 离开具名领域
					 * @param realm
					 * @param name
					*/
					virtual void OnRealmExit(Type *realm, const std::u16string &name) = 0;

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
				};

				class MULERT_API DataHandler : virtual public Handler
				{
				public:
					virtual ~DataHandler();

					virtual void OnSheetReadStart() = 0;

					virtual void OnSheetReadEnd() = 0;

					virtual void OnDataRead(const mule::Data::Basic::MultiValue &value) = 0;

					virtual void AppendMetadata(std::map<std::u16string, mule::Data::Basic::MultiValue> metadata);

					virtual void AppendMetadatum(std::u16string name, const mule::Data::Basic::MultiValue &value);

					void SetOutStream(xybase::TextStream *stream);
				protected:
					xybase::TextStream *outstream;
				};

				class MULERT_API FileHandler : virtual public Handler
				{
				public:
					virtual ~FileHandler();

					virtual void OnSheetWriteStart() = 0;

					virtual void OnSheetWriteEnd() = 0;

					virtual const mule::Data::Basic::MultiValue OnDataWrite() = 0;

					void SetInStream(xybase::TextStream *stream);

				protected:
					xybase::TextStream *instream;
				};

				virtual ~Type();

				virtual size_t Size() const = 0;

				/**
				 * @brief 获得数据类型名
				 * @return 类型名
				*/
				virtual std::u16string GetDataType() const = 0;

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
				virtual size_t EvalSize(const mule::Data::Basic::MultiValue &obj) const;

				/**
				 * @brief 指示该类型是否为复合类型
				 * @return
				*/
				virtual bool IsComposite() const;

				/**
				 * @brief 从数据流读出数据、处理数据
				 * @param stream 数据流（数据源）
				 * @param dataHandler 数据处理器
				*/
				virtual void Read(xybase::Stream *stream, mule::Data::Basic::Type::DataHandler *dataHandler) = 0;

				/**
				 * @brief 从数据源获取数据，写入数据流
				 * @param stream 数据流
				 * @param dataHandler 数据处理器（数据源）
				*/
				virtual void Write(xybase::Stream *stream, mule::Data::Basic::Type::FileHandler * fileHandler) = 0;
			};
		}
	}
}
