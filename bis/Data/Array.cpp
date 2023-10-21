#include "Array.h"
#include <xyutils.h>
#include <xystring.h>
#include "Basic/ContextManager.h"

using namespace mule::Data::Basic;

void mule::Data::Array::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	size_t limit = length;
	if (limit == (size_t)-1) limit = ContextManager::GetInstance().GetVariable(sizeCache).value.unsignedValue;
	for (size_t i = 0; i < limit; ++i) {
		dataHandler->OnRealmEnter(innerObject, (int)i);
		innerObject->Read(stream, dataHandler);
		dataHandler->OnRealmExit(innerObject, (int)i);
	}
}

void mule::Data::Array::Write(xybase::Stream *stream, FileHandler * fileHandler)
{
	size_t limit = length;
	if (limit == (size_t)-1) limit = ContextManager::GetInstance().GetVariable(sizeCache).value.unsignedValue;
	for (size_t i = 0; i < limit; ++i) {
		fileHandler->OnRealmEnter(innerObject, (int)i);
		innerObject->Write(stream, fileHandler);
		fileHandler->OnRealmExit(innerObject, (int)i);
	}
}

size_t mule::Data::Array::Size() const
{
	return length * innerObject->Size();
}

std::u16string mule::Data::Array::GetDataType() const
{
	return innerObject->GetDataType() + u"[" + xybase::string::to_utf16(std::to_string(length)) + u"]";
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
