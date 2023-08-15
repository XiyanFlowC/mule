#pragma once

#ifndef XY_INVALID_PARAMETER_EXCEPTION_H__
#define XY_INVALID_PARAMETER_EXCEPTION_H__

#include "Exception.h"
#include <string>

namespace xybase
{
	class InvalidParameterException : public Exception
	{
	public:
		InvalidParameterException(const std::u16string & param, const std::u16string &message, int err);

		virtual const std::u16string &GetParam() const;

	protected:
		std::u16string param;
	};
}
#endif
