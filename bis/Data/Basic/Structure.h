#pragma once

#ifndef STRUCTURE_H__
#define STRUCTURE_H__

#include "Object.h"
#include <list>

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			/**
			 * @brief 结构体类。用于表示字段读取链的包装。这样，链中的Field以其独特的Read和Write得以运行。
			 * 此结构体理应被注册到ObjectManager以提供Reference可见性。
			*/
			class Structure : public Basic::Object
			{
			public:

				class Field : public Basic::Object
				{
				public:
					virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;
					virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) override;
					virtual size_t Size() const override;

					const std::string &GetName() const;
					const Object * const GetObject() const;

					Field(std::string name, Object *obj);
				protected:

					std::string name;

					Object *object;

					// 通过 Object 继承
					virtual std::string GetTypeName() const override;
				};

				/**
				 * @brief 初始化一个新的结构
				 * @param name 名字
				*/
				Structure(std::string name);

				~Structure();

				/**
				 * @brief 在结构体尾部附加一个字段
				 * @param name 字段的名字
				 * @param field 字段处理器的实例
				*/
				void AppendField(const std::string &name, mule::Data::Basic::Object *field);

				/**
				 * @brief 对指定的流进行读取
				 * @param stream 要进行读取的流
				 * @param dataHandler 数据处理器
				*/
				virtual void Read(xybase::Stream *stream, Basic::Object::DataHandler *dataHandler) override;

				/**
				 * @brief 对指定的流进行写入
				 * @param stream 要进行写入的流
				 * @param dataHandler 数据处理器
				*/
				virtual void Write(xybase::Stream *stream, Basic::Object::DataHandler *dataHandler) override;

				/**
				 * @brief 获取结构体整体大小
				 * @return 结构体整体的大小
				*/
				virtual size_t Size() const override;
				// 通过 Object 继承
				virtual std::string GetTypeName() const override;
			protected:


				std::list<Field *> fields;

				std::string name;
			};
		}
	}
}

#endif