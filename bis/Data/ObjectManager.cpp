#include "ObjectManager.h"

using namespace mule::Data;

ObjectManager::ObjectManager()
{
	first = last = nullptr;
}

ObjectManager::~ObjectManager()
{
	for (auto &object : objects)
	{
		delete object.second;
	}
	objects.clear();
}

ObjectManager &ObjectManager::GetInstance()
{
	static ObjectManager _inst;
	return _inst;
}

void ObjectManager::RegisterObject(Basic::Object *object, std::string name)
{
	objects[name] = object;
}

void ObjectManager::UnregisterObject(std::string name)
{
	objects.erase(name);
}

void ObjectManager::ReleaseObject(std::string name)
{
	if (objects.contains(name)) {
		delete objects[name];
		objects.erase(name);
	}
}

void ObjectManager::RegisterObjectCreator(ObjectCreator *creator)
{
	if (creator->nextCreator != nullptr) throw Exception::InvalidParameterException(
		"creator",
		"The field nextCreator must be nullptr",
		__FILE__, __LINE__
		);

	if (last == nullptr)
	{
		first = last = creator;
		return;
	}

	last->nextCreator = creator;
	last = creator;
}

Basic::Structure *ObjectManager::NewStructure(std::string name) {
	Basic::Structure *ret = new Basic::Structure();
	RegisterObject(ret, name);
	return ret;
}

Basic::Object *ObjectManager::GetOrCreateObject(std::string info)
{
	// ���Ѿ�������ֱ�ӷ���
	if (objects.contains(info)) return objects[info];

	// ��û�д�������ֱ�ӷ���
	if (first == nullptr) return nullptr;

	// ���洴�����������
	return objects[info] = first->GetOrCreateObject(info);
}

Basic::Object *ObjectManager::GetObject(std::string name)
{
	if (objects.contains(name)) return objects[name];

	return nullptr;
}

