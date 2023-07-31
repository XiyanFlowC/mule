#pragma once

#ifndef NOT_IMPLEMENTED_EXCEPTION_H__
#define NOT_IMPLEMENTED_EXCEPTION_H__

#include "Exception.h"

namespace mule
{
	namespace Exception
	{
		class NotImplementedException : public Exception
		{
		public:
			NotImplementedException(const char *name, int line);
		};
	}
}

#endif /* End of NOT_IMPLEMENTED_EXCEPTION_H__ */
