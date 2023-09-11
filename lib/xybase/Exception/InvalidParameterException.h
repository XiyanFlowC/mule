#pragma once

#ifndef XY_INVALID_PARAMETER_EXCEPTION_H__
#define XY_INVALID_PARAMETER_EXCEPTION_H__

#include "Exception.h"
#include <string>

#include "../xyapi.h"

namespace xybase
{
	const int INVALID_PARAMETER_EXCPETION_LEAD = 0x0001'0000;

	class XY_API InvalidParameterException : public Exception
	{
	public:
		InvalidParameterException(const std::wstring & param, const std::wstring &message, int err);

		virtual const std::wstring &GetParam() const;

	protected:
		std::wstring param;
	};
}
#endif
