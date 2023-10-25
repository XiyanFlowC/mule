#include "Mule.h"

#include "VirtualFileSystem.h"
#include "TranscripterManager.h"
#include "Storage/DataManager.h"
#include <string>
#include <stack>
#include <Exception/InvalidParameterException.h>
#include <xystring.h>
#include <StringBuilder.h>
#include "Data/TypeManager.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

using namespace mule;

void mule::Mule::LoadPlugin(const char16_t *plugin)
{
#ifdef _WIN32
	std::wstring pluginFile((const wchar_t *)plugin);
	pluginFile += ".dll";
	HMODULE libraryHandle = LoadLibraryW(pluginFile.c_str());

	if (libraryHandle == NULL)
	{
		logger.Error(L"Load plugin failed, GetLastError(): {}", GetLastError());
		throw xybase::RuntimeException(L"Cannot load specified plugin", GetLastError());
	}
#else
	std::string pluginFile = xybase::string::to_string(plugin);
	size_t pathEnd = pluginFile.find_last_of('/');
	pluginFile = 
		(pathEnd == std::string::npos
		? ("lib" + pluginFile)
		: (pluginFile.replace(pathEnd, 1, "/lib"))) + ".so";
	void *libraryHandle = dlopen(pluginFile.c_str(), RTLD_LAZY);

	if (libraryHandle == NULL)
	{
		throw xybase::RuntimeException(L"Cannot load specified plugin.", errno);
	}
#endif

	PluginDescription *(*getDesc)() = (PluginDescription * (*)())
#ifdef _WIN32
		GetProcAddress(libraryHandle, "GetDescription");
#else
		dlsym(libraryHandle, "GetDescription");
#endif

	if (getDesc == nullptr)
	{
		logger.Error(L"Specified plugin has no function: GetDescription()");
#ifdef _WIN32
		FreeLibrary(libraryHandle);
#else
		dlclose(libraryHandle);
#endif
		throw xybase::RuntimeException(L"Try to load an invalid plugin.", 2331);
	}

	pluginHandlers.push_back(libraryHandle);

	auto desc = getDesc();
	LoadDescription(desc);
}

void mule::Mule::LoadDescription(const PluginDescription *description)
{
	descriptions.push_back(description);

	// 注册类型创建器
	if (description->GetCreators != nullptr)
		Data::TypeManager::GetInstance().RegisterObjectCreator(description->GetCreators());
	// 注册流转译器创建器
	if (description->ApplyStream != nullptr)
		TranscripterManager::GetInstance().RegisterCreator(description->ApplyStream);
	// 注册流转译器创建器（参数式）
	if (description->OpenStream != nullptr)
		TranscripterManager::GetInstance().RegisterCreator(description->OpenStream);
	// 注册容器转译器
	if (description->ApplyContainer != nullptr)
		VirtualFileSystem::GetInstance().RegisterContainerCreator(description->ApplyContainer);
	/*if (description->OpenContainer != nullptr)
		;*/
}

std::wstring mule::Mule::ShowPlugins()
{
	xybase::StringBuilder<wchar_t> sb;
	for (const PluginDescription *desc : descriptions)
	{
		sb.Append(L"Plugin ");
		sb.Append(desc->name);
		sb.Append(L"\tVer.");
		sb.Append(std::to_wstring(desc->majorVer).c_str());
		sb.Append('.');
		sb.Append(std::to_wstring(desc->minorVer).c_str());
		sb.Append('\t');
		sb.Append(desc->licence);
		sb.Append(L"\tby ");
		sb.Append(desc->author);
		sb.Append(L"\n\t");
		sb.Append(desc->description);
		sb.Append('\n');
	}
	return sb.ToString();
}

void mule::Mule::PrintPlugins()
{
	logger.Info(L"Loaded Plugin\n============\n{}", ShowPlugins());
}

xybase::Stream *mule::Mule::ApplyStream(const char16_t *typeName, xybase::Stream *infraStream)
{
	return TranscripterManager::GetInstance().Transcript(typeName, infraStream);
}

void mule::Mule::MountStream(const char16_t *mountName, const char16_t *typeName, xybase::Stream *infraStream)
{
	VirtualFileSystem::GetInstance().Mount(mountName, typeName, infraStream);
}

mule::Data::Basic::Type::DataHandler *mule::Mule::GetDataHandler(const char16_t *name)
{
	for (auto &&desc : descriptions)
	{
		if (desc->GetDataHandler != nullptr)
		{
			auto ret = desc->GetDataHandler(name);
			if (ret != nullptr) return ret;
		}
	}
	logger.Warn(L"Unable to create specified data handler: {}.", xybase::string::to_wstring(name));
	return nullptr;
}

mule::Data::Basic::Type::FileHandler *mule::Mule::GetFileHandler(const char16_t *name)
{
	for (auto &&desc : descriptions)
	{
		if (desc->GetFileHandler != nullptr)
		{
			auto ret = desc->GetFileHandler(name);
			if (ret != nullptr) return ret;
		}
	}
	logger.Warn(L"Unable to create specified file handler: {}.", xybase::string::to_wstring(name));
	return nullptr;
}

mule::Mule::Mule()
{
}

mule::Mule::~Mule()
{
	for (auto &&handler : pluginHandlers)
	{
#ifdef _WIN32
		FreeLibrary((HMODULE)handler);
#else
		dlclose(handler);
#endif
	}
}

Mule &mule::Mule::GetInstance()
{
	static Mule _inst;
	return _inst;
}

void mule::Mule::Export(const char16_t *targetFile, uint32_t id)
{
	logger.Debug(L"抽取文件{}到{}。", xybase::string::to_wstring(targetFile), id);

	VirtualFileSystem::GetInstance().CascadeProcess(targetFile, [=](xybase::Stream *target) -> void {
		target->Seek(0, xybase::Stream::SM_END);
		size_t size = target->Tell();
		target->Seek(0, xybase::Stream::SM_BEGIN);
		char *buffer = new char[size];
		target->ReadBytes(buffer, size);

		Storage::BinaryData bd(buffer, size, false);
		Storage::DataManager::GetInstance().SaveData(bd, id);
	}, xybase::FOM_READ);

	
	logger.Debug(L"抽取完毕。");
}

void mule::Mule::Extract(const char16_t *targetFile, size_t offset, size_t length, uint32_t id)
{
	logger.Debug(L"抽取数据{}({}, size={})到{}。", xybase::string::to_wstring(targetFile), offset, length, id);

	VirtualFileSystem::GetInstance().CascadeProcess(targetFile, [=](xybase::Stream *target) -> void {
		target->Seek(offset, xybase::Stream::SM_BEGIN);
		char *buffer = new char[length];
		target->ReadBytes(buffer, length);

		Storage::BinaryData bd(buffer, length, false);
		Storage::DataManager::GetInstance().SaveData(bd, id);
	}, xybase::FOM_READ);


	logger.Debug(L"抽取完毕。");
}

void mule::Mule::Import(const char16_t *targetFile, uint32_t id)
{
	logger.Debug(L"插入文件{}从{}。", xybase::string::to_wstring(targetFile), id);

	VirtualFileSystem::GetInstance().CascadeProcess(targetFile, [=](xybase::Stream *target) -> void {
		Storage::BinaryData bd = Storage::DataManager::GetInstance().LoadData(id);

		target->Write(bd.GetData(), bd.GetLength());
	}, xybase::FOM_WRITE);

	logger.Debug(L"插入完毕。");
}

void mule::Mule::Patch(const char16_t *targetFile, size_t offset, size_t length, uint32_t id)
{
	logger.Debug(L"修补数据{}({}, size={})从{}。", xybase::string::to_wstring(targetFile), offset, length, id);

	VirtualFileSystem::GetInstance().CascadeProcess(targetFile, [=](xybase::Stream *target) -> void {
		Storage::BinaryData bd = Storage::DataManager::GetInstance().LoadData(id);

		target->Seek(0, xybase::Stream::SM_BEGIN);
		target->Write(bd.GetData(), bd.GetLength());
	}, xybase::FOM_WRITE);

	logger.Debug(L"修补完毕。");
}
