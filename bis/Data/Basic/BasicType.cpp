#include "BasicType.h"

mule::Data::Basic::BasicType::ConstraintViolationException::ConstraintViolationException(const std::wstring &msg)
	: xybase::RuntimeException(msg, 0xFC0E)
{}

void mule::Data::Basic::BasicType::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	auto val = DoRead(stream);
	if (!cacheVariableName.empty())
	{
		ContextManager::GetInstance().SetVariable(cacheVariableName, val);
	}
	dataHandler->OnDataRead(val);
	DoConstraintCheck(val);
}

void mule::Data::Basic::BasicType::Write(xybase::Stream *stream, FileHandler * fileHandler)
{
	MultiValue value = fileHandler->OnDataWrite();
	if (MultiValue::MV_NULL == value)
	{
		size_t size = Size();
		if (size == static_cast<size_t>(-1)) return;
		stream->Seek(size, xybase::Stream::SM_CURRENT);
		return;
	}

	if (!cacheVariableName.empty())
	{
		ContextManager::GetInstance().SetVariable(cacheVariableName, value);
	}
	DoWrite(stream, value);
	DoConstraintCheck(value);
}

void mule::Data::Basic::BasicType::DoConstraintCheck(const MultiValue &value)
{
	switch (constraintType)
	{
	case mule::Data::Basic::BasicType::BTCT_NONE:
		return;
		break;
	case mule::Data::Basic::BasicType::BTCT_EQ:
		if (value != comparator) throw ConstraintViolationException(L"Not equal");
		break;
	case mule::Data::Basic::BasicType::BTCT_LT:
		if (value >= comparator) throw ConstraintViolationException(L"Not less than");
		break;
	case mule::Data::Basic::BasicType::BTCT_GT:
		if (value <= comparator) throw ConstraintViolationException(L"Not grater than");
		break;
	case mule::Data::Basic::BasicType::BTCT_NEQ:
		if (value == comparator) throw ConstraintViolationException(L"Equal");
		break;
	case mule::Data::Basic::BasicType::BTCT_NLT:
		if (value < comparator) throw ConstraintViolationException(L"Less than");
		break;
	case mule::Data::Basic::BasicType::BTCT_NGT:
		if (value > comparator) throw ConstraintViolationException(L"Grater than");
		break;
	default:
		throw xybase::InvalidOperationException(L"Invalid constraint type.", 0x8F52);
		break;
	}
}
