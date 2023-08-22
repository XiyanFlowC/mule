#include "TypeManager.h"

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

	// 保存创建结果并返回
	return objects[info] = first->CreateType(info);
}

Basic::Type *mule::Data::TypeManager::GetOrCreateType(std::u16string name, const std::map<std::u16string, std::u16string> &metainfo)
{
	// 若元参数为空则直接返回
	if (metainfo.empty()) return GetOrCreateType(name);

	// 若没有创建器则直接返回
	if (first == nullptr) return nullptr;

	// 返回创建结果（不保存，不复用）
	return first->CreateType(name, metainfo);
}

Basic::Type *TypeManager::GetType(std::u16string name)
{
	if (objects.contains(name)) return objects[name];

	return nullptr;
}

