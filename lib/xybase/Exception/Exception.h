#pragma once

#ifndef XY_EXCEPTION_H__
#define XY_EXCEPTION_H__

#include <exception>
#include <string>

namespace xybase
{
	class Exception : public std::exception
	{
	public:
		Exception(const std::u16string &message, int err);

		virtual ~Exception();

		virtual const char *what() const noexcept override;

		virtual const std::u16string &GetMessage() const;

		virtual int GetErrorCode() const;

	protected:
		std::u16string message;
		int err;
		char *buf;
	};
}

#endif