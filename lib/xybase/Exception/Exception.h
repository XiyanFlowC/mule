#pragma once

#ifndef XY_EXCEPTION_H__
#define XY_EXCEPTION_H__

#include <exception>
#include <string>

#include "../xyapi.h"

namespace xybase
{
	class XY_API Exception : public std::exception
	{
	public:
		Exception(const std::wstring &message, int err);

		virtual ~Exception();

		virtual const char *what() const noexcept override;

		virtual const std::wstring &GetMessage() const;

		virtual int GetErrorCode() const;

	protected:
		std::wstring message;
		int err;
		char *buf;
	};
}

#endif
