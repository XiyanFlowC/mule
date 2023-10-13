#pragma once

#include <string>
#include <Logger.h>

namespace mule
{
	namespace Lua
	{
		namespace Api
		{
			extern Logger logger;

			std::string SetLocale(std::string locale);

			void RegisterSystemOperations();
		}
	}
}
