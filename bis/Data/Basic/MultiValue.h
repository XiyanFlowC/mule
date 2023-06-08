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
			 * @brief 多用途值对象
			*/
			class MultiValue
			{
			public:
				/**
				 * @brief 多用途值类型
				*/
				enum ValueType
				{
					/**
					 * @brief 不确定的值，Lua 互操作时等价 nil
					*/
					MVT_NULL,
					/**
					 * @brief 整数
					*/
					MVT_INT,
					/**
					 * @brief 无符号整数
					*/
					MVT_UINT,
					/**
					 * @brief 实数
					*/
					MVT_REAL,
					/**
					 * @brief 字符串
					*/
					MVT_STRING,
					/**
					 * @brief 映射
					*/
					MVT_MAP,
					/**
					 * @brief 数组
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

				size_t length;

				~MultiValue();

				MultiValue();

				MultiValue(const MultiValue &pattern);

				MultiValue(const std::string &value);

				MultiValue(const double value);

				MultiValue(const uint64_t value);

				MultiValue(const int64_t value);

				MultiValue(const int size, const MultiValue *array);

				MultiValue(const std::map<MultiValue, MultiValue> map);

				std::string ToString() const;

				std::string Stringfy() const;

				void Parse(const std::string &value);

				void SetValue(const std::string &value);

				void SetValue(const double value);

				void SetValue(const uint64_t value);

				void SetValue(const int64_t value);

				void SetValue(const int size, const MultiValue *array);

				void SetValue(const std::map<MultiValue, MultiValue> map);

				void SetValue();

				MultiValue operator+ (const MultiValue &rvalue) const;

				MultiValue operator- (const MultiValue &rvalue) const;

				MultiValue operator* (const MultiValue &rvalue) const;

				MultiValue operator/ (const MultiValue &rvalue) const;

				const MultiValue &operator= (const MultiValue &rvalue);

				bool operator== (const MultiValue &rvalue) const;

				bool operator!= (const MultiValue &rvalue) const;

				bool operator< (const MultiValue &rvalue) const;

				bool operator<= (const MultiValue &rvalue) const;

				bool operator> (const MultiValue &rvalue) const;

				bool operator>= (const MultiValue &rvalue) const;
			private:
				void ParseInt(const std::string &value);

				void ParseString(const std::string &value);

				void ParseReal(const std::string &value);

				void DisposeOldValue();

				std::string Stringfy(std::string str) const;
			};
		}
	}
}

#endif
