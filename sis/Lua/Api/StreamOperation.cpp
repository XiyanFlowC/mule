#include "StreamOperation.h"

#include "../LuaEnvironment.h"
#include <VirtualFileSystem.h>
#include <Mule.h>
#include "xystring.h"
#include <Data/Table.h>
#include <Data/TypeManager.h>

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

int mule::Lua::Api::ExtractStream(std::u8string path, int offset, int length, int id)
{
	Mule::GetInstance().Extract(xybase::string::to_utf16(path).c_str(), offset, length, id);
	return 0;
}

int mule::Lua::Api::ImportStream(std::u8string path, int id)
{
	Mule::GetInstance().Import(xybase::string::to_utf16(path).c_str(), id);
	return 0;
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

void mule::Lua::Api::RegisterStreamOperationFunctions()
{
	LuaHost::GetInstance().RegisterFunction("open", OpenStream);
	LuaHost::GetInstance().RegisterFunction("close", CloseStream);
	LuaHost::GetInstance().RegisterFunction("export", ExportStream);
	LuaHost::GetInstance().RegisterFunction("extract", ExtractStream);
	LuaHost::GetInstance().RegisterFunction("import", ImportStream);
	LuaHost::GetInstance().RegisterFunction("patch", PatchStream);
	LuaHost::GetInstance().RegisterFunction("sos", StreamOverStream);
	LuaHost::GetInstance().RegisterFunction("applyfile", StreamOverStream);
}
