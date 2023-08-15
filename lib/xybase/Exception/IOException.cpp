#include "IOException.h"
#include "../xyutils.h"

xybase::IOException::IOException(const std::u16string &filename, const std::u16string &message, int err)
	: RuntimeException(message + u" (When operate file [" + filename + u"])", err), filename(filename)
{
}

const std::u16string &xybase::IOException::GetFileName() const
{
	return filename;
}
