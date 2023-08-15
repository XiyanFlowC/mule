#pragma once

#ifndef XY_IO_EXCEPTION_H__
#define XY_IO_EXCEPTION_H__

#include "RuntimeException.h"
#include <string>

namespace xybase
{
	class IOException : public RuntimeException
	{
	public:
		IOException(const std::u16string &filename, const std::u16string &message, int err = errno);

		virtual const std::u16string &GetFileName() const;

	protected:
		std::u16string filename;
	};
}

#endif
