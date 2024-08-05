#include "Reference.h"

#include <iostream>
#include <xystring.h>
#include "../Configuration.h"
#include "Basic/BasicType.h"

using namespace mule::Data::Basic;

void mule::Data::Reference::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	int ptr = stream->ReadInt32();

	if (ptr == 0)
	{
		if (referent->IsComposite())
			dataHandler->AppendMetadatum(u"ptr", (int64_t)0);
		dataHandler->OnDataRead(MultiValue::MV_NULL);
		dataHandler->AppendMetadatum(u"ptr", (int64_t)ptr);
		if (ptrReject && ptrRejectValue == ptr) throw BasicType::ConstraintViolationException(L"ptr rejected.");

		return;
	}
	if (ptrReject && ptrRejectValue == ptr) throw BasicType::ConstraintViolationException(L"ptr rejected.");

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
	if (!referent->IsComposite() && val.IsType(MultiValue::MVT_NULL))
	{
		stream->Write((int32_t)0);
		if (ptrReject && ptrRejectValue == 0) throw BasicType::ConstraintViolationException(L"ptr rejected.");
		return;
	}

	size_t ptr = val.metadata[u"ptr"].value.unsignedValue;
	if (!val.metadata[u"ptr"].IsType(MultiValue::MVT_INT))
	{
		ptr = stream->ReadUInt32();
		stream->Seek(-4, xybase::Stream::SM_CURRENT);
	}

	stream->Write((uint32_t)ptr);
	if (ptr == 0)
	{
		if (ptrReject && ptrRejectValue == ptr) throw BasicType::ConstraintViolationException(L"ptr rejected.");
		return;
	}
	if (ptrReject && ptrRejectValue == ptr) throw BasicType::ConstraintViolationException(L"ptr rejected.");

	// 不是复杂对象，检查是否早已写入过
	if (!referent->IsComposite())
	{
		if (ReferenceRegistry::GetInstance().IsRegistered(stream, ptr)) return;
		// 未写入过，登录
		ReferenceRegistry::GetInstance().Register(stream, ptr);
	}

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

mule::Data::Reference::ReferenceRegistry::ReferenceRegistry()
{
}

void mule::Data::Reference::ReferenceRegistry::RemoveStream(xybase::Stream *sender)
{
	records.erase(sender->GetName());
}

mule::Data::Reference::ReferenceRegistry &mule::Data::Reference::ReferenceRegistry::GetInstance()
{
	static ReferenceRegistry _inst;
	return _inst;
}

bool mule::Data::Reference::ReferenceRegistry::IsRegistered(xybase::Stream *stream, size_t loc)
{
	// 流未登录，返回
	if (!records.contains(stream->GetName())) return false;

	// 返回是否存在
	return records[stream->GetName()].contains(loc);
}

void mule::Data::Reference::ReferenceRegistry::Register(xybase::Stream *stream, size_t loc)
{
	// 流未登录，登录流
	if (!records.contains(stream->GetName()))
	{
		records.insert({ });
		// 在流关闭时自动注销
		stream->OnClose += [](auto sender) -> void {
			ReferenceRegistry::GetInstance().RemoveStream(sender);
		};
	}

	auto &target = records[stream->GetName()];
	// 存在同地址记录，忽略
	if (target.find(loc) != target.end()) return;
	// 新记录，登录
	target.insert(loc);
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
	if (metainfo.contains(u"ptr-reject"))
	{
		referrer->ptrReject = true;
		referrer->ptrRejectValue = xybase::string::stoi(metainfo.find(u"ptr-reject")->second);
	}
	return referrer;
}
