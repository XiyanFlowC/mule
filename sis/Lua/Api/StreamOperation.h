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

			int ExportStreamToResource(std::u8string src, std::u8string out);

			int ExtractStream(std::u8string path, int offset, int length, int id);

			int ImportStream(std::u8string path, int id);

			int ImportStreamFromResource(std::u8string dst, std::u8string in);

			int PatchStream(std::u8string path, int offset, int length, int id);

			int StreamOverStream(int streamId, std::u8string applier);

			std::string ReadStream(int streamId, int size);

			int WriteStream(int streamId, std::string data);

			int WriteStreamByte(int streamId, int byteValue);

			int ReadStreamByte(int streamId);

			int TellStream(int streamId);

			int SeekStream(int streamId, int offset, int seekType);

			void RegisterStreamOperationFunctions();
		}
	}
}
