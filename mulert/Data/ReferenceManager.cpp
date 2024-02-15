#include "ReferenceManager.h"

using namespace mule::Data;

void mule::Data::ReferenceManager::StreamCloseHandler(xybase::Stream *sender)
{
	ReferenceManager::GetInstance().UnregisterItem(sender);
}

ReferenceManager &mule::Data::ReferenceManager::GetInstance()
{
	static ReferenceManager _inst;
	return _inst;
}

bool mule::Data::ReferenceManager::RegisterItem(xybase::Stream *stream, size_t offset)
{
	// 流未登录，登录流
	if (records.find(stream) == records.end())
	{
		records.insert({ });
		// 在流关闭时自动注销
		stream->OnClose += StreamCloseHandler;
	}

	auto &target = records[stream];
	// 存在同地址记录，忽略
	if (target.find(offset) != target.end()) return false;
	target.insert(offset);
	return true;
}

void mule::Data::ReferenceManager::UnregisterItem(xybase::Stream *stream)
{
	stream->OnClose -= StreamCloseHandler;
	records.erase(stream);
}
