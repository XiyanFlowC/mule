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

mule::Data::Basic::MultiValue mule::Lua::Api::ReadStream(int streamId, int size)
{
	std::unique_ptr<char[]> buffer(new char[size]);
	LuaEnvironment::GetInstance().GetStream(streamId)->ReadBytes(buffer.get(), size);
	mule::Data::Basic::MultiValue ret{ mule::Data::Basic::MultiValue::MVT_ARRAY, size };
	for (int i = 0; i < size; ++i)
	{
		ret.value.arrayValue[i] = mule::Data::Basic::MultiValue((int64_t)(buffer[i] & 0xFF));
	}
	return ret;
}

int mule::Lua::Api::WriteStream(int streamId, mule::Data::Basic::MultiValue data)
{
	xybase::StringBuilder<char> sb;
	for (size_t i = 1; data.IsType(mule::Data::Basic::MultiValue::MVT_MAP) && data.value.mapValue->find(mule::Data::Basic::MultiValue((uint64_t)i)) != data.value.mapValue->end(); ++i)
	{
		auto &byteVal = data.value.mapValue->at(mule::Data::Basic::MultiValue((uint64_t)i));
		if (!byteVal.IsType(mule::Data::Basic::MultiValue::MVT_INT) && !byteVal.IsType(mule::Data::Basic::MultiValue::MVT_UINT))
			continue;
		sb.Append(static_cast<char>(byteVal.IsType(mule::Data::Basic::MultiValue::MVT_INT) ? (byteVal.value.signedValue & 0xFF) : (byteVal.value.unsignedValue & 0xFF)));
	}
	LuaEnvironment::GetInstance().GetStream(streamId)->Write(sb.ToString(), sb.Length());
	return 0;
}



int mule::Lua::Api::WriteStreamByte(int streamId, int byteValue)
{
	LuaEnvironment::GetInstance().GetStream(streamId)->Write(static_cast<uint8_t>(byteValue & 0xFF));
	return 0;
}

int mule::Lua::Api::ReadStreamByte(int streamId)
{
	char ret;
	LuaEnvironment::GetInstance().GetStream(streamId)->ReadBytes(&ret, 1);
	return (int)(ret & 0xFF);
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
	host.RegisterFunction("writebyte", WriteStreamByte);
	host.RegisterFunction("readbyte", ReadStreamByte);
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
