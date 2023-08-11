#include "Array.h"

using namespace mule::Data::Basic;

void mule::Data::Array::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	dataHandler->OnRealmEnter((Type *)this, name);
	for (size_t i = 0; i < length; ++i) {
		dataHandler->OnRealmEnter(innerObject, i);
		innerObject->Read(stream, dataHandler);
		dataHandler->OnRealmExit(innerObject, i);
	}
	dataHandler->AppendMetadatum("_type", MultiValue("array"));
	dataHandler->OnRealmExit((Type *)this, name);
}

void mule::Data::Array::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	dataHandler->OnRealmEnter((Type *)this, name);
	for (size_t i = 0; i < length; ++i) {
		dataHandler->OnRealmEnter(innerObject, i);
		innerObject->Write(stream, dataHandler);
		dataHandler->OnRealmExit(innerObject, i);
	}
	dataHandler->OnRealmExit((Type *)this, name);
}

size_t mule::Data::Array::Size() const
{
	return length * innerObject->Size();
}

std::string mule::Data::Array::GetTypeName() const
{
	return innerObject->GetTypeName() + "[" + std::to_string(length) + "]";
}

bool mule::Data::Array::IsComposite() const
{
	return true;
}

mule::Data::Basic::Type *mule::Data::Array::ArrayCreator::DoCreateObject(std::string info)
{
	if (!info.ends_with("]")) return nullptr;

	size_t startIndex = info.find_last_of("[");
	if (startIndex == std::string::npos) return nullptr;

	int size = static_cast<int>(xybase::string::stoi(info.substr(startIndex + 1, info.length() - startIndex - 2)));

	auto obj = TypeManager::GetInstance().GetOrCreateObject(info.substr(0, startIndex));
	if (obj == nullptr) return nullptr;

	Array *ret = new Array();
	ret->length = size;
	ret->innerObject = obj;
	return ret;
}
