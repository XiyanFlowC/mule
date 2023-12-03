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

			int Confirm(std::u8string word);

			int PrintPlugins();

			int Log(std::u8string msg);

			int LoadPlugin(std::u8string path);

			mule::Data::Basic::MultiValue Configuration(std::u8string name, mule::Data::Basic::MultiValue mv);

			void RegisterSystemOperations();
		}
	}
}
