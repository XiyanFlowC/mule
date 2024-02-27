#include "OutOfRangeException.h"

xybase::OutOfRangeException::OutOfRangeException(const std::wstring &message, int err)
	: RuntimeException(message, OUT_OF_RANGE_EXCEPTION_LEAD | err)
{
}

xybase::OutOfRangeException::~OutOfRangeException()
{
}
