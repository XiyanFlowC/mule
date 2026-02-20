#pragma once

#include <string>

#include <Data/Basic/MultiValue.h>

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

			int RebuildContainerWithResource(std::u8string containerRoot, std::u8string filePath, std::u8string resourcePath);

			int ImportStreamFromResourceWithRebuild(std::u8string dst, std::u8string in);

			int PatchStream(std::u8string path, int offset, int length, int id);

			int StreamOverStream(int streamId, std::u8string applier);

			mule::Data::Basic::MultiValue ReadStream(int streamId, int size);

			int WriteStream(int streamId, mule::Data::Basic::MultiValue data);

			int WriteStreamByte(int streamId, int byteValue);

			int ReadStreamByte(int streamId);

			int WriteStreamUInt16(int streamId, unsigned int halfValue);

			unsigned int ReadStreamUInt16(int streamId);

			int WriteStreamUInt32(int streamId, unsigned int value);

			unsigned int ReadStreamUInt32(int streamId);

			int WriteStreamUInt64(int streamId, unsigned long long value);

			unsigned long long ReadStreamUInt64(int streamId);

			int WriteStreamInt16(int streamId, int halfValue);

			int ReadStreamInt16(int streamId);

			int WriteStreamInt32(int streamId, int value);

			int ReadStreamInt32(int streamId);

			int WriteStreamInt64(int streamId, long long value);

			long long ReadStreamInt64(int streamId);

			int WriteStreamFloat(int streamId, float value);

			double ReadStreamFloat(int streamId);

			int WriteStreamDouble(int streamId, double value);

			double ReadStreamDouble(int streamId);

			int TellStream(int streamId);

			int SeekStream(int streamId, int offset, int seekType);

			void RegisterStreamOperationFunctions();
		}
	}
}
