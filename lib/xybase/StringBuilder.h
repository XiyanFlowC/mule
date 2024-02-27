#pragma once

#ifndef XY_STRING_BUILDER_H__
#define XY_STRING_BUILDER_H__

#include <cstring>
#include <cstdlib>
#include "Exception/RuntimeException.h"
#include "Exception/InvalidParameterException.h"

namespace xybase
{
	template <typename Tunit = char>
	class StringBuilder
	{
	public:
		StringBuilder(int initialSize = 0, const Tunit *initialContent = nullptr);

		~StringBuilder();

		StringBuilder &Append(const Tunit ch);

		StringBuilder &Append(const Tunit *str);

		StringBuilder &operator+ (const Tunit ch);

		StringBuilder &operator+ (const Tunit *str);

		StringBuilder &operator+= (const Tunit ch);

		StringBuilder &operator+= (const Tunit *str);

		StringBuilder &operator+= (const std::basic_string<Tunit>& str);

		size_t Length();

		size_t Capacity();

		void Resize(size_t size);

		void Clear();

		Tunit *ToString();
	protected:

		Tunit *buffer;
		size_t capacity, length;
	};

	template <typename Tunit>
	StringBuilder<Tunit>::StringBuilder(int initialSize, const Tunit *initialContent)
	{
		capacity = initialSize ? initialSize : 16;
		length = 0;

		buffer = (Tunit *)malloc(capacity * sizeof(Tunit));
		if (buffer == nullptr) throw RuntimeException(L"Memory allocation failed.", errno);

		if (initialContent != nullptr)
		{
			Append(initialContent);
		}
	}

	template <typename Tunit>
	StringBuilder<Tunit>::~StringBuilder()
	{
		if (buffer != nullptr) free(buffer);
	}

	template <typename Tunit>
	StringBuilder<Tunit> &StringBuilder<Tunit>::Append(const Tunit ch)
	{
		buffer[length++] = ch;

		if (length == capacity)
		{
			Resize(capacity << 1);
		}
		return *this;
	}

	template <typename Tunit>
	StringBuilder<Tunit> &StringBuilder<Tunit>::Append(const Tunit *str)
	{
		while (*str != 0)
		{
			Append(*str++);
		}
		return *this;
	}

	template<typename Tunit>
	StringBuilder<Tunit> &StringBuilder<Tunit>::operator+(const Tunit ch)
	{
		return Append(ch);
	}

	template<typename Tunit>
	StringBuilder<Tunit> &StringBuilder<Tunit>::operator+(const Tunit *str)
	{
		return Append(str);
	}

	template<typename Tunit>
	StringBuilder<Tunit> &StringBuilder<Tunit>::operator+=(const Tunit ch)
	{
		return Append(ch);
	}

	template<typename Tunit>
	StringBuilder<Tunit> &StringBuilder<Tunit>::operator+=(const Tunit *str)
	{
		return Append(str);
	}

	template<typename Tunit>
	StringBuilder<Tunit> &StringBuilder<Tunit>::operator+=(const std::basic_string<Tunit> &str)
	{
		return Append(str.c_str());
	}

	template <typename Tunit>
	size_t StringBuilder<Tunit>::Length()
	{
		return length;
	}

	template <typename Tunit>
	size_t StringBuilder<Tunit>::Capacity()
	{
		return capacity;
	}
	
	template<typename Tunit>
	void StringBuilder<Tunit>::Clear()
	{
		length = 0;
	}

	template<typename Tunit>
	void StringBuilder<Tunit>::Resize(size_t size)
	{
		if (size < length) throw InvalidParameterException(L"size", L"Cannot resize to a size smaller than current length.", __LINE__);

		Tunit *tmp = (Tunit *)realloc(buffer, size * sizeof(Tunit));
		if (tmp == nullptr)
		{
			tmp = (Tunit *)malloc(size * sizeof(Tunit)); // try again in another way.
			if (tmp == nullptr) throw RuntimeException(L"Memory re-allocation failed.", errno);
			memcpy(tmp, buffer, capacity);
			free(buffer);
		}
		buffer = tmp;
		capacity = size;
	}

	template<typename Tunit>
	Tunit *StringBuilder<Tunit>::ToString()
	{
		buffer[length] = (Tunit)0;
		return buffer;
	}

}

#endif
