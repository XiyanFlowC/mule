#include "VirtualFileSystem.h"
#include <Exception/InvalidOperationException.h>
#include <xystring.h>
#include <stack>
#include "TranscripterManager.h"

using namespace mule;

VirtualFileSystem &mule::VirtualFileSystem::GetInstance()
{
	static VirtualFileSystem _inst;
	return _inst;
}

const std::list<std::u16string> mule::VirtualFileSystem::List(const char16_t *container)
{
	auto &&it = containers.find(container);
	if (it == containers.end()) return std::list<std::u16string>{};
	return it->second->List();
}

xybase::Stream *mule::VirtualFileSystem::Open(const char16_t *p_path, xybase::FileOpenMode openMode)
{
	std::u16string fullpath = p_path;
	std::u16string root = fullpath.substr(0, fullpath.find_first_of(':')), path = fullpath.substr(fullpath.find_first_of(':') + 1);

	auto &&it = containers.find(root);
	if (it == containers.end()) return nullptr;

	logger.Info(L"Open [{}]: Root [{}] found.", xybase::string::to_wstring(path), xybase::string::to_wstring(root));
	return it->second->Open(path, openMode);
}

void mule::VirtualFileSystem::Remove(const char16_t *p_path, bool recursive)
{
	std::u16string fullpath = p_path;
	std::u16string root = fullpath.substr(0, fullpath.find_first_of(':')), path = fullpath.substr(fullpath.find_first_of(':') + 1);

	auto &&it = containers.find(root);
	if (it == containers.end()) return;

	it->second->Remove(path, recursive);
	logger.Info(L"File [{}] in root [{}] removed.", xybase::string::to_wstring(path), xybase::string::to_wstring(root));
}

void mule::VirtualFileSystem::CascadeProcess(const char16_t *targetFile, std::function<void(xybase::Stream *target)> lambda, xybase::FileOpenMode openMode)
{
	// 全限定名具有如下形式：device:dir/dir/file.bin|transformer|transformer
	// device:dir/dir/file.bin 为 VirtualFileSystem可处理之部分
	// 剩余部分须逐个适用对应Stream转义器
	std::u16string file{ targetFile };
	size_t pathEnd = file.find_first_of('|');

	// 打开基本流
	std::u16string path = file.substr(0, pathEnd);
	xybase::Stream *baseStream = VirtualFileSystem::GetInstance().Open(path.c_str(), openMode);
	std::stack<xybase::Stream *> streamStack;

	if (baseStream == nullptr) throw xybase::InvalidOperationException(L"Faile to open " + xybase::string::to_wstring(path), 7885);

	streamStack.push(baseStream);
	if (pathEnd != std::u16string::npos)
	{
		size_t transStart = pathEnd + 1, transEnd;
		// 逐级查找、打开转译器
		do
		{
			transEnd = file.find_first_of('|', transStart);
			std::u16string trans = file.substr(transStart, transEnd - transStart);
			auto tmp = TranscripterManager::GetInstance().Transcript(trans.c_str(), streamStack.top());

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

	lambda(streamStack.top());

	// 保存结束，清理
	while (!streamStack.empty())
	{
		auto stream = streamStack.top();
		delete stream;
		streamStack.pop();
	}
}

void mule::VirtualFileSystem::Mount(const char16_t *rootName, xybase::FileContainer *container)
{
	containers[rootName] = container;

	logger.Info(L"[{}] mounted to root [{}].", xybase::string::to_wstring(container->GetName()), xybase::string::to_wstring(rootName));
}

void mule::VirtualFileSystem::Mount(const char16_t *rootName, const char16_t *containerType, xybase::Stream *infraStream)
{
	for (auto &&applyer : containerApplyers)
	{
		auto ret = applyer(containerType, infraStream);
		if (ret == nullptr) continue;

		Mount(rootName, ret);
	}
}

void mule::VirtualFileSystem::Unmount(const char16_t *name)
{
	auto itr = containers.find(name);
	if (itr == containers.end()) throw xybase::InvalidParameterException(L"name", L"Specified root not fount.", 10297);
	delete itr->second;
	containers.erase(itr);
}

const std::list<std::u16string> mule::VirtualFileSystem::ListRoots() const
{
	std::list<std::u16string> ret;

	for (auto &&pair : containers)
	{
		ret.push_back(pair.first);
	}

	return ret;
}

void mule::VirtualFileSystem::RegisterContainerCreator(xybase::FileContainer *(*creator)(const char16_t *, xybase::Stream *))
{
	if (creator != nullptr)
	{
		containerApplyers.push_back(creator);
	}
}
