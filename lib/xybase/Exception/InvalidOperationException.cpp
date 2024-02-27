#include "InvalidOperationException.h"

#include "../xystring.h"

xybase::InvalidOperationException::InvalidOperationException(const std::wstring &message, int err)
	: RuntimeException(message, INVALID_OPERATION_EXCEPTION_LEAD | err)
{
}

xybase::InvalidOperationException::~InvalidOperationException() {}
