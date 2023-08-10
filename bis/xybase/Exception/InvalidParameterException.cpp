#include "InvalidParameterException.h"

xybase::InvalidParameterException::InvalidParameterException(const std::string &param, const std::string &message, int err)
	: param(param), message(message), err(err)
{
}

const char *xybase::InvalidParameterException::what() const noexcept
{
	return message.c_str();
}

const std::string &xybase::InvalidParameterException::GetMessage() const
{
	return message;
}

int xybase::InvalidParameterException::GetErrorCode() const
{
	return err;
}

const std::string &xybase::InvalidParameterException::GetParam() const
{
	return param;
}
