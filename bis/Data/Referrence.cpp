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
	stream->Seek(ptr, 0);
	referent->Read(stream, dataHandler);
	dataHandler->AppendMetadatum(u"ptr", MultiValue((uint64_t)ptr));
	stream->Seek(loc, 0);
}

void mule::Data::Referrence::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	// 避免从这里读入（喵的不刷洗的话会从缓存里读到不干净的东西，我他喵的到底为什么要这么写啊）
	//stream->Flush();
	//int ptr = stream->ReadInt32();
	stream->Seek(Size(), SEEK_CUR);
	int ptr = dataHandler->OnDataWrite().metadata[u"ptr"].value.unsignedValue;

	if (ptr == 0)
	{
		return;
	}

	size_t loc = stream->Tell();
	stream->Seek(ptr, 0);
	try
	{
		referent->Write(stream, dataHandler);
	}
	catch (xybase::InvalidParameterException &ex)
	{
		std::cerr << "ptr: " << ptr << std::endl;
		std::cerr << xybase::string::to_string(ex.GetMessage()) << std::endl;
	}
	stream->Seek(loc, 0);
}

size_t mule::Data::Referrence::Size() const
{
	return size_t(sizeof(int32_t));
}

std::u16string mule::Data::Referrence::GetTypeName() const
{
	return referent->GetTypeName() + u'*';
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
