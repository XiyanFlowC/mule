#pragma once
#ifndef FIELD_H__
#define FIELD_H__

#include <cstdio>
#include <string>
#include "../../xybase/Stream.h"
#include "DataHandler.h"
#include "Object.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{

			/**
			 * @brief 字段。设计为可以相连而成为读取链。
			 * 处在读取链中的字段不再可以如同Object一样Read和Write，这是对结构性的保护。
			*/
			class Field : public Object
			{
			public:
				Field();

				~Field();

				/**
				 * @brief 字段读写上下文。用于保存字段读写的上下文信息。
				*/
				struct FieldReadWriteContext
				{
				public:
					bool imcomplete;
					void *context;
					xybase::Stream *stream;
					DataHandler *handler;

					FieldReadWriteContext(xybase::Stream *stream, DataHandler *textStream);
					FieldReadWriteContext(FieldReadWriteContext &pattern);
				};
				std::string name;

				Field *next;

				/**
				 * @brief 读入。
				 * @param context 读写上下文。
				 * @return 成功为0.
				*/
				int Read(FieldReadWriteContext &context);


				/**
				 * @brief 写出。
				 * @param context 读写上下文
				 * @return 成功为0.
				*/
				int Write(FieldReadWriteContext &context) const;

				/**
				 * @brief 获取此字段单个的大小。
				 * @return 改字段的大小
				*/
				virtual size_t FieldSize() const;

				/**
				 * @brief Object风格读入。
				 * @param stream 要读的流。
				 * @param dataHandler 要使用的数据处理器。
				*/
				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;

				/**
				 * @brief Object风格写出。
				 * @param stream 要写的流。
				 * @param dataHandler 要使用的数据处理器。
				*/
				virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) const override;

				/**
				 * @brief 获取Field的整体大小（包括后继的大小）
				 * @return Field的大小和后继Field大小的总和的和
				*/
				virtual size_t Size() const override;

			protected:
				virtual MultiValue DoRead(FieldReadWriteContext &context) = 0;

				virtual int DoWrite(FieldReadWriteContext &context, const MultiValue &value) const = 0;
			};
		}
	}
}

#endif
