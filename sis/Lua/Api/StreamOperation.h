#pragma once

#include <string>

namespace mule
{
	namespace Lua
	{
		namespace Api
		{
			int OpenStream(std::u8string path, std::u8string openMode);

			int CloseStream(int id);

			int ExportStream(std::u8string path, int id);

			int ExtractStream(std::u8string path, int offset, int length, int id);

			int ImportStream(std::u8string path, int id);

			int PatchStream(std::u8string path, int offset, int length, int id);

			int StreamOverStream(int streamId, std::u8string applier);

			void RegisterStreamOperationFunctions();
		}
	}
}
