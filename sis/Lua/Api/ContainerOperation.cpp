#include "ContainerOperation.h"

#include <Mule.h>
#include <VirtualFileSystem.h>
#include <xystring.h>
#include <FileContainerBasic.h>
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
		(*mv.value.mapValue)[Data::Basic::MultiValue{ idx++ }] = Data::Basic::MultiValue{ item };
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

int mule::Lua::Api::Remove(std::u8string file)
{
	VirtualFileSystem::GetInstance().Remove(xybase::string::to_utf16(file).c_str());
	return 0;
}

mule::Data::Basic::MultiValue mule::Lua::Api::Metadata(std::u8string path, mule::Data::Basic::MultiValue data)
{
	auto root = VirtualFileSystem::GetInstance().GetRoot(xybase::string::to_utf16(path.substr(0, path.find_first_of(':'))).c_str());
	xybase::FileContainerBasic *rootB = dynamic_cast<xybase::FileContainerBasic *>(root);
	if (!rootB) return mule::Data::Basic::MultiValue::MV_NULL;

	if (data.IsType(mule::Data::Basic::MultiValue::MVT_NULL))
	{
		auto data = rootB->GetMetadata(xybase::string::to_utf16(path.substr(path.find_first_of(':') + 1)));

		mule::Data::Basic::MultiValue ret(mule::Data::Basic::MultiValue::ValueType::MVT_MAP);
		ret.value.mapValue->insert(
			{
				{ {u"path"}, {data.path} },
				{ {u"size"}, {data.size} },
				{ {u"offset"}, {data.offset} },
				{ {u"occupied"}, {data.occupied ? 1 : 0} },
			});
		return ret;
	}
	else
	{
		auto mapping = *data.value.mapValue;
		xybase::FileContainerBasic::FileEntry entry
		{
			mapping[{u"offset"}].value.unsignedValue,
			mapping[{u"size"}].value.unsignedValue,
			*mapping[{u"path"}].value.stringValue,
			mapping[{u"occupied"}].value.signedValue == 1,
		};
		rootB->SetMetadata(xybase::string::to_utf16(path.substr(path.find_first_of(':') + 1)), entry);
		return mule::Data::Basic::MultiValue::MV_NULL;
	}
}

void mule::Lua::Api::RegisterContainerOperationFunctions()
{
	LuaHost::GetInstance().RegisterFunction("mount", &MountStream);
	LuaHost::GetInstance().RegisterFunction("unmount", &Unmount);
	LuaHost::GetInstance().RegisterFunction("remove", &Remove);
	LuaHost::GetInstance().RegisterFunction("list", &List);
	LuaHost::GetInstance().RegisterFunction("openpack", &OpenAndMount);
	LuaHost::GetInstance().RegisterFunction("listroots", &ListRoots);
	LuaHost::GetInstance().RegisterFunction("mdata", &Metadata);
}
