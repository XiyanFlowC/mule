#include "JsonOutHandler.h"

#include <xystring.h>

using namespace mule::Data::Basic;

void mule::Json::JsonOutHandler::OnRealmEnter(Type *realm, const std::u16string &name)
{
	outstream->Write('"');
	outstream->Write((char *)xybase::string::to_utf8(name).c_str());
	outstream->Write("\": ");
	if (realm->IsComposite())
	{
		if (realm->GetDataType().starts_with(u"array/"))
		{
			outstream->Write("[");
		}
		else
		{
			outstream->Write("{");
		}
	}
}

void mule::Json::JsonOutHandler::OnRealmExit(Type *realm, const std::u16string &name)
{
	if (realm->IsComposite())
	{
		if (realm->GetDataType().starts_with(u"array/"))
		{
			outstream->Write("]");
		}
		else
		{
			outstream->Write("}");
		}
	}
	outstream->Write(", ");
}

void mule::Json::JsonOutHandler::OnRealmEnter(Type *realm, int idx)
{
	if (realm->IsComposite())
	{
		if (realm->GetDataType().starts_with(u"array/"))
		{
			outstream->Write("[");
		}
		else
		{
			outstream->Write("{");
		}
	}
}

void mule::Json::JsonOutHandler::OnRealmExit(Type *realm, int idx)
{
	if (realm->IsComposite())
	{
		if (realm->GetDataType().starts_with(u"array/"))
		{
			outstream->Write("]");
		}
		else
		{
			outstream->Write("}");
		}
	}
	outstream->Write(", ");
}

void mule::Json::JsonOutHandler::OnSheetReadStart()
{
}

void mule::Json::JsonOutHandler::OnSheetReadEnd()
{
}

void mule::Json::JsonOutHandler::OnDataRead(const mule::Data::Basic::MultiValue &value)
{
}
