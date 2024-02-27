#pragma once

#ifndef XY_NOT_IMPLEMENTED_EXCEPTION_H__
#define XY_NOT_IMPLEMENTED_EXCEPTION_H__

#include "Exception.h"

#include "../xyapi.h"

namespace xybase
{
	class NotImplementedException : public Exception
	{
	public:
		XY_API NotImplementedException();

		XY_API virtual ~NotImplementedException();
	};
}

#endif // !XY_NOT_IMPLEMENTED_EXCEPTION_H__
