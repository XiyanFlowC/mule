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

std::string BinaryBlock::GetTypeName() const
{
	return "BinaryBlock";
}

mule::Data::Basic::Type *BinaryBlock::BinaryBlockCreator::DoCreateObject(std::string info)
{
	return nullptr;
}
