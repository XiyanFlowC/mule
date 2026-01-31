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

			int Log(std::u8string msg, mule::Data::Basic::MultiValue mv);

			int LogInit(int level, bool enableColour);

			int LogSetOutput(std::u8string filename);

			int LogSetErrorOutput(std::u8string filename);

			int LoadPlugin(std::u8string path);

			mule::Data::Basic::MultiValue Configuration(std::u8string name, mule::Data::Basic::MultiValue mv);

			int EraseConfiguration(std::u8string name);

			void RegisterSystemOperations();

			int DefineStructure(std::u8string name, mule::Data::Basic::MultiValue def);
		}
	}
}
