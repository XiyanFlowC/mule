#pragma once

#include <string>
#include <Stream.h>
#include <Data/Basic/MultiValue.h>

namespace mule
{
	namespace Lua
	{
		namespace Api
		{
			int MountStream(int streamId, std::u8string type, std::u8string root);

			int Unmount(std::u8string root);

			mule::Data::Basic::MultiValue List(std::u8string root);

			mule::Data::Basic::MultiValue ListRoots();

			mule::Data::Basic::MultiValue Metadata(std::u8string path, mule::Data::Basic::MultiValue data);

			int OpenAndMount(std::u8string root, std::u8string param);

			int Remove(std::u8string name);

			void RegisterContainerOperationFunctions();
		}
	}
}
