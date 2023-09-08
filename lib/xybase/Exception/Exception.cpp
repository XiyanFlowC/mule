#include "Exception.h"
#include <cstring>
#include "../xyutils.h"

xybase::Exception::Exception(const std::u16string &message, int err)
	: message(message), err(err)
{
	auto ret = xybase::string::to_utf8(message);
	buf = new char[ret.size() + 1];
	memcpy(buf, ret.c_str(), ret.size());
	buf[ret.size()] = '\0';
}

xybase::Exception::~Exception()
{
	delete[] buf;
}

const char *xybase::Exception::what() const noexcept
{
	return buf;
}

const std::u16string &xybase::Exception::GetMessage() const
{
	return message;
}

int xybase::Exception::GetErrorCode() const
{
	return err;
}
