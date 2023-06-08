#include "ArrayField.h"

using namespace mule::Data;
using namespace mule::Exception;

Basic::Field *ArrayField::ArrayFieldCreator::DoCreateField(FieldCreatingInfo &info)
{
	if (!info.typeDescriptor.starts_with("array:")) return nullptr;

	std::string objectName = info.typeDescriptor.substr(6);
	Object *object = ObjectManager::GetInstance().GetObject(objectName);
	if (object == nullptr)
		throw InvalidParameterException("info.typeDescriptor:[objectName]",
			std::string("Unable to find specified object by name: ") + objectName,
			__FILE__,
			__LINE__);

	return new ArrayField(object, info.width);
}

ArrayField::ArrayField(Object *innerObject, int length)
{
	this->innerObject = innerObject;
	this->length = length;
}

size_t ArrayField::FieldSize() const
{
	return length * innerObject->Size();
}

mule::Data::Basic::MultiValue ArrayField::DoRead(FieldReadWriteContext &context)
{
	xybase::StringBuilder sb;
	sb += '[';
	for (int i = 0; i < length; ++i) {
		innerObject->Read(context.stream, context.handler);
		sb += ',';
	}
	sb += ']';
	return mule::Data::Basic::MultiValue(sb.ToString());
}

int ArrayField::DoWrite(FieldReadWriteContext &context, const mule::Data::Basic::MultiValue &value) const
{
	return 0;
}
