#pragma once

#ifndef EXCEPTION_H__
#define EXCEPTION_H__

#include <string>
#include <exception>
#include <typeinfo>

#define MULE_FILE __FILE__

namespace mule
{
	namespace Exception
	{
		class Exception : std::exception
		{
			std::string description;

			int line;

			const char *file;

		public:
			Exception(std::string description, const char *file, int line) noexcept;

			virtual const char *what() const;

			const char *What() const;
		};
	}
}

#endif
