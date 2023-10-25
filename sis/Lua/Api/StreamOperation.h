#pragma once

#include <string>

namespace mule
{
	namespace Lua
	{
		namespace Api
		{
			int OpenStream(std::string path, std::string openMode);

			int CloseStream(int id);

			int ExportStream(std::string path, int id);

			int ExtractStream(std::string path, int offset, int length, int id);

			int ImportStream(std::string path, int id);

			int PatchStream(std::string path, int offset, int length, int id);

			int StreamOverStream(int streamId, std::string applier);

			void RegisterStreamOperationFunctions();
		}
	}
}
