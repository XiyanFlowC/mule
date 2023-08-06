#include "InvalidOperationException.h"

xybase::InvalidOperationException::InvalidOperationException(const std::string &message, int err)
	: err(err), message(message)
{
}

const char *xybase::InvalidOperationException::what() const noexcept
{
	return message.c_str();
}

const std::string &xybase::InvalidOperationException::GetMessage() const
{
	return message;
}

int xybase::InvalidOperationException::GetErrorCode() const
{
	return err;
}
