#pragma once

#ifndef XY_IO_EXCEPTION_H__
#define XY_IO_EXCEPTION_H__

#include "RuntimeException.h"
#include <string>

#include "../xyapi.h"

namespace xybase
{
	class IOException : public RuntimeException
	{
	public:
		XY_API IOException(const std::wstring &filename, const std::wstring &message, int err = errno);

		XY_API virtual const std::wstring &GetFileName() const;

		XY_API virtual ~IOException();

	protected:
		std::wstring filename;
	};
}

#endif
