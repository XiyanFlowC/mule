#include "DoubleField.h"

using namespace mule::Data::Basic;

MultiValue mule::Data::Basic::DoubleField::DoRead(FieldReadWriteContext &context)
{
	return MultiValue(context.stream->ReadDouble());
}

int mule::Data::Basic::DoubleField::DoWrite(FieldReadWriteContext &context, const MultiValue &value) const
{
	context.stream->Write(value.value.realValue);
	return 0;
}
