#include "InvalidOperationException.h"

#include "../xystring.h"

xybase::InvalidOperationException::InvalidOperationException(const std::u16string &message, int err)
	: message(message), err(err)
{
}

const char *xybase::InvalidOperationException::what() const noexcept
{
	return xybase::string::to_string(message).c_str();
}

const std::u16string &xybase::InvalidOperationException::GetMessage() const
{
	return message;
}

int xybase::InvalidOperationException::GetErrorCode() const
{
	return err;
}
