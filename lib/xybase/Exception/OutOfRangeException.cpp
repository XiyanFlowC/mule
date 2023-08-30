#include "OutOfRangeException.h"

xybase::OutOfRangeException::OutOfRangeException(const std::u16string &message, int err)
	: RuntimeException(message, OUT_OF_RANGE_EXCEPTION_LEAD | err)
{
}
