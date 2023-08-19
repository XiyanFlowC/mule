#include "BinaryBlock.h"

using namespace mule::Data::Basic;

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

MultiValue BinaryBlock::DoRead(xybase::Stream *stream)
{
	return MultiValue();
}

void BinaryBlock::DoWrite(xybase::Stream *stream, const MultiValue &value)
{
}

mule::Data::Basic::Type *BinaryBlock::BinaryBlockCreator::DoCreateObject(std::u16string info, const mule::Data::Basic::MultiValue &extraInfo)
{
	return nullptr;
}

mule::Data::Basic::Type *BinaryBlock::BinaryBlockCreator::DoCreateObject(std::u16string info)
{
	return nullptr;
}
