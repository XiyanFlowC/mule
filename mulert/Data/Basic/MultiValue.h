#pragma once

#ifndef MULTI_VALUE_H__
#define MULTI_VALUE_H__

#include <string>
#include <map>
#include <cstdint>
#include <format>
#include <StringBuilder.h>
#include <Exception/Exception.h>
#include <Exception/InvalidParameterException.h>
#include <Exception/InvalidOperationException.h>

#include "../../mulert_api.h"

namespace mule
{
	namespace Data
	{
		namespace Basic
		{
			class InvalidRValueException : public xybase::Exception
			{
			public:
				MULERT_API InvalidRValueException(std::wstring description, int line);

				MULERT_API ~InvalidRValueException();
			};

			/**
			 * @brief 多用途值对象
			*/
			class MULERT_API MultiValue
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
				//FIXME: 禁用警告：尽快寻找代替方案
#pragma warning(push)
#pragma warning(disable: 4251)
				std::map<std::u16string, MultiValue> metadata;
#pragma warning(pop)
				
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
					std::u16string *stringValue;
					std::map<MultiValue, MultiValue> *mapValue;
					MultiValue *arrayValue;
				} value;

				~MultiValue();

				MultiValue();

				MultiValue(ValueType type, int length = 0);

				MultiValue(const MultiValue &pattern);

				MultiValue(MultiValue &&movee) noexcept;

				MultiValue(const std::u16string &value);

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

				bool IsType(ValueType type) const;

				size_t GetLength() const;

				/**
				 * @brief 转换为字符串。
				 * @return 字符串表示值。
				*/
				std::wstring ToString() const;

				/**
				 * @brief 序列化为字符串。
				 * @return 序列化结果。
				*/
				std::wstring Stringfy() const;

				/**
				 * @brief 反序列化字符串。
				 * @param value 要解析的字符串对象。
				*/
				static MultiValue Parse(const std::wstring &value);

				static MultiValue Parse(const std::u16string &value);

				void SetValue(const std::u16string &value);

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

				MultiValue &operator[] (const MultiValue &key) const;
			private:
				void ParseInt(const std::u16string &value);

				void ParseString(const std::u16string &value, bool isBare = false);

				void ParseReal(const std::u16string &value);

				void DisposeOldValue();

				static std::wstring Stringfy(std::wstring str);
			};
		}
	}
}

template<>
struct std::formatter<mule::Data::Basic::MultiValue, wchar_t>
{
	template<class FormatContext>
	auto format(const mule::Data::Basic::MultiValue &val, FormatContext &context)
	{
		return std::format_to(context.out(), val.ToString());
	}
};

#endif
