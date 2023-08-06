#pragma once

#ifndef XY_INVALID_PARAMETER_EXCEPTION_H__
#define XY_INVALID_PARAMETER_EXCEPTION_H__

#include <exception>
#include <string>

namespace xybase
{
	class InvalidParameterException : public std::exception
	{
	public:
		InvalidParameterException(const std::string & param, const std::string &message, int err);

		virtual const char *what() const noexcept override;

		virtual const std::string &GetMessage() const;

		virtual int GetErrorCode() const;

		virtual const std::string &GetParam() const;

	protected:
		std::string param;
		std::string message;
		int err;
	};
}
#endif
