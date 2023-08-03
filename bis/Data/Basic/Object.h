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
					 * @brief �����������
					 * @param realm ������
					 * @param name 
					*/
					virtual void OnRealmEnter(Object *realm, std::string name) = 0;

					/**
					 * @brief �뿪��������
					 * @param realm 
					 * @param name 
					*/
					virtual void OnRealmExit(Object *realm, std::string name) = 0;

					/**
					 * @brief ������������
					 * @param realm 
					 * @param idx 
					*/
					virtual void OnRealmEnter(Object *realm, int idx) = 0;

					/**
					 * @brief �뿪��������
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
				 * @brief ����������������ڷ���Э���ã�
				 * @return ������
				*/
				virtual std::string GetTypeName() const = 0;

				/**
				 * @brief �����һ�ζ�ȡ�Ĵ�С
				 * @return ��һ�ζ�ȡ������ȡ���ֽ���
				*/
				virtual size_t GetLastSize() const;

				/**
				 * @brief ��������������Ҫд�룬��Ҫʹ�õĿռ�
				 * @param obj Ҫ����������
				 * @return �������õĴ�С���
				*/
				virtual size_t EvalSize(const MultiValue &obj) const;
			};
		}
	}
}

#endif
