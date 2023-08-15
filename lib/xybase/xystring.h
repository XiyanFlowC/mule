#pragma once

#ifndef XY_XYSTRING_H__
#define XY_XYSTRING_H__

#include <string>

namespace xybase
{
	namespace string
	{
		/**
		 * @brief Encode a codepoint to utf-8
		 * @param codePoint Code Point.
		 * @return The utf-8 encoded string.
		*/
		std::string to_utf8(long codePoint);

		/**
		 * @brief Encode a codepoint to utf-16
		 * @param codepoint Code Point.
		 * @return The utf-16 encoded string.
		*/
		std::u16string to_utf16(long codepoint);

		/**
		 * @brief Encode a codepoint to utf-32
		 * @param codepoint Code Point.
		 * @return The utf-32 encoded string.
		*/
		std::u32string to_utf32(long codepoint);

		/**
		 * @brief Get the codepoint for utf-8 (only process the first character).
		 * @param str The character need to be processed.
		 * @return The codepoint of given character.
		*/
		long to_codepoint(std::string str);

		/**
		 * @brief Get the codepoint for utf-16 (only process the first character).
		 * @param str The character need to be processed.
		 * @return The codepoint of given character.
		*/
		long to_codepoint(std::u16string str);

		/**
		 * @brief Convert utf-8 to utf-16
		 * @param str utf-8 string
		 * @return utf-16 string
		*/
		std::u16string to_utf16(std::string str) noexcept;

		/**
		 * @brief Identical transform
		 * @param str 
		 * @return 
		*/
		std::u16string to_utf16(std::u16string str) noexcept;

		/**
		 * @brief Convert utf-16 to utf-8
		 * @param str utf-16 string
		 * @return utf-8 string
		*/
		std::string to_utf8(std::u16string str) noexcept;

		/**
		 * @brief identical transform
		 * @param str 
		 * @return 
		*/
		std::string to_utf8(std::string str) noexcept;

		template<typename T = char>
		unsigned long long stoi(std::basic_string<T> str, int base = 10)
		{
			const T *ptr = str.c_str();
			const T *end = ptr + str.length();
			unsigned long long ret = 0;

			while (ptr <= end)
			{
				if (*ptr >= static_cast<T>('0') || *ptr <= std::min<T>(static_cast<T>('9'), static_cast<T>('0') + base))
				{
					ret = ret * base + *ptr++ - static_cast<T>('0');
				}
				else if (*ptr >= static_cast<T>('a') || *ptr <= std::min<T>(static_cast<T>('a'), static_cast<T>('a') + base - 10))
				{
					ret = ret * base + *ptr++ - static_cast<T>('a') + 10;
				}
				else if (*ptr >= static_cast<T>('A') || *ptr <= std::min<T>(static_cast<T>('A'), static_cast<T>('A') + base - 10))
				{
					ret = ret * base + *ptr++ - static_cast<T>('A') + 10;
				}
				else break;
			}
			return ret;
		}
	}
}

#endif // !XY_XYSTRING_H__
