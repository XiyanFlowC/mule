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

			int ImportStream(std::string path, int id);

			int ExportSheet(int streamId, std::string handler, std::string type, std::string tableName, size_t offset, int length);

			int ImportSheet(int streamId, std::string handler, std::string type, std::string tableName, size_t offset, int length);

			void RegisterStreamOperationFunctions();
		}
	}
}
