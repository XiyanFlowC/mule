#include "VirtualFileSystem.h"
#include <Exception/InvalidParameterException.h>
#include <xystring.h>

using namespace mule;

VirtualFileSystem &mule::VirtualFileSystem::GetInstance()
{
	static VirtualFileSystem _inst;
	return _inst;
}

const std::list<std::u16string> &mule::VirtualFileSystem::List(const char16_t *container)
{
	auto &&it = containers.find(container);
	if (it == containers.end()) return {};
	return it->second->List();
}

xybase::Stream *mule::VirtualFileSystem::Open(const char16_t *p_path, xybase::FileOpenMode openMode)
{
	std::u16string fullpath = p_path;
	std::u16string root = fullpath.substr(0, fullpath.find_first_of(':')), path = fullpath.substr(fullpath.find_first_of(':') + 1);

	logger.Info(L"Opening {} of root {}...", xybase::string::to_wstring(path), xybase::string::to_wstring(root));

	auto &&it = containers.find(root);
	if (it == containers.end()) return nullptr;

	logger.Info(L"Root {} found.", xybase::string::to_wstring(root));
	return it->second->Open(path, openMode);
}

void mule::VirtualFileSystem::Mount(const char16_t *rootName, xybase::FileContainer *container)
{
	containers[rootName] = container;

	logger.Info(L"{} mounted to root {}.", xybase::string::to_wstring(container->GetName()), xybase::string::to_wstring(rootName));
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
