#include "StreamOperation.h"

#include "../LuaEnvironment.h"
#include <VirtualFileSystem.h>
#include <Mule.h>
#include "xystring.h"
#include <Data/TypeManager.h>
#include <Storage/ResourceManager.h>

int mule::Lua::Api::OpenStream(std::u8string path, std::u8string openMode)
{
	xybase::FileOpenMode mode{};
	for (char ch : openMode)
	{
		if (ch == 'r') mode |= xybase::FOM_READ;
		if (ch == 'w') mode |= xybase::FOM_WRITE;
		if (ch == 't') mode |= xybase::FOM_TRUNCATE;
		if (ch == 'a') mode |= xybase::FOM_APPEND;
		if (ch == 'x') mode |= xybase::FOM_EXCLUSIVE;
		if (ch == 'b') mode |= xybase::FOM_BIG_ENDIAN;
	}
	xybase::Stream *stream = VirtualFileSystem::GetInstance().Open(xybase::string::to_utf16(path).c_str(), mode);
	if (stream == nullptr) return -10;
	return LuaEnvironment::GetInstance().SetStream(stream);
}

int mule::Lua::Api::CloseStream(int id)
{
	try
	{
		LuaEnvironment::GetInstance().CloseStream(id);
	}
	catch (xybase::Exception &ex)
	{
		return ex.GetErrorCode();
	}
	return 0;
}

int mule::Lua::Api::ExportStream(std::u8string path, int id)
{
	Mule::GetInstance().Export(xybase::string::to_utf16(path).c_str(), id);
	return 0;
}

int mule::Lua::Api::ExportStreamToResource(std::u8string src, std::u8string out)
{
	try
	{
		VirtualFileSystem::GetInstance().CascadeProcess(xybase::string::to_utf16(src).c_str(), [&](xybase::Stream *target) -> void {
			target->Seek(0, xybase::Stream::SM_END);
			size_t size = target->Tell();
			target->Seek(0, xybase::Stream::SM_BEGIN);
			char *buffer = new char[size];
			target->ReadBytes(buffer, size);

			Storage::BinaryData bd(buffer, size, false);
			Storage::ResourceManager::GetInstance().SaveResource(xybase::string::to_utf16(out), bd);
			}, xybase::FOM_READ);
		return 0;
	}
	catch (xybase::Exception &ex)
	{
		return ex.GetErrorCode();
	}
}

int mule::Lua::Api::ExtractStream(std::u8string path, int offset, int length, int id)
{
	Mule::GetInstance().Extract(xybase::string::to_utf16(path).c_str(), offset, length, id);
	return 0;
}

int mule::Lua::Api::ImportStream(std::u8string path, int id)
{
	Mule::GetInstance().TryImport(xybase::string::to_utf16(path).c_str(), id);
	return 0;
}

int mule::Lua::Api::ImportStreamFromResource(std::u8string dst, std::u8string in)
{
	try
	{
		VirtualFileSystem::GetInstance().CascadeProcess(xybase::string::to_utf16(dst).c_str(), [&](xybase::Stream *target) -> void {
			Storage::BinaryData bd = Storage::ResourceManager::GetInstance().LoadResource(xybase::string::to_string(in).c_str());

			target->Write(bd.GetData(), bd.GetLength());
			}, xybase::FOM_WRITE);
		return 0;
	}
	catch (xybase::Exception &ex)
	{
		return ex.GetErrorCode();
	}
}

int mule::Lua::Api::PatchStream(std::u8string path, int offset, int length, int id)
{
	Mule::GetInstance().Patch(xybase::string::to_utf16(path).c_str(), offset, length, id);
	return 0;
}

int mule::Lua::Api::StreamOverStream(int streamId, std::u8string applier)
{
	auto infraStream = LuaEnvironment::GetInstance().GetStream(streamId);
	if (infraStream == nullptr) return -1;
	auto stream = mule::Mule::GetInstance().ApplyStream(xybase::string::to_utf16(applier).c_str(), infraStream);
	return LuaEnvironment::GetInstance().SetStream(stream);
}

std::string mule::Lua::Api::ReadStream(int streamId, int size)
{
	std::unique_ptr<char[]> buffer(new char[size]);
	LuaEnvironment::GetInstance().GetStream(streamId)->ReadBytes(buffer.get(), size);
	return buffer.get();
}

int mule::Lua::Api::WriteStream(int streamId, std::string data)
{
	LuaEnvironment::GetInstance().GetStream(streamId)->Write(data.c_str(), data.size() - 1);
	return 0;
}

int mule::Lua::Api::TellStream(int streamId)
{
	return (int)LuaEnvironment::GetInstance().GetStream(streamId)->Tell();
}

int mule::Lua::Api::SeekStream(int streamId, int offset, int seekType)
{
	LuaEnvironment::GetInstance().GetStream(streamId)->Seek(offset, (xybase::Stream::SeekMode)seekType);
	return (int)LuaEnvironment::GetInstance().GetStream(streamId)->Tell();
}

void mule::Lua::Api::RegisterStreamOperationFunctions()
{
	auto &host = LuaHost::GetInstance();
	host.RegisterFunction("open", OpenStream);
	host.RegisterFunction("close", CloseStream);
	host.RegisterFunction("read", ReadStream);
	host.RegisterFunction("write", WriteStream);
	host.RegisterFunction("tell", TellStream);
	host.RegisterFunction("seek", SeekStream);
	host.RegisterFunction("export", ExportStream);
	host.RegisterFunction("resexport", ExportStreamToResource);
	host.RegisterFunction("extract", ExtractStream);
	host.RegisterFunction("import", ImportStream);
	host.RegisterFunction("resimport", ImportStreamFromResource);
	host.RegisterFunction("patch", PatchStream);
	host.RegisterFunction("sos", StreamOverStream);
	host.RegisterFunction("applyfile", StreamOverStream);
}
