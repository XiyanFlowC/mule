#include "BinaryBlock.h"

void BinaryBlock::Read(xybase::Stream *stream, DataHandler *dataHandler)
{

}

void BinaryBlock::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
}

size_t BinaryBlock::Size() const
{
	return size_t();
}

std::u16string BinaryBlock::GetTypeName() const
{
	return u"BinaryBlock";
}

mule::Data::Basic::Type *BinaryBlock::BinaryBlockCreator::DoCreateObject(std::u16string info)
{
	return nullptr;
}
