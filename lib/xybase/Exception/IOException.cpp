#include "IOException.h"
#include "../xyutils.h"

xybase::IOException::IOException(const std::wstring &filename, const std::wstring &message, int err)
	: RuntimeException(message + L" (When operate file [" + filename + L"])", err), filename(filename)
{
}

const std::wstring &xybase::IOException::GetFileName() const
{
	return filename;
}

xybase::IOException::~IOException()
{
}
