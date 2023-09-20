#include "TypeManager.h"
#include "xystring.h"
using namespace mule::Data;

TypeManager::TypeManager()
{
	first = last = nullptr;
}

TypeManager::~TypeManager()
{
	for (auto &object : objects)
	{
		delete object.second;
	}
	objects.clear();
}

TypeManager &TypeManager::GetInstance()
{
	static TypeManager _inst;
	return _inst;
}

void TypeManager::RegisterObject(Basic::Type *object, std::u16string name)
{
	objects[name] = object;
}

void TypeManager::UnregisterObject(std::u16string name)
{
	objects.erase(name);
}

void TypeManager::ReleaseObject(std::u16string name)
{
	if (objects.contains(name)) {
		delete objects[name];
		objects.erase(name);
	}
}

void TypeManager::RegisterObjectCreator(TypeCreator *creator)
{
	if (last == nullptr)
	{
		first = last = creator;
		while (creator->nextCreator != nullptr)
		{
			creator = last = creator->nextCreator;
		}
		return;
	}

	last->nextCreator = creator;
	while (last->nextCreator != nullptr)
	{
		last = last->nextCreator;
	}
}

Basic::Type *TypeManager::GetOrCreateType(std::u16string info)
{
	// 若已经存在则直接返回
	if (objects.contains(info)) return objects[info];

	// 若没有创建器则直接返回
	if (first == nullptr) return nullptr;

	logger.Info(L"New type {} registered.", xybase::string::to_wstring(info));

	// 保存创建结果并返回
	return objects[info] = first->CreateType(info);
}

Basic::Type *mule::Data::TypeManager::GetOrCreateType(std::u16string name, const std::map<std::u16string, std::u16string> &metainfo)
{
	// 若元参数为空则直接返回
	if (metainfo.empty()) return GetOrCreateType(name);

	// 若没有创建器则直接返回
	if (first == nullptr) return nullptr;

	auto mangledName = name + u"%$%";
	for (auto &&pair : metainfo)
	{
		mangledName += pair.first + u"%:%" + pair.second + u"%;%";
	}

	if (objects.contains(mangledName)) return objects[mangledName];

	logger.Info(L"New type {}[mangled] registered.", xybase::string::to_wstring(mangledName));
	// 返回创建结果
	return objects[mangledName] = first->CreateType(name, metainfo);
}

Basic::Type *TypeManager::GetType(std::u16string name)
{
	if (objects.contains(name)) return objects[name];

	return nullptr;
}

