#include "Referrence.h"

#include <iostream>
#include <xystring.h>
using namespace mule::Data::Basic;

void mule::Data::Referrence::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	int ptr = stream->ReadInt32();

	if (ptr == 0)
	{
		dataHandler->OnDataRead(MultiValue::MV_NULL);
		return;
	}

	size_t loc = stream->Tell();
	stream->Seek(ptr, xybase::Stream::SM_BEGIN);
	referent->Read(stream, dataHandler);
	dataHandler->AppendMetadatum(u"ptr", MultiValue((uint64_t)ptr));
	stream->Seek(loc, xybase::Stream::SM_BEGIN);
}

void mule::Data::Referrence::Write(xybase::Stream *stream, FileHandler * fileHandler)
{
	// 避免从这里读入（喵的不刷洗的话会从缓存里读到不干净的东西，我他喵的到底为什么要这么写啊）
	//stream->Flush();
	//int ptr = stream->ReadInt32();
	stream->Seek(Size(), xybase::Stream::SM_CURRENT);
	auto val = fileHandler->OnDataWrite();
	int ptr = val.metadata[u"ptr"].value.unsignedValue;

	if (ptr == 0)
	{
		return;
	}

	size_t loc = stream->Tell();
	stream->Seek(ptr, xybase::Stream::SM_BEGIN);
	try
	{
		referent->Write(stream, fileHandler);
	}
	catch (xybase::InvalidParameterException &ex)
	{
		std::cerr << "ptr: " << ptr << std::endl;
		std::cerr << xybase::string::to_string(ex.GetMessage()) << std::endl;
	}
	stream->Seek(loc, xybase::Stream::SM_BEGIN);
}

size_t mule::Data::Referrence::Size() const
{
	return size_t(sizeof(int32_t));
}

std::u16string mule::Data::Referrence::GetDataType() const
{
	return referent->GetDataType();
}

bool mule::Data::Referrence::IsComposite() const
{
	return referent->IsComposite();
}

mule::Data::Referrence::Referrence(Type *referent)
{
	this->referent = referent;
}

Type *mule::Data::Referrence::ReferrenceCreator::DoCreateObject(std::u16string info)
{
	if (!info.ends_with(u"*"))
	{
		return nullptr;
	}

	Basic::Type *innerType = TypeManager::GetInstance().GetOrCreateType(info.substr(0, info.size() - 1));
	if (innerType == nullptr) return nullptr;

	Referrence *referrer = new Referrence(innerType);
	return referrer;
}
