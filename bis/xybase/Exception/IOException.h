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
		IOException(const std::string &filename, const std::string &message, int err = errno);

		virtual const char *what() const noexcept override;

		virtual const std::string &GetFileName() const;

		virtual const std::string &GetMessage() const;

		virtual int GetErrorCode() const;

	protected:
		std::string filename;
		std::string text;
	};
}

#endif
