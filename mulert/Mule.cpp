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
	HMODULE libraryHandle = LoadLibraryW((const wchar_t *)plugin);
#else
	void *libraryHandle = dlopen(xybase::string::to_wstring(plugin).c_str(), RTLD_LAZY);
#endif

	if (libraryHandle == NULL)
	{
		throw xybase::InvalidParameterException(L"path", L"Cannot load specified plugin.", __LINE__);
	}

	PluginDescription *(*getDesc)() = (PluginDescription * (*)())
#ifdef _WIN32
		GetProcAddress(libraryHandle, "GetDescription");
#else
		dlsym(libraryHandle, "GetDescription");
#endif

	if (getDesc == nullptr)
	{
#ifdef _WIN32
		FreeLibrary(libraryHandle);
#else
		dlclose(libraryHandle);
#endif
		throw xybase::RuntimeException(L"Try to load an invalid plugin.", __LINE__);
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
	for (const PluginDescription *desc : descriptions)
	{
		auto func = desc->ApplyStream;
		if (func == nullptr) continue;

		xybase::Stream *stream = func(typeName, infraStream);
		if (stream != nullptr) return stream;
	}
}

void mule::Mule::MountStream(const char16_t *mountName, const char16_t *typeName, xybase::Stream *infraStream)
{
	for (const PluginDescription *desc : descriptions)
	{
		auto func = desc->ApplyContainer;
		if (func == nullptr) continue;

		xybase::FileContainer *container = func(typeName, infraStream);
		if (container == nullptr) continue;

		VirtualFileSystem::GetInstance().Mount(mountName, container);
	}
}

mule::Data::Basic::Type::DataHandler *mule::Mule::GetDataHandler(const char16_t *name)
{
	logger.Info(L"Try to create data handler for {}...", xybase::string::to_wstring(name));
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
	logger.Info(L"Try to create file handler for {}...", xybase::string::to_wstring(name));
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

	// 全限定名具有如下形式：device:dir/dir/file.bin|transformer|transformer
	// device:dir/dir/file.bin 为 VirtualFileSystem可处理之部分
	// 剩余部分须逐个适用对应Stream转义器
	std::u16string file{targetFile};
	size_t pathEnd = file.find_first_of('|');

	// 打开基本流
	std::u16string path = file.substr(pathEnd);
	xybase::Stream *baseStream = VirtualFileSystem::GetInstance().Open(path.c_str(), xybase::FOM_READ);
	std::stack<xybase::Stream *> streamStack;

	if (baseStream == nullptr) throw xybase::InvalidParameterException(L"targetFile", L"File does not exist or cannot be read.", 7885);

	streamStack.push(baseStream);
	if (pathEnd != std::u16string::npos)
	{
		size_t transStart = pathEnd + 1, transEnd;
		// 逐级查找、打开转译器
		do
		{
			transEnd = file.find_first_of('|', transStart);
			std::u16string trans = file.substr(transStart, transEnd - transStart);
			auto tmp = TranscripterManager::GetInstance().GetTranscripter(trans.c_str(), streamStack.top());

			// 打开失败，清理
			if (tmp == nullptr)
			{
				while (!streamStack.empty())
				{
					auto stream = streamStack.top();
					delete stream;
					streamStack.pop();
				}
				logger.Error(L"Specified transcripter: {} is not available.", xybase::string::to_wstring(trans));
				throw xybase::InvalidParameterException(L"targetFile", L"Specified transcripter not found.", 7886);
			}
		} while (transStart != std::u16string::npos);
	}

	auto target = streamStack.top();
	target->Seek(0, xybase::Stream::SM_END);
	size_t size = target->Tell();
	target->Seek(0, xybase::Stream::SM_BEGIN);
	char *buffer = new char[size];
	target->ReadBytes(buffer, size);

	Storage::BinaryData bd(buffer, size, false);
	Storage::DataManager::GetInstance().SaveData(bd, id);

	logger.Info(L"File {}(size={}) was saved to {}.", xybase::string::to_wstring(path), size, id);

	// 保存结束，清理
	while (!streamStack.emplace())
	{
		auto stream = streamStack.top();
		delete stream;
		streamStack.pop();
	}
	logger.Debug(L"抽取完毕。");
}

void mule::Mule::Import(const char16_t *targetFile, uint32_t id)
{
	// 和 Export 重复，需要重构
	logger.Debug(L"插入文件{}从{}。", xybase::string::to_wstring(targetFile), id);

	// 全限定名具有如下形式：device:dir/dir/file.bin|transformer|transformer
	// device:dir/dir/file.bin 为 VirtualFileSystem可处理之部分
	// 剩余部分须逐个适用对应Stream转义器
	std::u16string file{targetFile};
	size_t pathEnd = file.find_first_of('|');

	// 打开基本流
	std::u16string path = file.substr();
	xybase::Stream *baseStream = VirtualFileSystem::GetInstance().Open(targetFile, xybase::FOM_WRITE);
	std::stack<xybase::Stream *> streamStack;

	if (baseStream == nullptr) throw xybase::InvalidParameterException(L"targetFile", L"File does not exist or cannot be read.", 7885);

	streamStack.push(baseStream);
	if (pathEnd != std::u16string::npos)
	{
		size_t transStart = pathEnd + 1, transEnd;
		// 逐级查找、打开转译器
		do
		{
			transEnd = file.find_first_of('|', transStart);
			std::u16string trans = file.substr(transStart, transEnd - transStart);
			auto tmp = TranscripterManager::GetInstance().GetTranscripter(trans.c_str(), streamStack.top());

			// 打开失败，清理
			if (tmp == nullptr)
			{
				while (!streamStack.empty())
				{
					auto stream = streamStack.top();
					delete stream;
					streamStack.pop();
				}
				throw xybase::InvalidParameterException(L"targetFile", L"Specified transcripter not found.", 7886);
			}
		} while (transStart != std::u16string::npos);
	}

	Storage::BinaryData bd = Storage::DataManager::GetInstance().LoadData(id);

	auto target = streamStack.top();
	target->Write(bd.GetData(), bd.GetLength());

	// 保存结束，清理
	while (!streamStack.emplace())
	{
		auto stream = streamStack.top();
		delete stream;
		streamStack.pop();
	}
	logger.Debug(L"插入完毕。");
}
