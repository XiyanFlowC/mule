#include "XmlHandler.h"

#include <xystring.h>

using namespace mule::Data::Basic;

void mule::Xml::XmlHandler::OnSheetReadStart()
{
}

void mule::Xml::XmlHandler::OnSheetReadEnd()
{
}

void mule::Xml::XmlHandler::OnSheetWriteStart()
{
}

void mule::Xml::XmlHandler::OnSheetWriteEnd()
{
}

void mule::Xml::XmlHandler::OnRealmEnter(Type *realm, const std::u16string& name)
{
}

void mule::Xml::XmlHandler::OnRealmExit(Type *realm, const std::u16string& name)
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

void mule::Xml::XmlHandler::SetStream(xybase::Stream *stream)
{
	this->stream = stream;
}
