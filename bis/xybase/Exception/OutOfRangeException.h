#pragma once

#ifndef XY_OUT_OF_RANGE_H__
#define XY_OUT_OF_RANGE_H__

#include "RuntimeException.h"

namespace xybase
{
	class OutOfRangeException : public RuntimeException
	{
	public:
		OutOfRangeException(const std::string & message, int err);
	};
}

#endif // !XY_OUT_OF_RANGE_H__
