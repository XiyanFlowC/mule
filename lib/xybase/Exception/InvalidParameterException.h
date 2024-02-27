#pragma once

#ifndef XY_INVALID_PARAMETER_EXCEPTION_H__
#define XY_INVALID_PARAMETER_EXCEPTION_H__

#include "Exception.h"
#include <string>

#include "../xyapi.h"

namespace xybase
{
	const int INVALID_PARAMETER_EXCPETION_LEAD = 0x2000'0000;

	class InvalidParameterException : public Exception
	{
	public:
		XY_API InvalidParameterException(const std::wstring & param, const std::wstring &message, int err);

		XY_API virtual const std::wstring &GetParam() const;

		XY_API virtual ~InvalidParameterException();

	protected:
		std::wstring param;
	};
}
#endif
