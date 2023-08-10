#include "IOException.h"

xybase::IOException::IOException(const std::string &filename, const std::string &message, int err)
	: RuntimeException(message, err), filename(filename)
{
	text = message + " (When operate file [" + filename + "])";
}

const char *xybase::IOException::what() const noexcept
{
	return text.c_str();
}

const std::string &xybase::IOException::GetFileName() const
{
	return filename;
}

const std::string &xybase::IOException::GetMessage() const
{
	return filename;
}

int xybase::IOException::GetErrorCode() const
{
	return err;
}
