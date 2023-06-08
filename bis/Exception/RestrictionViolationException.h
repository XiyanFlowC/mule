#pragma once

#ifndef RESTRICTION_VIOLATION_EXCEPTION_H__
#define RESTRICTION_VIOLATION_EXCEPTION_H__

#include "Exception.h"

namespace mule
{
	namespace Exception
	{

		class RestrictionViolationException : Exception
		{
		public:
			RestrictionViolationException(std::string name, std::string value, std::string actualValue, const char *file, int line);
		};
	}
}

#endif
