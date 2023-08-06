#include "RuntimeException.h"

xybase::RuntimeException::RuntimeException(const std::string &message, int err)
	: err(err), message(message)
{
}

const char *xybase::RuntimeException::what() const noexcept
{
	return message.c_str();
}

const std::string &xybase::RuntimeException::GetMessage() const
{
	return message;
}

int xybase::RuntimeException::GetErrorCode() const
{
	return err;
}
