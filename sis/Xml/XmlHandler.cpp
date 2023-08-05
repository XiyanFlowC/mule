#include "XmlHandler.h"

using namespace mule::Data::Basic;

void mule::Xml::XmlHandler::OnRealmEnter(Type *realm, std::string name)
{
}

void mule::Xml::XmlHandler::OnRealmExit(Type *realm, std::string name)
{
}

void mule::Xml::XmlHandler::OnRealmEnter(Type *realm, int idx)
{
}

void mule::Xml::XmlHandler::OnRealmExit(Type *realm, int idx)
{
}

void mule::Xml::XmlHandler::OnDataRead(const MultiValue &value)
{
}

MultiValue mule::Xml::XmlHandler::OnDataWrite()
{
	return MultiValue();
}
