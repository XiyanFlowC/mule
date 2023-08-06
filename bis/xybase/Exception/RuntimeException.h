#pragma once

#ifndef XY_RUNTIME_EXCEPTION_H__
#define XY_RUNTIME_EXCEPTION_H__

#include <exception>
#include <string>

namespace xybase
{
	class RuntimeException : public std::exception
	{
	public:
		RuntimeException(const std::string &message, int err);

		virtual const char *what() const noexcept override;

		virtual const std::string &GetMessage() const;

		virtual int GetErrorCode() const;

	protected:
		std::string message;
		int err;
	};
}

#endif
