#include "TableRef.h"

#include <Data/Basic/ContextManager.h>
#include <Data/TypeManager.h>

size_t mule::Data::TableRef::Size() const
{
	return size_t(0);
}

std::u16string mule::Data::TableRef::GetDataType() const
{
	return infraType->GetDataType() + u'@';
}

void mule::Data::TableRef::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	auto loc = stream->Tell();
	auto tloc = mule::Data::Basic::ContextManager::GetInstance().GetVariable(locCacheName);
	auto tsiz = mule::Data::Basic::ContextManager::GetInstance().GetVariable(sizeCacheName);
	if ((!tloc.IsType(Basic::MultiValue::MVT_UINT) && !tloc.IsType(Basic::MultiValue::MVT_INT))
		|| (!tsiz.IsType(Basic::MultiValue::MVT_UINT) && !tsiz.IsType(Basic::MultiValue::MVT_INT))) return;

	stream->Seek(tloc.value.signedValue, xybase::Stream::SM_BEGIN);
	for (int i = 0; i < tsiz.value.signedValue; ++i)
	{
		dataHandler->OnRealmEnter(infraType, i);
		infraType->Read(stream, dataHandler);
		dataHandler->OnRealmExit(infraType, i);
	}
	stream->Seek(loc, xybase::Stream::SM_BEGIN);
}

void mule::Data::TableRef::Write(xybase::Stream *stream, FileHandler * fileHandler)
{
	auto loc = stream->Tell();
	auto tloc = mule::Data::Basic::ContextManager::GetInstance().GetVariable(locCacheName);
	auto tsiz = mule::Data::Basic::ContextManager::GetInstance().GetVariable(sizeCacheName);
	if ((!tloc.IsType(Basic::MultiValue::MVT_UINT) && !tloc.IsType(Basic::MultiValue::MVT_INT))
		|| (!tsiz.IsType(Basic::MultiValue::MVT_UINT) && !tsiz.IsType(Basic::MultiValue::MVT_INT))) return;

	stream->Seek(tloc.value.signedValue, xybase::Stream::SM_BEGIN);
	for (int i = 0; i < tsiz.value.signedValue; ++i)
	{
		fileHandler->OnRealmEnter(infraType, i);
		infraType->Write(stream, fileHandler);
		fileHandler->OnRealmExit(infraType, i);
	}
	stream->Seek(loc, xybase::Stream::SM_BEGIN);
}

bool mule::Data::TableRef::IsComposite() const
{
	return true;
}

mule::Data::Basic::Type *mule::Data::TableRef::TableRefCreator::DoCreateObject(const std::u16string &info)
{
	size_t splt = info.find_last_of('@');
	if (splt == std::u16string::npos)
		return nullptr;

	size_t sizeStart = info.find_first_of('(', splt);
	if (sizeStart == std::u16string::npos || !info.ends_with(')')) return nullptr;

	auto infra = TypeManager::GetInstance().GetOrCreateType(info.substr(0, splt));
	if (infra == nullptr) return nullptr;

	TableRef *ret = new TableRef();

	ret->infraType = infra;
	ret->locCacheName = info.substr(splt + 1, sizeStart - splt - 1);
	ret->sizeCacheName = info.substr(sizeStart + 1, info.size() - sizeStart - 2);

	return ret;
}

mule::Data::Basic::MultiValue mule::Data::TableRef::ReadValue(xybase::Stream *stream)
{
	mule::Data::Basic::MultiValue ret{ mule::Data::Basic::MultiValue::MVT_MAP };
	auto loc = stream->Tell();
	auto tloc = mule::Data::Basic::ContextManager::GetInstance().GetVariable(locCacheName);
	auto tsiz = mule::Data::Basic::ContextManager::GetInstance().GetVariable(sizeCacheName);
	if ((!tloc.IsType(Basic::MultiValue::MVT_UINT) && !tloc.IsType(Basic::MultiValue::MVT_INT))
		|| (!tsiz.IsType(Basic::MultiValue::MVT_UINT) && !tsiz.IsType(Basic::MultiValue::MVT_INT))) return ret;

	stream->Seek(tloc.value.signedValue, xybase::Stream::SM_BEGIN);
	for (int i = 0; i < tsiz.value.signedValue; ++i)
	{
		ret[(uint64_t)i] = infraType->ReadValue(stream);
	}
	stream->Seek(loc, xybase::Stream::SM_BEGIN);

	return ret;
}

void mule::Data::TableRef::WriteValue(xybase::Stream *stream, mule::Data::Basic::MultiValue value)
{
	auto loc = stream->Tell();
	auto tloc = mule::Data::Basic::ContextManager::GetInstance().GetVariable(locCacheName);
	auto tsiz = mule::Data::Basic::ContextManager::GetInstance().GetVariable(sizeCacheName);
	if ((!tloc.IsType(Basic::MultiValue::MVT_UINT) && !tloc.IsType(Basic::MultiValue::MVT_INT))
		|| (!tsiz.IsType(Basic::MultiValue::MVT_UINT) && !tsiz.IsType(Basic::MultiValue::MVT_INT))) return;

	stream->Seek(tloc.value.signedValue, xybase::Stream::SM_BEGIN);
	for (int i = 0; i < tsiz.value.signedValue; ++i)
	{
		infraType->WriteValue(stream, value[(uint64_t) i]);
	}
	stream->Seek(loc, xybase::Stream::SM_BEGIN);
}
