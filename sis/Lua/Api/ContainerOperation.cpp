#include "ContainerOperation.h"

#include <VirtualFileSystem.h>
#include <xystring.h>
#include "../LuaEnvironment.h"

int mule::Lua::Api::MountStream(int streamId, std::string type, std::string root)
{
	xybase::Stream *infra = LuaEnvironment::GetInstance().GetStream(streamId);
	if (infra == nullptr) return -10;

	std::u16string utype = xybase::string::to_utf16(type), uroot = xybase::string::to_utf16(root);
	VirtualFileSystem::GetInstance().Mount(uroot.c_str(), utype.c_str(), infra);
}

int mule::Lua::Api::Unmount(std::string root)
{
	VirtualFileSystem::GetInstance().Unmount(xybase::string::to_utf16(root).c_str());
	return 0;
}

int mule::Lua::Api::List(std::string root)
{
	wprintf(L"根 %s 的文件列表：\n", xybase::string::to_wstring(root).c_str());
	for (auto &&item : VirtualFileSystem::GetInstance().List(xybase::string::to_utf16(root).c_str()))
	{
		wprintf(L"%s\n", xybase::string::to_wstring(item).c_str());
	}

	return 0;
}

int mule::Lua::Api::ListRoots()
{
	fputws(L"已注册的根：\n", stdout);
	for (auto &&root : VirtualFileSystem::GetInstance().ListRoots())
	{
		wprintf(L"%s\n", xybase::string::to_wstring(root).c_str());
	}
	return 0;
}

void mule::Lua::Api::RegisterContainerOperationFunctions()
{
	LuaHost::GetInstance().RegisterFunction("mount", MountStream);
	LuaHost::GetInstance().RegisterFunction("unmount", Unmount);
	LuaHost::GetInstance().RegisterFunction("list", List);
	LuaHost::GetInstance().RegisterFunction("listroots", ListRoots);
}
