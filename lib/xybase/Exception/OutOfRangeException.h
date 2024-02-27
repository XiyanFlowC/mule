#pragma once

#ifndef XY_OUT_OF_RANGE_H__
#define XY_OUT_OF_RANGE_H__

#include "RuntimeException.h"

#include "../xyapi.h"

namespace xybase
{
	const int OUT_OF_RANGE_EXCEPTION_LEAD = 0x3000'0000;

	class OutOfRangeException : public RuntimeException
	{
	public:
		XY_API OutOfRangeException(const std::wstring & message, int err);

		XY_API virtual ~OutOfRangeException();
	};
}

#endif // !XY_OUT_OF_RANGE_H__
