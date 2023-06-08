#include "FloatField.h"

using namespace mule::Data::Basic;

MultiValue mule::Data::Basic::FloatField::DoRead(FieldReadWriteContext &context)
{
	return MultiValue(context.stream->ReadFloat());
}

int mule::Data::Basic::FloatField::DoWrite(FieldReadWriteContext &context, const MultiValue &value) const
{
	context.stream->Write(value.value.realValue);
	return 0;
}
