#pragma once

#ifndef STRUCTURE_H__
#define STRUCTURE_H__

#include "Basic/Type.h"
#include "../mulert_api.h"
#include <list>

namespace mule
{
	namespace Data
	{
		/**
		 * @brief 结构体类。包含多个字段的集合，按顺序依次触发字段区域的进出，并触发字段对应类型的读写。
		 * 任何新建的结构体理应被注册到ObjectManager以提供Reference可见性。
		*/
		class Structure : public Basic::Type
		{
		public:
			/**
			 * @brief 字段。结构体内部类。用于触发字段的进出事件和读写事件。
			 * 一个结构体持有多个字段类，并有结构体负责生命周期。
			*/
			class Field : public Basic::Type
			{
			public:
				MULERT_API virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
				MULERT_API virtual void Write(xybase::Stream *stream, FileHandler *fileHandler) override;
				MULERT_API virtual size_t Size() const override;

				MULERT_API const std::u16string &GetName() const;
				MULERT_API const Type *const GetObject() const;

				MULERT_API Field(std::u16string name, Type *obj);
				MULERT_API ~Field();
			protected:
				/**
				 * @brief 字段的名字
				*/
				std::u16string name;

				/**
				 * @brief 字段读写事件触发时，处理事件的类型实例
				*/
				Type *object;

				virtual std::u16string GetDataType() const override;
			};

			/**
			 * @brief 初始化一个新的结构
			 * @param name 名字
			*/
			MULERT_API Structure(std::u16string name);

			MULERT_API virtual ~Structure();

			/**
			 * @brief 在结构体尾部附加一个字段
			 * @param name 字段的名字
			 * @param field 字段处理器的实例
			*/
			MULERT_API void AppendField(const std::u16string &name, mule::Data::Basic::Type *field);

			/**
			 * @brief 对指定的流进行读取
			 * @param stream 要进行读取的流
			 * @param dataHandler 数据处理器
			*/
			MULERT_API virtual void Read(xybase::Stream *stream, Basic::Type::DataHandler *dataHandler) override;

			/**
			 * @brief 对指定的流进行写入
			 * @param stream 要进行写入的流
			 * @param dataHandler 数据处理器
			*/
			MULERT_API virtual void Write(xybase::Stream *stream, Basic::Type::FileHandler *fileHandler) override;

			/**
			 * @brief 获取结构体整体大小
			 * @return 结构体整体的大小
			*/
			MULERT_API virtual size_t Size() const override;

			MULERT_API virtual std::u16string GetName() const;

			MULERT_API virtual std::u16string GetDataType() const override;

			MULERT_API virtual bool IsComposite() const override;
		protected:

			/**
			 * @brief 结构体中的字段
			*/
			std::list<Field *> fields;

			/**
			 * @brief 结构体自身的名字（类型名）
			*/
			std::u16string name;
		};
	}
}

#endif