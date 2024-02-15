#include "Anchor.h"
#include "TypeManager.h"

mule::Data::Anchor::Anchor(Basic::Type *infraType, size_t offset, const std::u16string &name)
	: infraType(infraType), offset(offset), name(name)
{
}

size_t mule::Data::Anchor::Size() const
{
	return infraType->Size();
}

std::u16string mule::Data::Anchor::GetDataType() const
{
	return infraType->GetDataType();
}

bool mule::Data::Anchor::IsComposite() const
{
	return true;
}

void mule::Data::Anchor::Read(xybase::Stream *stream, mule::Data::Basic::Type::DataHandler *dataHandler)
{
	stream->Seek(offset, xybase::Stream::SM_BEGIN);
	dataHandler->OnRealmEnter(this, name);
	infraType->Read(stream, dataHandler);
	dataHandler->OnRealmExit(this, name);
}

void mule::Data::Anchor::Write(xybase::Stream *stream, mule::Data::Basic::Type::FileHandler *fileHandler)
{
	stream->Seek(offset, xybase::Stream::SM_BEGIN);
	fileHandler->OnRealmEnter(infraType, name);
	infraType->Write(stream, fileHandler);
	fileHandler->OnRealmExit(infraType, name);
}

void mule::Data::Anchor::WriteValue(xybase::Stream *stream, mule::Data::Basic::MultiValue mv)
{
	stream->Seek(offset);
	infraType->WriteValue(stream, mv);
}

mule::Data::Basic::MultiValue mule::Data::Anchor::ReadValue(xybase::Stream *stream)
{
	stream->Seek(offset);
	return infraType->ReadValue(stream);
}
