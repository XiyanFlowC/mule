#pragma once

#ifndef XYUTILS_H__
#define XYUTILS_H__
#define XY_ALIGN(n, b) ((n) < 0 ? (n) & ~((b) - 1) : ((n + (b) - 1) & ~((b) - 1))

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <string>

#include "StringBuilder.h"

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
		std::u16string to_utf16(std::string str);

		/**
		 * @brief Convert utf-16 to utf-8
		 * @param str utf-16 string
		 * @return utf-8 string
		*/
		std::string to_utf8(std::u16string str);

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

	namespace io
	{
		enum AccessMode
		{
			PM_READ = 0x1,
			PM_WRITE = 0x2,
			PM_READWRITE = 0X3,
			PM_EXECUTE = 0x4,
		};

		int access(const char *path, AccessMode mode);

		int mkdir(const char *path);
	}
}

#endif // !XYUTILS_H__
