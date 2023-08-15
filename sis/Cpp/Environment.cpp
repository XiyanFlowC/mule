#include "Environment.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

using namespace mule::Cpp;
using namespace mule::Data;

mule::Cpp::Environment::Environment()
{
}

mule::Cpp::Environment::~Environment()
{
	for (auto &&handler : handlers)
	{
#ifdef _WIN32
		FreeLibrary((HMODULE)handler.second);
#else
		dlclose(handler.second);
#endif
	}
}

Environment &Environment::GetInstance()
{
	static Environment _inst {};
	return _inst;
}

void mule::Cpp::Environment::OpenPlugin(std::string moduleName, std::string path)
{
	auto &&itr = handlers.find(moduleName);
	if (itr != handlers.end())
	{
		ClosePlugin(moduleName);
	}

#ifdef _WIN32
	void *libraryHandle = LoadLibraryA(path.c_str());
#else
	void *libraryHandle = dlopen(path.c_str(), RTLD_LAZY);
#endif

	if (libraryHandle == NULL)
	{
		throw xybase::InvalidParameterException(u"path", u"Cannot load specified plugin.", __LINE__);
	}

	handlers[moduleName] = libraryHandle;

	PluginDescription *(*getDesc)() = (PluginDescription *(*)())GetFunction(moduleName, "GetDescription");
	if (getDesc == nullptr)
		throw xybase::RuntimeException(u"Try to load an invalid plugin.", __LINE__);
	
	auto desc = getDesc();
	descriptions.push_back(desc);
	if (desc->GetCreators != nullptr)
	{
		TypeManager::GetInstance().RegisterObjectCreator(desc->GetCreators());
	}
}

void *mule::Cpp::Environment::GetFunction(std::string moduleName, std::string functionName)
{
	auto &&itr = handlers.find(moduleName);
	if (itr == handlers.end())
	{
		throw xybase::InvalidParameterException(u"moduleName", u"Unregistered module.", __LINE__);
	}
	
	return
#ifdef _WIN32
		GetProcAddress((HMODULE)itr->second, functionName.c_str());
#else
		dlsym(itr->second, functionName.c_str());
#endif
}

void mule::Cpp::Environment::ClosePlugin(std::string moduleName)
{
	auto &&itr = handlers.find(moduleName);
	if (itr == handlers.end())
	{
		throw xybase::InvalidParameterException(u"moduleName", u"Unregistered module.", __LINE__);
	}

#ifdef _WIN32
	FreeLibrary((HMODULE)itr->second);
#else
	dlclose(itr->second);
#endif
	handlers.erase(itr);
}

xybase::Stream *mule::Cpp::Environment::GetStream(std::string name)
{
	for (auto &&itr : descriptions)
	{
		if (itr->GetStream != nullptr)
		{
			auto stream = itr->GetStream(name.c_str());
			if (stream != nullptr)
			{
				return stream;
			}
		}
	}
	return nullptr;
}

xybase::Stream *mule::Cpp::Environment::ApplyStream(std::string name, xybase::Stream *infraStream)
{
	for (auto &&itr : descriptions)
	{
		if (itr->ApplyStream != nullptr)
		{
			auto stream = itr->ApplyStream(name.c_str(), infraStream);
			if (stream != nullptr)
			{
				return stream;
			}
		}
	}
	return nullptr;
}

xybase::FileContainer *mule::Cpp::Environment::GetFileContainer(std::string name, xybase::Stream *infraStream)
{
	for (auto &&itr : descriptions)
	{
		if (itr->GetFileContainer != nullptr)
		{
			auto stream = itr->GetFileContainer(name.c_str(), infraStream);
			if (stream != nullptr)
			{
				return stream;
			}
		}
	}
	return nullptr;
}
