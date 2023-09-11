#pragma once

#ifndef XY_IO_EXCEPTION_H__
#define XY_IO_EXCEPTION_H__

#include "RuntimeException.h"
#include <string>

#include "../xyapi.h"

namespace xybase
{
	class XY_API IOException : public RuntimeException
	{
	public:
		IOException(const std::wstring &filename, const std::wstring &message, int err = errno);

		virtual const std::wstring &GetFileName() const;

	protected:
		std::wstring filename;
	};
}

#endif
