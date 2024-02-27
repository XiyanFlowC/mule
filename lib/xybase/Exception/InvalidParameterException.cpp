#include "InvalidParameterException.h"

#include "../xystring.h"

xybase::InvalidParameterException::InvalidParameterException(const std::wstring &param, const std::wstring &message, int err)
	: param(param), Exception(L"Parameter: [" + param + L"] is invalid: " + message, INVALID_PARAMETER_EXCPETION_LEAD | err)
{
}


const std::wstring &xybase::InvalidParameterException::GetParam() const
{
	return param;
}

xybase::InvalidParameterException::~InvalidParameterException()
{
}
