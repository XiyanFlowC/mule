#include "OutOfRangeException.h"

xybase::OutOfRangeException::OutOfRangeException(const std::string &message, int err)
	: RuntimeException(message, err)
{
}
