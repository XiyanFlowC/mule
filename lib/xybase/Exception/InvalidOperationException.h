#pragma once

#ifndef XY_INVALID_OPERATION_EXCEPTION_H__
#define XY_INVALID_OPERATION_EXCEPTION_H__

#include "RuntimeException.h"
#include <string>

#include "../xyapi.h"

namespace xybase
{
	const int INVALID_OPERATION_EXCEPTION_LEAD = 0x0001'0000;

	class XY_API InvalidOperationException : public RuntimeException
	{
	public:
		InvalidOperationException(const std::wstring &message, int err);
	};
}

#endif
