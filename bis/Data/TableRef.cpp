#include "TableRef.h"

#include "Basic/ContextManager.h"
#include "TypeManager.h"

size_t mule::Data::TableRef::Size() const
{
	return size_t(0);
}

std::u16string mule::Data::TableRef::GetTypeName() const
{
	return infraType->GetTypeName() + u'@';
}

void mule::Data::TableRef::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	auto loc = stream->Tell();
	auto tloc = mule::Data::Basic::ContextManager::GetInstance().GetVariable(locCacheName);
	auto tsiz = mule::Data::Basic::ContextManager::GetInstance().GetVariable(sizeCacheName);
	if ((!tloc.IsType(Basic::MultiValue::MVT_UINT) && !tloc.IsType(Basic::MultiValue::MVT_INT))
		|| (!tsiz.IsType(Basic::MultiValue::MVT_UINT) && !tsiz.IsType(Basic::MultiValue::MVT_INT))) return;

	stream->Seek(tloc.value.signedValue, 0);
	for (int i = 0; i < tsiz.value.signedValue; ++i)
	{
		dataHandler->OnRealmEnter(infraType, i);
		infraType->Read(stream, dataHandler);
		dataHandler->OnRealmExit(infraType, i);
	}
	stream->Seek(loc, 0);
}

void mule::Data::TableRef::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	auto loc = stream->Tell();
	auto tloc = mule::Data::Basic::ContextManager::GetInstance().GetVariable(locCacheName);
	auto tsiz = mule::Data::Basic::ContextManager::GetInstance().GetVariable(sizeCacheName);
	if ((!tloc.IsType(Basic::MultiValue::MVT_UINT) && !tloc.IsType(Basic::MultiValue::MVT_INT))
		|| (!tsiz.IsType(Basic::MultiValue::MVT_UINT) && !tsiz.IsType(Basic::MultiValue::MVT_INT))) return;

	stream->Seek(tloc.value.signedValue, 0);
	for (int i = 0; i < tsiz.value.signedValue; ++i)
	{
		dataHandler->OnRealmEnter(infraType, i);
		infraType->Write(stream, dataHandler);
		dataHandler->OnRealmExit(infraType, i);
	}
	stream->Seek(loc, 0);
}

bool mule::Data::TableRef::IsComposite() const
{
	return true;
}

mule::Data::Basic::Type *mule::Data::TableRef::TableRefCreator::DoCreateObject(std::u16string info)
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
