#include "Array.h"
#include <xyutils.h>
#include <xystring.h>
#include "Basic/ContextManager.h"
#include "Basic/BasicType.h"
#include "../Configuration.h"

using namespace mule::Data::Basic;

const std::u16string ARRAY_SIZE_INFINITY = u"<inf>";

mule::Data::Array::Array()
	: length(0), innerObject(nullptr)
{
}

void mule::Data::Array::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	size_t limit = length;
	if (limit == (size_t)-1 && sizeCache != ARRAY_SIZE_INFINITY) limit = ContextManager::GetInstance().GetVariable(sizeCache).value.unsignedValue;
	for (size_t i = 0; i < limit; ++i) {
		dataHandler->OnRealmEnter(innerObject, (int)i);
		try
		{
			innerObject->Read(stream, dataHandler);
		}
		catch (Basic::BasicType::ConstraintViolationException &ex)
		{
			if (sizeCache == ARRAY_SIZE_INFINITY)
			{
				dataHandler->OnRealmExit(innerObject, (int) i);
				break;
			}
			else
			{
				throw ex;
			}
		}
		dataHandler->OnRealmExit(innerObject, (int)i);
	}
}

void mule::Data::Array::Write(xybase::Stream *stream, FileHandler * fileHandler)
{
	size_t limit = length;
	// 定义的长度是变量时，试图通过上下文信息获取变量数值
	if (limit == (size_t)-1 && sizeCache != ARRAY_SIZE_INFINITY) limit = ContextManager::GetInstance().GetVariable(sizeCache).value.unsignedValue;
	// 未定义长度时，尝试从 sheet 中获取长度定义
	if (limit == (size_t)-1 && sizeCache == ARRAY_SIZE_INFINITY && fileHandler->OnDataWrite().metadata.contains(u"size"))
	{
		limit = fileHandler->OnDataWrite().metadata.find(u"size")->second.value.unsignedValue;
	}
	for (size_t i = 0; i < limit; ++i) {
		fileHandler->OnRealmEnter(innerObject, (int)i);
		try
		{
			innerObject->Write(stream, fileHandler);
		}
		catch (Basic::BasicType::ConstraintViolationException &ex)
		{
			if (sizeCache == ARRAY_SIZE_INFINITY)
			{
				fileHandler->OnRealmExit(innerObject, (int)i);
				break;
			}
			else
			{
				throw ex;
			}
		}
		fileHandler->OnRealmExit(innerObject, (int)i);
	}
}

size_t mule::Data::Array::Size() const
{
	if (innerObject->Size() == (size_t)-1) return (size_t)-1;
	return length * innerObject->Size();
}

std::u16string mule::Data::Array::GetDataType() const
{
	return u"array/" + innerObject->GetDataType();
}

bool mule::Data::Array::IsComposite() const
{
	return true;
}

mule::Data::Basic::Type *mule::Data::Array::ArrayCreator::DoCreateObject(const std::u16string &info)
{
	if (!info.ends_with(u"]")) return nullptr;

	size_t startIndex = info.find_last_of(u"[");
	if (startIndex == std::u16string::npos) return nullptr;

	int size{ -1 };
	std::u16string cache;
	if (isdigit(info[startIndex + 1]))
		size = static_cast<int>(xybase::string::stoi(info.substr(startIndex + 1, info.length() - startIndex - 2)));
	else if (info[startIndex + 1] == u']')
		cache = ARRAY_SIZE_INFINITY;
	else
		cache = info.substr(startIndex + 1, info.length() - startIndex - 2);

	auto obj = TypeManager::GetInstance().GetOrCreateType(info.substr(0, startIndex));
	if (obj == nullptr) return nullptr;

	Array *ret = new Array();
	ret->length = size;
	ret->sizeCache = cache;
	ret->innerObject = obj;
	return ret;
}
