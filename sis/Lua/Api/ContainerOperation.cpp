#include "ContainerOperation.h"

#include <Mule.h>
#include <VirtualFileSystem.h>
#include <xystring.h>
#include "../LuaEnvironment.h"

int mule::Lua::Api::MountStream(int streamId, std::u8string type, std::u8string root)
{
	xybase::Stream *infra = LuaEnvironment::GetInstance().GetStream(streamId);
	if (infra == nullptr) return -10;

	std::u16string utype = xybase::string::to_utf16(type), uroot = xybase::string::to_utf16(root);
	VirtualFileSystem::GetInstance().Mount(uroot.c_str(), utype.c_str(), infra);
	return 0;
}

int mule::Lua::Api::Unmount(std::u8string root)
{
	VirtualFileSystem::GetInstance().Unmount(xybase::string::to_utf16(root).c_str());
	return 0;
}

mule::Data::Basic::MultiValue mule::Lua::Api::List(std::u8string root)
{
	mule::Data::Basic::MultiValue mv{ Data::Basic::MultiValue::MVT_MAP };
	int idx = 1;
	for (auto &&item : VirtualFileSystem::GetInstance().List(xybase::string::to_utf16(root).c_str()))
	{
		(*mv.value.mapValue)[Data::Basic::MultiValue{ idx }] = Data::Basic::MultiValue{ item };
	}

	return mv;
}

mule::Data::Basic::MultiValue mule::Lua::Api::ListRoots()
{
	mule::Data::Basic::MultiValue mv{ Data::Basic::MultiValue::MVT_MAP };
	int idx = 1;
	for (auto &&root : VirtualFileSystem::GetInstance().ListRoots())
	{
		(*mv.value.mapValue)[Data::Basic::MultiValue{ idx }] = Data::Basic::MultiValue{ root };
	}
	return mv;
}

int mule::Lua::Api::OpenAndMount(std::u8string root, std::u8string param)
{
	Mule::GetInstance().OpenAndMount(xybase::string::to_utf16(root).c_str(), xybase::string::to_utf16(param).c_str());
	return 0;
}

void mule::Lua::Api::RegisterContainerOperationFunctions()
{
	LuaHost::GetInstance().RegisterFunction("mount", MountStream);
	LuaHost::GetInstance().RegisterFunction("unmount", Unmount);
	LuaHost::GetInstance().RegisterFunction("list", List);
	LuaHost::GetInstance().RegisterFunction("openpack", OpenAndMount);
	LuaHost::GetInstance().RegisterFunction("listroots", ListRoots);
}
