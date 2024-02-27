#pragma once

#ifndef XY_EXCEPTION_H__
#define XY_EXCEPTION_H__

#include <exception>
#include <string>

#include "../xyapi.h"

namespace xybase
{
	class Exception : public std::exception
	{
	public:
		XY_API Exception(const std::wstring &message, int err);

		XY_API virtual ~Exception();

		XY_API virtual const char *what() const noexcept override;

		XY_API virtual const std::wstring &GetMessage() const;

		XY_API virtual int GetErrorCode() const;

	protected:
		std::wstring message;
		int err;
		char *buf;
	};
}

#endif
