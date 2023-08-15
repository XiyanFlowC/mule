#include "OutOfRangeException.h"

xybase::OutOfRangeException::OutOfRangeException(const std::u16string &message, int err)
	: RuntimeException(message, err)
{
}
