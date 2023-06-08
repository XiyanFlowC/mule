#include "Field.h"

using namespace mule::Data::Basic;
using namespace mule::Exception;

int Field::Write(FieldReadWriteContext& context) const
{
	int ret = DoWrite(context, context.handler->BeforeCellWrite());
	if (ret) return ret;
	if (next == nullptr) return 0;

	next->Write(context);
}

size_t Field::FieldSize() const
{
	return 0;
}

void Field::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	if (next != nullptr) throw InvalidOperationException("A field in chain cannot read as a object!", __FILE__, __LINE__);

	FieldReadWriteContext context(stream, dataHandler);
	int ret = 0;
	if (ret = Read(context)) throw Exception::Exception(std::string("Unknown error from read chain, code: ") + std::to_string(ret), __FILE__, __LINE__);
}

void Field::Write(xybase::Stream *stream, DataHandler *dataHandler) const
{
	if (next != nullptr) throw InvalidOperationException("A field in chain cannot write as a object!", __FILE__, __LINE__);

	FieldReadWriteContext context(stream, dataHandler);
	int ret = 0;
	if (ret = Write(context)) throw Exception::Exception(std::string("Unknown error from write chain, code: ") + std::to_string(ret), __FILE__, __LINE__);
}

size_t Field::Size() const
{
	if (next != nullptr) return next->Size() + FieldSize();
	
	return FieldSize();
}

Field::Field()
{
	next = nullptr;
}

Field::~Field()
{
	if (next != nullptr) delete next;
}

int Field::Read(FieldReadWriteContext& context)
{
	context.handler->AfterCellRead(DoRead(context));
	if (next == nullptr) return 0;

	next->Read(context);
}

Field::FieldReadWriteContext::FieldReadWriteContext(xybase::Stream *stream, DataHandler *dataHandler)
{
	imcomplete = false;
	context = nullptr;
	this->stream = stream;
	this->handler = dataHandler;
}

Field::FieldReadWriteContext::FieldReadWriteContext(FieldReadWriteContext& pattern)
{
	imcomplete = pattern.imcomplete;
	context = pattern.context;
	stream = pattern.stream;
	handler = pattern.handler;
}
