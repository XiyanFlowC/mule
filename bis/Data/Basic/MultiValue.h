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
				/**
				 * @brief 引用类型引用计数器
				*/
				int *useCounter = nullptr;

				/**
				 * @brief 表示元素长度。
				*/
				size_t length;

			public:
				static const MultiValue MV_NULL;

				std::map<std::string, MultiValue> metadata;

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

				~MultiValue();

				MultiValue();

				MultiValue(ValueType type, int length = 0);

				MultiValue(const MultiValue &pattern);

				MultiValue(MultiValue &&movee) noexcept;

				MultiValue(const std::string &value);

				MultiValue(const double value);

				MultiValue(const uint64_t value);

				MultiValue(const int64_t value);

				MultiValue(const int size, const MultiValue *array);

				MultiValue(const std::map<MultiValue, MultiValue> map);

				void SetType(ValueType type, int length = 0);

				ValueType GetType() const;

				/**
				 * @brief 转换为字符串。
				 * @return 字符串表示值。
				*/
				std::string ToString() const;

				/**
				 * @brief 序列化为字符串。
				 * @return 序列化结果。
				*/
				std::string Stringfy() const;

				/**
				 * @brief 反序列化字符串。
				 * @param value 要解析的字符串对象。
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

				const MultiValue &operator= (const MultiValue &&movee) noexcept;

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
