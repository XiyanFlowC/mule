/**
* xystring.h - A platform isolation string processing util
* @author Xiyan
*/
#pragma once

#ifndef XY_XYSTRING_H__
#define XY_XYSTRING_H__

#include <string>
#include <stack>

#include "StringBuilder.h"

namespace xybase
{
	namespace string
	{
		/* Codepoint conversions. */

		/**
		 * @brief Encode a codepoint to utf-8
		 * @param codePoint Code Point.
		 * @return The utf-8 encoded string.
		*/
		std::u8string XY_API to_utf8(long codePoint);

		/**
		 * @brief Encode a codepoint to utf-16
		 * @param codepoint Code Point.
		 * @return The utf-16 encoded string.
		*/
		std::u16string XY_API to_utf16(long codepoint);

		/**
		 * @brief Encode a codepoint to utf-32
		 * @param codepoint Code Point.
		 * @return The utf-32 encoded string.
		*/
		std::u32string XY_API to_utf32(long codepoint);

		/**
		 * @brief Get the codepoint for utf-8 (only process the first character).
		 * @param str The character need to be processed.
		 * @return The codepoint of given character.
		*/
		long XY_API to_codepoint(const std::u8string &str);

		/**
		 * @brief Get the codepoint for utf-16 (only process the first character).
		 * @param str The character need to be processed.
		 * @return The codepoint of given character.
		*/
		long XY_API to_codepoint(const std::u16string &str);

		/* String type conversions. */

		/**
		 * @brief Convert locale string to ucs-4 string
		 * @param str 
		 * @return 
		*/
		std::u32string XY_API to_utf32(const std::string &str) noexcept;

		/**
		 * @brief Convert utf8 string to utf32 string
		 * @param str 
		 * @return 
		*/
		std::u32string XY_API to_utf32(const std::u8string &str) noexcept;

		/**
		 * @brief Convert utf16 string to utf32 string
		 * @param str 
		 * @return 
		*/
		std::u32string XY_API to_utf32(const std::u16string &str) noexcept;

		/**
		 * @brief Identical conversion.
		 * @param str 
		 * @return 
		*/
		std::u32string XY_API to_utf32(const std::u32string &str) noexcept;

		/**
		 * @brief Convert wstring to ucs-4 string
		 * @param str 
		 * @return 
		*/
		std::u32string XY_API to_utf32(const std::wstring &str) noexcept;

		/**
		 * @brief Convert local string to utf-16
		 * @param str 
		 * @return 
		*/
		std::u16string XY_API to_utf16(const std::string &str) noexcept;

		/**
		 * @brief Convert utf-8 to utf-16
		 * @param str utf-8 string
		 * @return utf-16 string
		*/
		std::u16string XY_API to_utf16(const std::u8string &str) noexcept;

		/**
		 * @brief Identical transform
		 * @param str 
		 * @return 
		*/
		std::u16string XY_API to_utf16(const std::u16string &str) noexcept;

		/**
		 * @brief Convert UCS-4 to Utf-16
		 * @param str 
		 * @return 
		*/
		std::u16string XY_API to_utf16(const std::u32string &str) noexcept;

		/**
		 * @brief Convert a wstring to utf16 string
		 * @param str 
		 * @return 
		*/
		std::u16string XY_API to_utf16(const std::wstring &str) noexcept;

		/**
		 * @brief Convert wstring to utf-8 string
		 * @param str 
		 * @return 
		*/
		std::u8string XY_API to_utf8(const std::wstring &str) noexcept;

		/**
		 * @brief Convert u32string to utf8
		 * @param str 
		 * @return 
		*/
		std::u8string XY_API to_utf8(const std::u32string &str) noexcept;

		/**
		 * @brief Convert utf-16 to utf-8
		 * @param str utf-16 string
		 * @return utf-8 string
		*/
		std::u8string XY_API to_utf8(const std::u16string &str) noexcept;

		/**
		 * @brief identical transform
		 * @param str 
		 * @return 
		*/
		std::u8string XY_API to_utf8(const std::u8string &str) noexcept;

		/**
		 * @brief Convert local string to utf-8 string
		 * @param str 
		 * @return 
		*/
		std::u8string XY_API to_utf8(const std::string &str) noexcept;

		/**
		 * @brief 
		 * @param str 
		 * @return 
		*/
		std::wstring XY_API to_wstring(const std::string &str) noexcept;

		/**
		 * @brief Convert to wstring (platform specified)
		 * @param str 
		 * @return 
		*/
		std::wstring XY_API to_wstring(const std::u8string &str) noexcept;

		/**
		 * @brief Convert to wstring
		 * @param str UTF-16 string
		 * @return 
		*/
		std::wstring XY_API to_wstring(const std::u16string &str) noexcept;

		/**
		 * @brief Convert to wstring
		 * @param str UCS-4 string
		 * @return 
		*/
		std::wstring XY_API to_wstring(const std::u32string &str) noexcept;

		/**
		 * @brief Identical conversion
		 * @param str 
		 * @return 
		*/
		std::wstring XY_API to_wstring(const std::wstring &str) noexcept;

		/**
		 * @brief Identical conversion
		 * @param str 
		 * @return 
		*/
		std::string XY_API to_string(const std::string &str) noexcept;

		/**
		 * @brief Convert utf8 string to local string
		 * @param str 
		 * @return 
		*/
		std::string XY_API to_string(const std::u8string &str) noexcept;

		/**
		 * @brief Convert utf16 string to local string
		 * @param str 
		 * @return 
		*/
		std::string XY_API to_string(const std::u16string &str) noexcept;

		/**
		 * @brief Convert utf32 string to local string
		 * @param str 
		 * @return 
		*/
		std::string XY_API to_string(const std::u32string &str) noexcept;

		/**
		 * @brief Convert wstring to local string
		 * @param str 
		 * @return 
		*/
		std::string XY_API to_string(const std::wstring &str) noexcept;

		/**
		 * @brief Set costum string converter (of multibyte string and wide string
		 * @param p_mbcstowcs function converts MBC string to WC string
		 * @param p_wcstombcs function converts WC string to MBC string
		*/
		void XY_API set_string_cvt(std::wstring(*p_mbcstowcs)(const std::string &), std::string(*p_wcstombcs)(const std::wstring &)) noexcept;

		/**
		 * @brief Replace all substring in a string with given replacement. If replacement is a substring of target, then fucked.
		 * @tparam ChT Type of char.
		 * @param original Original string.
		 * @param target The substring need to be replaced.
		 * @param replacement Replacement.
		 * @return Replaced string.
		*/
		template <typename ChT>
		std::basic_string<ChT> replace(std::basic_string<ChT> original, const std::basic_string<ChT> &target, const std::basic_string<ChT> &replacement)
		{
			size_t offset = original.find(target);
			while (offset != std::basic_string<ChT>::npos)
			{
				original.replace(offset, target.length(), replacement);
				offset = original.find(target);
			}
			return original;
		}

		/**
		 * @brief Parse string to integer (up to base 36)
		 * @tparam T Type of the string unit.
		 * @param str String.
		 * @param base Base.
		 * @return Parsed integer.
		*/
		template<typename T = char>
		unsigned long long stoi(const std::basic_string<T> &str, int base = 10)
		{
			const T *ptr = str.c_str();
			const T *end = ptr + str.length();
			unsigned long long ret = 0;

			while (ptr < end)
			{
				if (*ptr >= static_cast<T>('0') && *ptr <= std::min<T>(static_cast<T>('9'), static_cast<T>('0') + base - 1))
				{
					ret = ret * base + *ptr++ - static_cast<T>('0');
				}
				else if (*ptr >= static_cast<T>('a') && *ptr <= std::max<T>(static_cast<T>('a') - 1, static_cast<T>('a') + base - 11))
				{
					ret = ret * base + *ptr++ - static_cast<T>('a') + 10;
				}
				else if (*ptr >= static_cast<T>('A') && *ptr <= std::max<T>(static_cast<T>('A') - 1, static_cast<T>('A') + base - 11))
				{
					ret = ret * base + *ptr++ - static_cast<T>('A') + 10;
				}
				else break;
			}
			return ret;
		}

		template<typename T = char>
		std::basic_string<T> itos(unsigned long long value, int base = 10)
		{
			static const char *pre = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
			if (value == 0) return std::basic_string<T>{'0'};

			std::stack<T> stk;
			while (value)
			{
				stk.push(static_cast<T>(pre[value % base]));
				value /= base;
			}

			StringBuilder<T> sb;
			while (!stk.empty())
			{
				sb.Append(stk.top());
				stk.pop();
			}
			return sb.ToString();
		}

		/**
		 * @brief 【模板用】将原始编码的指定字符串转换为类型指定的编码。
		 * @tparam Ch 目标类型
		 * @tparam SrcCh 原始字符串类型
		 * @param str 要转换的字符串
		 * @return 转完毕的字符串
		*/
		template <typename Ch, typename SrcCh>
		std::basic_string<Ch> to_enc(const std::basic_string<SrcCh> &str)
		{
			if constexpr (std::is_same_v<Ch, wchar_t>) return xybase::string::to_wstring(str);
			else if constexpr (std::is_same_v<Ch, char>) return xybase::string::to_string(str);
			else if constexpr (std::is_same_v<Ch, char8_t>) return xybase::string::to_utf8(str);
			else if constexpr (std::is_same_v<Ch, char16_t>) return xybase::string::to_utf16(str);
			else if constexpr (std::is_same_v<Ch, char32_t>) return xybase::string::to_utf32(str);
			else abort();
		}
	}
}

#endif // !XY_XYSTRING_H__
