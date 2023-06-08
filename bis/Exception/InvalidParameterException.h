#pragma once

#ifndef INVALID_PARAMETER_EXCEPTION_H__
#define INVALID_PARAMETER_EXCEPTION_H__

#include "Exception.h"

namespace mule
{
	namespace Exception
	{

		class InvalidParameterException : public Exception
		{
		public:
			InvalidParameterException(std::string parameter_name, std::string description, const char *name, int line);
		};
	}
}

#endif
