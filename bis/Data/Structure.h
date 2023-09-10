#pragma once

#ifndef STRUCTURE_H__
#define STRUCTURE_H__

#include "Basic/Type.h"
#include <list>

namespace mule
{
	namespace Data
	{
		/**
		 * @brief 结构体类。用于表示字段读取链的包装。
		 * 此结构体理应被注册到ObjectManager以提供Reference可见性。
		*/
		class Structure : public Basic::Type
		{
		public:

			class Field : public Basic::Type
			{
			public:
				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
				virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) override;
				virtual size_t Size() const override;

				const std::u16string &GetName() const;
				const Type *const GetObject() const;

				Field(std::u16string name, Type *obj);
			protected:

				std::u16string name;

				Type *object;

				virtual std::u16string GetDataType() const override;
			};

			/**
			 * @brief 初始化一个新的结构
			 * @param name 名字
			*/
			Structure(std::u16string name);

			~Structure();

			/**
			 * @brief 在结构体尾部附加一个字段
			 * @param name 字段的名字
			 * @param field 字段处理器的实例
			*/
			void AppendField(const std::u16string &name, mule::Data::Basic::Type *field);

			/**
			 * @brief 对指定的流进行读取
			 * @param stream 要进行读取的流
			 * @param dataHandler 数据处理器
			*/
			virtual void Read(xybase::Stream *stream, Basic::Type::DataHandler *dataHandler) override;

			/**
			 * @brief 对指定的流进行写入
			 * @param stream 要进行写入的流
			 * @param dataHandler 数据处理器
			*/
			virtual void Write(xybase::Stream *stream, Basic::Type::DataHandler *dataHandler) override;

			/**
			 * @brief 获取结构体整体大小
			 * @return 结构体整体的大小
			*/
			virtual size_t Size() const override;

			virtual std::u16string GetName() const;

			virtual std::u16string GetDataType() const override;

			virtual bool IsComposite() const override;
		protected:

			std::list<Field *> fields;

			std::u16string name;
		};
	}
}

#endif