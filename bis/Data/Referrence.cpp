#include "Referrence.h"

using namespace mule::Data::Basic;

void mule::Data::Referrence::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	int ptr = stream->ReadInt32();
	size_t loc = stream->Tell();
	referent->Read(stream, dataHandler);
	stream->Seek(loc, 0);
}

void mule::Data::Referrence::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	int ptr = stream->ReadInt32();
	size_t loc = stream->Tell();
	referent->Write(stream, dataHandler);
	stream->Seek(loc, 0);
}

size_t mule::Data::Referrence::Size() const
{
	return size_t(sizeof(int32_t));
}

std::string mule::Data::Referrence::GetTypeName() const
{
	return std::string("ref:") + referent->GetTypeName();
}

mule::Data::Referrence::Referrence(Object *referent)
{
	this->referent = referent;
}

inline Object *mule::Data::Referrence::ReferrenceObjectCreator::DoCreateObject(std::string info)
{
	if (!info.starts_with("ref:"))
	{
		return nullptr;
	}

	Basic::Object *innerType = ObjectManager::GetInstance().GetObject(info.substr(4));
	if (innerType == nullptr) return nullptr;

	Referrence *referrer = new Referrence(innerType);
	return referrer;
}
