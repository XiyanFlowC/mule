#pragma once

#ifndef XY_NOT_IMPLEMENTED_EXCEPTION_H__
#define XY_NOT_IMPLEMENTED_EXCEPTION_H__

#include "Exception.h"

namespace xybase
{
	class NotImplementedException : public Exception
	{
	public:
		NotImplementedException();
	};
}

#endif // !XY_NOT_IMPLEMENTED_EXCEPTION_H__
