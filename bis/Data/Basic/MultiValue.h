#pragma once

#ifndef MULTI_VALUE_H__
#define MULTI_VALUE_H__

#include <string>
#include <map>
#include <cstdint>
#include "../../Exception/Exception.h"
#include "../../Exception/InvalidParameterException.h"
#include "../../Exception/InvalidOperationException.h"
#include "../../xybase/StringBuilder.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{

			class InvalidRValueException : public mule::Exception::Exception
			{
			public:
				InvalidRValueException(std::string description, const char *file, int line);
			};

			/**
			 * @brief ����;ֵ����
			*/
			class MultiValue
			{
				/**
				 * @brief �����������ü�����
				*/
				int *useCounter = nullptr;

				/**
				 * @brief ��ʾԪ�س��ȡ�
				*/
				size_t length;

			public:
				static const MultiValue MV_NULL;

				std::map<std::string, MultiValue> metadata;

				/**
				 * @brief ����;ֵ����
				*/
				enum ValueType
				{
					/**
					 * @brief ��ȷ����ֵ��Lua ������ʱ�ȼ� nil
					*/
					MVT_NULL,
					/**
					 * @brief ����
					*/
					MVT_INT,
					/**
					 * @brief �޷�������
					*/
					MVT_UINT,
					/**
					 * @brief ʵ��
					*/
					MVT_REAL,
					/**
					 * @brief �ַ���
					*/
					MVT_STRING,
					/**
					 * @brief ӳ��
					*/
					MVT_MAP,
					/**
					 * @brief ����
					*/
					MVT_ARRAY
				} type;

				union MultipleValue
				{
					uint64_t unsignedValue;
					int64_t signedValue;
					double realValue;
					std::string *stringValue;
					std::map<MultiValue, MultiValue> *mapValue;
					MultiValue *arrayValue;
				} value;

				~MultiValue();

				MultiValue();

				MultiValue(ValueType type, int length = 0);

				MultiValue(const MultiValue &pattern);

				MultiValue(MultiValue &&movee) noexcept;

				MultiValue(const std::string &value);

				MultiValue(const double value);

				MultiValue(const uint64_t value);

				MultiValue(const uint32_t value);

				MultiValue(const uint16_t value);

				MultiValue(const uint8_t value);

				MultiValue(const int64_t value);

				MultiValue(const int32_t value);

				MultiValue(const int16_t value);

				MultiValue(const int8_t value);

				MultiValue(const int size, const MultiValue *array);

				MultiValue(const std::map<MultiValue, MultiValue> map);

				void SetType(ValueType type, int length = 0);

				ValueType GetType() const;

				size_t GetLength() const;

				/**
				 * @brief ת��Ϊ�ַ�����
				 * @return �ַ�����ʾֵ��
				*/
				std::string ToString() const;

				/**
				 * @brief ���л�Ϊ�ַ�����
				 * @return ���л������
				*/
				std::string Stringfy() const;

				/**
				 * @brief �����л��ַ�����
				 * @param value Ҫ�������ַ�������
				*/
				static MultiValue Parse(const std::string &value);

				void SetValue(const std::string &value);

				void SetValue(const double value);

				void SetValue(const uint64_t value);

				void SetValue(const int64_t value);

				void SetValue(const int size, const MultiValue *array);

				void SetValue(const std::map<MultiValue, MultiValue> &map);

				void SetValue();

				MultiValue operator+ (const MultiValue &rvalue) const;

				MultiValue operator- (const MultiValue &rvalue) const;

				MultiValue operator* (const MultiValue &rvalue) const;

				MultiValue operator/ (const MultiValue &rvalue) const;

				const MultiValue &operator= (const MultiValue &rvalue);

				const MultiValue &operator= (MultiValue &&movee) noexcept;

				bool operator== (const MultiValue &rvalue) const;

				bool operator!= (const MultiValue &rvalue) const;

				bool operator< (const MultiValue &rvalue) const;

				bool operator<= (const MultiValue &rvalue) const;

				bool operator> (const MultiValue &rvalue) const;

				bool operator>= (const MultiValue &rvalue) const;
			private:
				void ParseInt(const std::string &value);

				void ParseString(const std::string &value, bool isBare = false);

				void ParseReal(const std::string &value);

				void DisposeOldValue();

				std::string Stringfy(std::string str) const;
			};
		}
	}
}

#endif
