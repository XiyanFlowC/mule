#include "XmlHandler.h"

using namespace mule::Data::Basic;

void mule::Xml::XmlHandler::OnRealmEnter(Object *realm, std::string name)
{
}

void mule::Xml::XmlHandler::OnRealmExit(Object *realm, std::string name)
{
}

void mule::Xml::XmlHandler::OnRealmEnter(Object *realm, int idx)
{
}

void mule::Xml::XmlHandler::OnRealmExit(Object *realm, int idx)
{
}

void mule::Xml::XmlHandler::OnDataRead(const MultiValue &value)
{
}

MultiValue mule::Xml::XmlHandler::OnDataWrite()
{
	return MultiValue();
}
