#pragma once

#ifndef XY_NOT_IMPLEMENTED_EXCEPTION_H__
#define XY_NOT_IMPLEMENTED_EXCEPTION_H__

#include "RuntimeException.h"

namespace xybase
{
	class NotImplementedException : public RuntimeException
	{
	public:
		NotImplementedException();
	};
}

#endif // !XY_NOT_IMPLEMENTED_EXCEPTION_H__
