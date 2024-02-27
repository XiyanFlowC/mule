#pragma once

#ifndef XY_INVALID_OPERATION_EXCEPTION_H__
#define XY_INVALID_OPERATION_EXCEPTION_H__

#include "RuntimeException.h"
#include <string>

#include "../xyapi.h"

namespace xybase
{
	const int INVALID_OPERATION_EXCEPTION_LEAD = 0x1000'0000;

	class InvalidOperationException : public RuntimeException
	{
	public:
		XY_API InvalidOperationException(const std::wstring &message, int err);

		XY_API virtual ~InvalidOperationException();
	};
}

#endif
