#pragma once

#ifndef STRUCTURE_H__
#define STRUCTURE_H__

#include "Object.h"
#include "Field.h"
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
				/**
				 * @brief ��ʼ��һ���µĽṹ
				*/
				Structure();

				/**
				 * @brief �ڽṹ��β������һ���ֶ�
				 * @param name �ֶε�����
				 * @param object �ֶδ�������ʵ��
				*/
				void AppendField(const std::string &name, mule::Data::Basic::Field *field);

				/**
				 * @brief ��ָ���������ж�ȡ
				 * @param stream Ҫ���ж�ȡ����
				 * @param dataHandler ���ݴ�����
				*/
				virtual void Read(xybase::Stream *stream, Basic::DataHandler *dataHandler) override;

				/**
				 * @brief ��ָ����������д��
				 * @param stream Ҫ����д�����
				 * @param dataHandler ���ݴ�����
				*/
				virtual void Write(xybase::Stream *stream, Basic::DataHandler *dataHandler) const override;

				/**
				 * @brief ��ȡ�ṹ�������С
				 * @return �ṹ������Ĵ�С
				*/
				virtual size_t Size() const override;

				void Froze();
			protected:
				Basic::Field *firstField, *lastField;

				std::list<std::string> nameList;

				bool frozen;
			};
		}
	}
}

#endif