#pragma once

#ifndef RUNTIME_EXCEPTION_H__
#define RUNTIME_EXCEPTION_H__

#include <exception>
#include <string>

namespace xybase
{
	class InvalidOperationException : public std::exception
	{
	public:
		InvalidOperationException(const std::string &message, int err);

		virtual const char *what() const noexcept override;

		virtual const std::string &GetMessage() const;

		virtual int GetErrorCode() const;

	protected:
		std::string message;
		int err;
	};
}

#endif
