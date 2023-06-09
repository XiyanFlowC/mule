#pragma once

#ifndef STRING_BUILDER_H__
#define STRING_BUILDER_H__

#include <cstdlib>
#include "../Exception/Exception.h"
#include "../Exception/InvalidParameterException.h"

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

		size_t Length();

		size_t Size();

		void Resize(size_t size);

		Tunit *ToString();
	protected:

		Tunit *buffer;
		size_t currentSize, length;
	};

	template <typename Tunit>
	StringBuilder<Tunit>::StringBuilder(int initialSize, const Tunit *initialContent)
	{
		currentSize = initialSize ? initialSize : 16;
		length = 0;

		buffer = (Tunit *)malloc(currentSize * sizeof(Tunit));
		if (buffer == nullptr) throw mule::Exception::Exception("Memory allocation falied.", __FILE__, __LINE__);

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

		if (length == currentSize)
		{
			Resize(currentSize << 1);
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

	template <typename Tunit>
	size_t StringBuilder<Tunit>::Length()
	{
		return length;
	}

	template <typename Tunit>
	size_t StringBuilder<Tunit>::Size()
	{
		return currentSize;
	}

	template<typename Tunit>
	void StringBuilder<Tunit>::Resize(size_t size)
	{
		if (size < length) throw mule::Exception::InvalidParameterException("size", "Cannot resize to a size smaller than current length.", __FILE__, __LINE__);

		Tunit *tmp = (Tunit *)realloc(buffer, size);
		if (tmp == nullptr)
		{
			tmp = (Tunit *)malloc(size * sizeof(Tunit)); // try again in another way.
			if (tmp == nullptr) throw mule::Exception::Exception("Memory re-allocation failed.", __FILE__, __LINE__);
			memcpy(tmp, buffer, currentSize);
			free(buffer);
		}
		buffer = tmp;
		currentSize = size;
	}

	template<typename Tunit>
	Tunit *StringBuilder<Tunit>::ToString()
	{
		buffer[length] = (Tunit)0;
		return buffer;
	}

}

#endif
