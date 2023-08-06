#pragma once

#ifndef IO_EXCEPTION_H__
#define IO_EXCEPTION_H__

#include <exception>
#include <string>

namespace xybase
{
	class IOException : public std::exception
	{
	public:
		IOException(const std::string &filename, const std::string &message, int err = errno);

		virtual const char *what() const noexcept override;

		virtual const std::string &GetFileName() const;

		virtual const std::string &GetMessage() const;

		virtual int GetErrorCode() const;

	protected:
		std::string filename;
		std::string message;
		std::string text;
		int err;
	};
}

#endif
