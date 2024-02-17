#include "Reference.h"

#include <iostream>
#include <xystring.h>
#include "../Configuration.h"
#include "ReferenceManager.h"

using namespace mule::Data::Basic;

void mule::Data::Reference::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	int ptr = stream->ReadInt32();

	if (ptr == 0)
	{
		dataHandler->OnDataRead(MultiValue::MV_NULL);
		dataHandler->AppendMetadatum(u"ptr", (int64_t)ptr);
		return;
	}

	size_t loc = stream->Tell();
	stream->Seek(ptr, xybase::Stream::SM_BEGIN);
	if (referent->IsComposite())
		dataHandler->AppendMetadatum(u"ptr", MultiValue((int64_t)ptr));
	referent->Read(stream, dataHandler);
	dataHandler->AppendMetadatum(u"ptr", MultiValue((int64_t)ptr));
	stream->Seek(loc, xybase::Stream::SM_BEGIN);
}

void mule::Data::Reference::Write(xybase::Stream *stream, FileHandler * fileHandler)
{
	auto val = fileHandler->OnDataWrite();
	size_t ptr = val.metadata[u"ptr"].value.unsignedValue;
	if (!val.metadata[u"ptr"].IsType(MultiValue::MVT_INT))
	{
		ptr = stream->ReadUInt32();
		stream->Seek(-4, xybase::Stream::SM_CURRENT);
		if (Configuration::GetInstance().GetSigned(u"mule.data.reference.quiet", 0) != 1)
		{
			logger.Warn(L"Pointer not found. Read {} from stream. May corrupt so be careful.", ptr);
			logger.Note(L"Metadata lost. The file may corrupt or using a data lossy data format.");
			logger.Note(L"To mute this warning, set mule.data.reference.quiet=1");
		}
	}

	stream->Write((uint32_t)ptr);
	if (ptr == 0)
	{
		return;
	}

	if (!ReferenceManager::GetInstance().RegisterItem(stream, ptr)) return;

	size_t loc = stream->Tell();
	stream->Seek((long long)ptr, xybase::Stream::SM_BEGIN);
	try
	{
		referent->Write(stream, fileHandler);
	}
	catch (xybase::InvalidParameterException &ex)
	{
		logger.Warn(L"For reference (ptr: {}), an invalid parameter exception detected. (ignored)", ptr, ex.GetErrorCode());
		logger.Note(L"[{:X}] {}", ex.GetErrorCode(), ex.GetMessage());
	}
	stream->Seek(loc, xybase::Stream::SM_BEGIN);
}

size_t mule::Data::Reference::Size() const
{
	return size_t(sizeof(int32_t));
}

std::u16string mule::Data::Reference::GetDataType() const
{
	return referent->GetDataType();
}

bool mule::Data::Reference::IsComposite() const
{
	//return true;
	return referent->IsComposite();
}

mule::Data::Reference::Reference(Type *referent)
{
	this->referent = referent;
}

Type *mule::Data::Reference::ReferenceCreator::DoCreateObject(const std::u16string &info)
{
	if (!info.ends_with(u"*"))
	{
		return nullptr;
	}

	Basic::Type *innerType = TypeManager::GetInstance().GetOrCreateType(info.substr(0, info.size() - 1));
	if (innerType == nullptr) return nullptr;

	Reference *referrer = new Reference(innerType);
	return referrer;
}

Type *mule::Data::Reference::ReferenceCreator::DoCreateObject(const std::u16string &info, const std::map<std::u16string, std::u16string> &metainfo)
{
	if (!info.ends_with(u"*"))
	{
		return nullptr;
	}

	Basic::Type *innerType = TypeManager::GetInstance().GetOrCreateType(info.substr(0, info.size() - 1), metainfo);
	if (innerType == nullptr) return nullptr;

	Reference *referrer = new Reference(innerType);
	return referrer;
}
