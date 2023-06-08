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
			 * @brief �ֶΡ����Ϊ������������Ϊ��ȡ����
			 * ���ڶ�ȡ���е��ֶβ��ٿ�����ͬObjectһ��Read��Write�����ǶԽṹ�Եı�����
			*/
			class Field : public Object
			{
			public:
				Field();

				~Field();

				/**
				 * @brief �ֶζ�д�����ġ����ڱ����ֶζ�д����������Ϣ��
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
				 * @brief ���롣
				 * @param context ��д�����ġ�
				 * @return �ɹ�Ϊ0.
				*/
				int Read(FieldReadWriteContext &context);


				/**
				 * @brief д����
				 * @param context ��д������
				 * @return �ɹ�Ϊ0.
				*/
				int Write(FieldReadWriteContext &context) const;

				/**
				 * @brief ��ȡ���ֶε����Ĵ�С��
				 * @return ���ֶεĴ�С
				*/
				virtual size_t FieldSize() const;

				/**
				 * @brief Object�����롣
				 * @param stream Ҫ��������
				 * @param dataHandler Ҫʹ�õ����ݴ�������
				*/
				virtual void Read(xybase::Stream *stream, DataHandler *dataHandler) override;

				/**
				 * @brief Object���д����
				 * @param stream Ҫд������
				 * @param dataHandler Ҫʹ�õ����ݴ�������
				*/
				virtual void Write(xybase::Stream *stream, DataHandler *dataHandler) const override;

				/**
				 * @brief ��ȡField�������С��������̵Ĵ�С��
				 * @return Field�Ĵ�С�ͺ��Field��С���ܺ͵ĺ�
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
