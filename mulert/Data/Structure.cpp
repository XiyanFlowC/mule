#include "Structure.h"

#include "Basic/BasicType.h"

using namespace mule::Data;
using namespace mule::Data::Basic;

Structure::Structure(std::u16string name) : name(name)
{
}

mule::Data::Structure::~Structure()
{
	for (Field *field : fields)
	{
		delete field;
	}
}

void Structure::AppendField(const std::u16string &name, Type * field)
{
	fields.push_back(new Field(name, field));
}

void Structure::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	for (Field *field : fields)
	{
		field->Read(stream, dataHandler);
	}
}

void Structure::Write(xybase::Stream *stream, FileHandler * fileHandler)
{
	for (Field *field : fields)
	{
		field->Write(stream, fileHandler);
	}
}

size_t Structure::Size() const
{
	size_t size = 0;
	for (Type *obj : fields)
	{
		if (obj->Size() == -1)
			return -1;
		size += obj->Size();
	}
	return size;
}

std::u16string mule::Data::Structure::GetName() const
{
	return name;
}

std::u16string mule::Data::Structure::GetDataType() const
{
	return name;
}

bool mule::Data::Structure::IsComposite() const
{
	return true;
}

void mule::Data::Structure::Field::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	dataHandler->OnRealmEnter(object, name);
	try
	{
		object->Read(stream, dataHandler);
	}
	catch (BasicType::ConstraintViolationException &ex)
	{
		dataHandler->OnRealmExit(object, name);
		throw BasicType::ConstraintViolationException(ex.GetMessage());
	}
	dataHandler->OnRealmExit(object, name);
}

void mule::Data::Structure::Field::Write(xybase::Stream *stream, FileHandler * fileHandler)
{
	fileHandler->OnRealmEnter(object, name);
	try
	{
		object->Write(stream, fileHandler);
	}
	catch (BasicType::ConstraintViolationException &ex)
	{
		fileHandler->OnRealmExit(object, name);
		throw BasicType::ConstraintViolationException(ex.GetMessage());
	}
	fileHandler->OnRealmExit(object, name);
}

size_t mule::Data::Structure::Field::Size() const
{
	return object->Size();
}

const std::u16string &mule::Data::Structure::Field::GetName() const
{
	return name;
}

const Type *const mule::Data::Structure::Field::GetObject() const
{
	return object;
}

mule::Data::Structure::Field::Field(std::u16string name, Type *obj)
	: name(name), object(obj)
{
}

mule::Data::Structure::Field::~Field()
{
}

std::u16string mule::Data::Structure::Field::GetDataType() const
{
	return u"field/" + object->GetDataType();
}
