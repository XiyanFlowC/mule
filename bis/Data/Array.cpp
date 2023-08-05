#include "Array.h"

using namespace mule::Data::Basic;

void mule::Data::Array::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	dataHandler->OnRealmEnter((Type *)this, name);
	for (int i = 0; i < length; ++i) {
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
	for (int i = 0; i < length; ++i) {
		dataHandler->OnRealmEnter(innerObject, i);
		innerObject->Write(stream, dataHandler);
		dataHandler->OnRealmExit(innerObject, i);
	}
	dataHandler->OnRealmExit((Type *)this, name);
}

size_t mule::Data::Array::Size() const
{
	return size_t();
}

std::string mule::Data::Array::GetTypeName() const
{
	return std::string("array");
}

mule::Data::Basic::Type *mule::Data::Array::ArrayCreator::DoCreateObject(std::string info)
{
	return nullptr;
}
