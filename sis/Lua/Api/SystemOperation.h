#pragma once

#include <string>
#include <Logger.h>
#include <Data/Basic/MultiValue.h>

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

			mule::Data::Basic::MultiValue Configuration(std::string name, mule::Data::Basic::MultiValue mv);

			void RegisterSystemOperations();
		}
	}
}
