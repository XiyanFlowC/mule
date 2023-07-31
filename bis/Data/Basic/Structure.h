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
			 * @brief �ṹ���ࡣ���ڱ�ʾ�ֶζ�ȡ���İ�װ�����������е�Field������ص�Read��Write�������С�
			 * �˽ṹ����Ӧ��ע�ᵽObjectManager���ṩReference�ɼ��ԡ�
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

					// ͨ�� Object �̳�
					virtual std::string GetTypeName() const override;
				};

				/**
				 * @brief ��ʼ��һ���µĽṹ
				 * @param name ����
				*/
				Structure(std::string name);

				~Structure();

				/**
				 * @brief �ڽṹ��β������һ���ֶ�
				 * @param name �ֶε�����
				 * @param field �ֶδ�������ʵ��
				*/
				void AppendField(const std::string &name, mule::Data::Basic::Object *field);

				/**
				 * @brief ��ָ���������ж�ȡ
				 * @param stream Ҫ���ж�ȡ����
				 * @param dataHandler ���ݴ�����
				*/
				virtual void Read(xybase::Stream *stream, Basic::Object::DataHandler *dataHandler) override;

				/**
				 * @brief ��ָ����������д��
				 * @param stream Ҫ����д�����
				 * @param dataHandler ���ݴ�����
				*/
				virtual void Write(xybase::Stream *stream, Basic::Object::DataHandler *dataHandler) override;

				/**
				 * @brief ��ȡ�ṹ�������С
				 * @return �ṹ������Ĵ�С
				*/
				virtual size_t Size() const override;
				// ͨ�� Object �̳�
				virtual std::string GetTypeName() const override;
			protected:


				std::list<Field *> fields;

				std::string name;
			};
		}
	}
}

#endif