#pragma once

#include <string>
#include <Stream.h>

namespace mule
{
	namespace Lua
	{
		namespace Api
		{
			int MountStream(int streamId, std::string type, std::string root);

			int Unmount(std::string root);

			int List(std::string root);

			int ListRoots();

			void RegisterContainerOperationFunctions();
		}
	}
}
