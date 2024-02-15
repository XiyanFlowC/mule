#include "Sheet.h"

size_t mule::Data::Sheet::Size() const
{
	if (infraType->Size() == -1) return -1;
	return infraType->Size() * length;
}

std::u16string mule::Data::Sheet::GetDataType() const
{
	return std::u16string(u"Sheet");
}

bool mule::Data::Sheet::IsComposite() const
{
	return true;
}

void mule::Data::Sheet::Read(xybase::Stream *stream, mule::Data::Basic::Type::DataHandler *dataHandler)
{
	dataHandler->OnRealmEnter(this, name);
	stream->Seek(offset, xybase::Stream::SeekMode::SM_BEGIN);
	for (int i = 0; i < length; ++i) {
		dataHandler->OnRealmEnter(infraType, i);
		infraType->Read(stream, dataHandler);
		dataHandler->OnRealmExit(infraType, i);
	}
	dataHandler->OnRealmExit(this, name);
}

void mule::Data::Sheet::Write(xybase::Stream *stream, mule::Data::Basic::Type::FileHandler *fileHandler)
{
	fileHandler->OnRealmEnter(this, name);
	stream->Seek(offset, xybase::Stream::SeekMode::SM_BEGIN);
	for (int i = 0; i < length; ++i) {
		fileHandler->OnRealmEnter(infraType, i);
		infraType->Write(stream, fileHandler);
		fileHandler->OnRealmExit(infraType, i);
	}
	fileHandler->OnRealmExit(this, name);
}
