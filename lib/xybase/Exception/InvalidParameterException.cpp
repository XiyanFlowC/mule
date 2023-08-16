#include "InvalidParameterException.h"

#include "../xystring.h"

xybase::InvalidParameterException::InvalidParameterException(const std::u16string &param, const std::u16string &message, int err)
	: param(param), Exception(u"Parameter: [" + param + u"] is invalid: " + message, err)
{
}


const std::u16string &xybase::InvalidParameterException::GetParam() const
{
	return param;
}