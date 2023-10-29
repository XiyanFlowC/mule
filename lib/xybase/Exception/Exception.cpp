#include "Exception.h"
#include <cstring>
#include "../xyutils.h"
#include "../xystring.h"

xybase::Exception::Exception(const std::wstring &message, int err)
	: message(message), err(err)
{
	auto ret = xybase::string::to_string(message);
	buf = new char[ret.size() + 1];
	memcpy(buf, ret.c_str(), ret.size() + 1);
}

xybase::Exception::~Exception()
{
	delete[] buf;
}

const char *xybase::Exception::what() const noexcept
{
	return buf;
}

const std::wstring &xybase::Exception::GetMessage() const
{
	return message;
}

int xybase::Exception::GetErrorCode() const
{
	return err;
}
