#include "Referrence.h"

#include <iostream>
#include <xystring.h>
using namespace mule::Data::Basic;

void mule::Data::Referrence::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	int ptr = stream->ReadInt32();

	if (ptr == 0)
	{
		if (referent->IsComposite())
		{
			auto dummy = TypeManager::GetInstance().GetOrCreateType(u"uint32");
			dataHandler->OnRealmEnter(dummy, u"ptr");
			dataHandler->OnDataRead(MultiValue((uint64_t)ptr));
			dataHandler->OnRealmExit(dummy, u"ptr");
		}
		else
		{
			dataHandler->OnDataRead(MultiValue::MV_NULL);
			dataHandler->AppendMetadatum(u"ptr", (uint64_t)ptr);
		}
		return;
	}

	size_t loc = stream->Tell();
	stream->Seek(ptr, xybase::Stream::SM_BEGIN);
	if (referent->IsComposite())
	{
		auto dummy = TypeManager::GetInstance().GetOrCreateType(u"uint32");
		dataHandler->OnRealmEnter(dummy, u"ptr");
		dataHandler->OnDataRead(MultiValue((uint64_t)ptr));
		dataHandler->OnRealmExit(dummy, u"ptr");
		dataHandler->OnRealmEnter(referent, u"referent");
		referent->Read(stream, dataHandler);
		dataHandler->OnRealmExit(referent, u"referent");
	}
	else
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
	size_t ptr;
	if (referent->IsComposite())
	{
		auto dummy = TypeManager::GetInstance().GetOrCreateType(u"uint32");
		fileHandler->OnRealmEnter(dummy, u"ptr");
		ptr = fileHandler->OnDataWrite().value.unsignedValue;
		fileHandler->OnRealmExit(dummy, u"ptr");
	}
	else
		ptr = val.metadata[u"ptr"].value.unsignedValue;

	if (ptr == 0)
	{
		return;
	}

	size_t loc = stream->Tell();
	stream->Seek((long long)ptr, xybase::Stream::SM_BEGIN);
	try
	{
		if (referent->IsComposite())
		{
			fileHandler->OnRealmEnter(referent, u"referent");
			referent->Write(stream, fileHandler);
			fileHandler->OnRealmExit(referent, u"referent");
		}
		else
			referent->Write(stream, fileHandler);
	}
	catch (xybase::InvalidParameterException &ex)
	{
		logger.Warn(L"For reference (ptr: {}), an invalid parameter exception detected. (ignored)", ptr, ex.GetErrorCode());
		logger.Note(L"[{:X}] {}", ex.GetErrorCode(), ex.GetMessage());
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
	//return true;
	return referent->IsComposite();
}

mule::Data::Referrence::Referrence(Type *referent)
{
	this->referent = referent;
}

Type *mule::Data::Referrence::ReferrenceCreator::DoCreateObject(const std::u16string &info)
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

Type *mule::Data::Referrence::ReferrenceCreator::DoCreateObject(const std::u16string &info, const std::map<std::u16string, std::u16string> &metainfo)
{
	if (!info.ends_with(u"*"))
	{
		return nullptr;
	}

	Basic::Type *innerType = TypeManager::GetInstance().GetOrCreateType(info.substr(0, info.size() - 1), metainfo);
	if (innerType == nullptr) return nullptr;

	Referrence *referrer = new Referrence(innerType);
	return referrer;
}
