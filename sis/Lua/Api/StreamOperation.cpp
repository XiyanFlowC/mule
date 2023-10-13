#include "StreamOperation.h"

#include "../LuaEnvironment.h"
#include <VirtualFileSystem.h>
#include <Mule.h>
#include "xystring.h"
#include <Data/Table.h>
#include <Data/TypeManager.h>

int mule::Lua::Api::OpenStream(std::string path, std::string openMode)
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

int mule::Lua::Api::ExportStream(std::string path, int id)
{
	Mule::GetInstance().Export(xybase::string::to_utf16(path).c_str(), id);
	return 0;
}

int mule::Lua::Api::ImportStream(std::string path, int id)
{
	Mule::GetInstance().Import(xybase::string::to_utf16(path).c_str(), id);
	return 0;
}

int mule::Lua::Api::ExportSheet(int streamId, std::string handler, std::string type, std::string tableName, size_t offset, int length)
{
	std::u16string utype = xybase::string::to_utf16(type), utbl = xybase::string::to_utf16(tableName);
	
	xybase::Stream *stream = LuaEnvironment::GetInstance().GetStream(streamId);
	if (stream == nullptr) return -10;

	mule::Data::Basic::Type::DataHandler *hnd = Mule::GetInstance().GetDataHandler(xybase::string::to_utf16(handler).c_str());
	if (hnd == nullptr) return -11;

	Data::Table *target = new Data::Table(Data::TypeManager::GetInstance().GetType(utype), utbl, length, offset);
	if (target == nullptr) return -12;

	target->Read(stream, hnd);
	delete target;
}

int mule::Lua::Api::ImportSheet(int streamId, std::string handler, std::string type, std::string tableName, size_t offset, int length)
{
	std::u16string utype = xybase::string::to_utf16(type), utbl = xybase::string::to_utf16(tableName);

	xybase::Stream *stream = LuaEnvironment::GetInstance().GetStream(streamId);
	if (stream == nullptr) return -10;

	mule::Data::Basic::Type::FileHandler *hnd = Mule::GetInstance().GetFileHandler(xybase::string::to_utf16(handler).c_str());
	if (hnd == nullptr) return -11;

	Data::Table *target = new Data::Table(Data::TypeManager::GetInstance().GetType(utype), utbl, length, offset);
	if (target == nullptr) return -12;

	target->Write(stream, hnd);
	delete target;
}

void mule::Lua::Api::RegisterStreamOperationFunctions()
{
	LuaHost::GetInstance().RegisterFunction("open", OpenStream);
	LuaHost::GetInstance().RegisterFunction("close", CloseStream);
	LuaHost::GetInstance().RegisterFunction("export", ExportStream);
	LuaHost::GetInstance().RegisterFunction("import", ImportStream);
	LuaHost::GetInstance().RegisterFunction("exportsht", ExportSheet);
	LuaHost::GetInstance().RegisterFunction("importsht", ImportSheet);
}
