#include "BinaryBlock.h"

#include <xystring.h>
#include <Storage/DataManager.h>

using namespace mule::Data::Basic;
using namespace mule::Data::Storage;
using namespace mule::Storage;

mule::Data::Storage::BinaryBlock::BinaryBlock(size_t size)
	: size(size)
{
}

void BinaryBlock::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	char *buffer = new char[size];
	stream->ReadBytes(buffer, size);

	// 生成 link
	MultiValue mv = DataManager::GetInstance().SaveData(BinaryData{buffer, size, false});
	dataHandler->OnDataRead(mv);
}

void BinaryBlock::Write(xybase::Stream *stream, FileHandler * fileHandler)
{
	MultiValue mv = fileHandler->OnDataWrite();
	if (!mv.IsType(MultiValue::MVT_UINT))
	{
		stream->Seek(size, xybase::Stream::SM_CURRENT);
		return;
	}
	
	// 获取 link 的数据
	stream->Write(DataManager::GetInstance().LoadData((unsigned int)mv.value.unsignedValue).GetData(), size);
}

size_t BinaryBlock::Size() const
{
	return size;
}

std::u16string BinaryBlock::GetDataType() const
{
	return u"bin{" + xybase::string::itos<char16_t>(size) + u"}";
}

mule::Data::Basic::Type *BinaryBlock::BinaryBlockCreator::DoCreateObject(const std::u16string &info)
{
	if (!info.starts_with(u"bin{"))
	{
		return nullptr;
	}

	size_t loc = info.find_first_of(u"}");

	if (loc == std::u16string::npos) return nullptr;

	size_t size = xybase::string::stoi(info.substr(4, loc - 4));
	if (size == 0) return nullptr;

	auto ret = new BinaryBlock{size};
	return ret;
}
