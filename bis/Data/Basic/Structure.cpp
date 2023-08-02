#include "Structure.h"

using namespace mule::Data;
using namespace mule::Data::Basic;

Structure::Structure(std::string name) : name(name)
{
}

mule::Data::Basic::Structure::~Structure()
{
	for (Field *field : fields)
	{
		delete field;
	}
}

void Structure::AppendField(const std::string &name, Object * field)
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

void Structure::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	for (Field *field : fields)
	{
		field->Write(stream, dataHandler);
	}
}

size_t Structure::Size() const
{
	size_t size = 0;
	for (Object *obj : fields)
	{
		size += obj->Size();
	}
	return size;
}

std::string mule::Data::Basic::Structure::GetName() const
{
	return name;
}

std::string mule::Data::Basic::Structure::GetTypeName() const
{
	return "structure";
}

void mule::Data::Basic::Structure::Field::Read(xybase::Stream *stream, DataHandler *dataHandler)
{
	dataHandler->OnRealmEnter(object, name);
	object->Read(stream, dataHandler);
	dataHandler->OnRealmExit(object, name);
}

void mule::Data::Basic::Structure::Field::Write(xybase::Stream *stream, DataHandler *dataHandler)
{
	dataHandler->OnRealmEnter(object, name);
	object->Write(stream, dataHandler);
	dataHandler->OnRealmExit(object, name);
}

size_t mule::Data::Basic::Structure::Field::Size() const
{
	return object->Size();
}

const std::string &mule::Data::Basic::Structure::Field::GetName() const
{
	return name;
}

const Object *const mule::Data::Basic::Structure::Field::GetObject() const
{
	return object;
}

mule::Data::Basic::Structure::Field::Field(std::string name, Object *obj)
	: name(name), object(obj)
{
}

std::string mule::Data::Basic::Structure::Field::GetTypeName() const
{
	return std::string("field");
}
