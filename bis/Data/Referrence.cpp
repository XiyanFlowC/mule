#include "Referrence.h"

using namespace mule::Data::Basic;

void mule::Data::Referrence::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	int ptr = stream->ReadInt32();
	size_t loc = stream->Tell();
	stream->Seek(ptr, 0);
	referent->Read(stream, dataHandler);
	dataHandler->AppendMetadatum("addr", (unsigned long long)ptr);
	stream->Seek(loc, 0);
}

void mule::Data::Referrence::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	int ptr = stream->ReadInt32();
	size_t loc = stream->Tell();
	stream->Seek(ptr, 0);
	referent->Write(stream, dataHandler);
	stream->Seek(loc, 0);
}

size_t mule::Data::Referrence::Size() const
{
	return size_t(sizeof(int32_t));
}

std::string mule::Data::Referrence::GetTypeName() const
{
	return referent->GetTypeName() + "*";
}

mule::Data::Referrence::Referrence(Type *referent)
{
	this->referent = referent;
}

Type *mule::Data::Referrence::ReferrenceObjectCreator::DoCreateObject(std::string info)
{
	if (!info.ends_with("*"))
	{
		return nullptr;
	}

	Basic::Type *innerType = TypeManager::GetInstance().GetOrCreateObject(info.substr(0, info.size() - 1));
	if (innerType == nullptr) return nullptr;

	Referrence *referrer = new Referrence(innerType);
	return referrer;
}
