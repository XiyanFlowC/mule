#pragma once

#ifndef MULTI_VALUE_H__
#define MULTI_VALUE_H__

#include <string>
#include <map>
#include <cstdint>
#include <format>
#include <optional>
#include <span>
#include <type_traits>
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
#ifndef _WIN32
				/* Make Linux happy */
				MultiValue(const long long value);

				/* Make Linux happy */
				MultiValue(const unsigned long long value);
#endif

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
				// TODO: remove const here
				MultiValue &operator[] (const MultiValue &key) const;

				// ── 类型安全读取门面 ─────────────────────────────────────────────
				// header-inline、非虚、无新增数据成员 → 不改 sizeof / 成员偏移 / vtable，
				// 对既有二进制插件与 ABI 零影响。读取失败（类型不符或坏状态）一律返回
				// std::nullopt，不抛异常；复合访问器（GetMap/GetArray）因语义是"必须是"，
				// 在类型不符 / 空指针时抛出。

				/// 精确取：仅当存储类型恰为 T 时返回，否则 nullopt。
				/// 支持的 T：int64_t / uint64_t / double / std::u16string / std::span<const MultiValue>
				template <typename T>
				std::optional<T> GetValue() const
				{
					if constexpr (std::is_same_v<T, int64_t>)
					{
						if (type != MVT_INT) return std::nullopt;
						return value.signedValue;
					}
					else if constexpr (std::is_same_v<T, uint64_t>)
					{
						if (type != MVT_UINT) return std::nullopt;
						return value.unsignedValue;
					}
					else if constexpr (std::is_same_v<T, double>)
					{
						if (type != MVT_REAL) return std::nullopt;
						return value.realValue;
					}
					else if constexpr (std::is_same_v<T, std::u16string>)
					{
						if (type != MVT_STRING || value.stringValue == nullptr) return std::nullopt;
						return *value.stringValue;
					}
					else if constexpr (std::is_same_v<T, std::span<const MultiValue>>)
					{
						if (type != MVT_ARRAY || value.arrayValue == nullptr) return std::nullopt;
						return std::span<const MultiValue>(value.arrayValue, length);
					}
					else
					{
						static_assert(!std::is_same_v<T, T>, "GetValue<T>: unsupported type T");
					}
				}

				/// 有界强制转换。
				///  - int↔uint：取模（C++20 定义良好）
				///  - int/uint→real：允许，但可能损失精度
				///  - real→int：拒绝（几乎必然有损），交由调用者自行决定如何转换
				///  - string↔数字：拒绝
				template <typename T>
				std::optional<T> CastValue() const
				{
					if constexpr (std::is_same_v<T, int64_t>)
					{
						if (type == MVT_INT) return value.signedValue;
						if (type == MVT_UINT) return static_cast<int64_t>(value.unsignedValue);
						return std::nullopt;
					}
					else if constexpr (std::is_same_v<T, uint64_t>)
					{
						if (type == MVT_UINT) return value.unsignedValue;
						if (type == MVT_INT) return static_cast<uint64_t>(value.signedValue);
						return std::nullopt;
					}
					else if constexpr (std::is_same_v<T, double>)
					{
						if (type == MVT_REAL) return value.realValue;
						if (type == MVT_INT) return static_cast<double>(value.signedValue);
						if (type == MVT_UINT) return static_cast<double>(value.unsignedValue);
						return std::nullopt;
					}
					else if constexpr (std::is_same_v<T, std::u16string>)
					{
						if (type == MVT_STRING && value.stringValue != nullptr) return *value.stringValue;
						return std::nullopt;
					}
					else
					{
						static_assert(!std::is_same_v<T, T>, "CastValue<T>: unsupported type T");
					}
				}

				/// 数组非拥有视图。非数组 / 空指针 → 抛。定义在 MultiValue.cpp（与 SetValue 等一致）。
				std::span<const MultiValue> GetArray() const;

				/// 映射引用（不复制整张表）。非映射 / 空指针 → 抛。定义在 MultiValue.cpp。
				const std::map<MultiValue, MultiValue> &GetMap() const;

				/// 开放元数据读取：key 可为任意字符串（加新键无需改头文件），
				/// 用 Cast 语义读出（元数据来源多样，类型不总精确匹配更顺手）。
				/// 以 const std::u16string& 传参：与 metadata 的键类型一致，
				/// 免去临时构造，也与 FileContainerBasic::GetMetadata 的惯例一致。
				template <typename T>
				std::optional<T> GetMetadata(const std::u16string &key) const
				{
					auto it = metadata.find(key);
					if (it == metadata.end()) return std::nullopt;
					return it->second.CastValue<T>();
				}

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
