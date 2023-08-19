#include "Array.h"
#include "../xybase/xyutils.h"

using namespace mule::Data::Basic;

void mule::Data::Array::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	for (size_t i = 0; i < length; ++i) {
		dataHandler->OnRealmEnter(innerObject, i);
		innerObject->Read(stream, dataHandler);
		dataHandler->OnRealmExit(innerObject, i);
	}
}

void mule::Data::Array::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	for (size_t i = 0; i < length; ++i) {
		dataHandler->OnRealmEnter(innerObject, i);
		innerObject->Write(stream, dataHandler);
		dataHandler->OnRealmExit(innerObject, i);
	}
}

size_t mule::Data::Array::Size() const
{
	return length * innerObject->Size();
}

std::u16string mule::Data::Array::GetTypeName() const
{
	return innerObject->GetTypeName() + u"[" + xybase::string::to_utf16(std::to_string(length)) + u"]";
}

bool mule::Data::Array::IsComposite() const
{
	return true;
}

MultiValue mule::Data::Array::DoRead(xybase::Stream *stream)
{
	return MultiValue();
}

void mule::Data::Array::DoWrite(xybase::Stream *stream, const MultiValue &value)
{
}

mule::Data::Basic::Type *mule::Data::Array::ArrayCreator::DoCreateObject(std::u16string info)
{
	if (!info.ends_with(u"]")) return nullptr;

	size_t startIndex = info.find_last_of(u"[");
	if (startIndex == std::u16string::npos) return nullptr;

	int size = static_cast<int>(xybase::string::stoi(info.substr(startIndex + 1, info.length() - startIndex - 2)));

	auto obj = TypeManager::GetInstance().GetOrCreateObject(info.substr(0, startIndex));
	if (obj == nullptr) return nullptr;

	Array *ret = new Array();
	ret->length = size;
	ret->innerObject = obj;
	return ret;
}
