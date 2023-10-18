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

			int Confirm(std::string word);

			int PrintPlugins();

			int Log(std::string msg);

			int LoadPlugin(std::string path);

			void RegisterSystemOperations();
		}
	}
}
