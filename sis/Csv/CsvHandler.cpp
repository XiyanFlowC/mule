#include "CsvHandler.h"

#include <xystring.h>
#include <Configuration.h>
#include "../Lua/LuaHost.h"

using namespace mule::Data::Basic;

void mule::Csv::CsvOutHandler::OnSheetReadStart()
{
	if (status != CHS_IDLE)
		throw xybase::InvalidOperationException(L"This handler is not idle for read.", 10000);
	status = CHS_READ;
	if (Configuration::GetInstance().IsExist(u"mule.handler.wrap-layer"))
		wrapLayer = Configuration::GetInstance().GetSigned(u"mule.handler.wrap-layer");
	outstream->Write("\xEF\xBB\xBF"); // 让Excel高兴
}

void mule::Csv::CsvOutHandler::OnSheetReadEnd()
{
	if (status != CHS_READ)
		throw xybase::InvalidOperationException(L"This handler is not reading.", 10000);
	status = CHS_IDLE;
}

void mule::Csv::CsvOutHandler::OnRealmEnter(Type *realm, const std::u16string &name)
{
	if (realm->IsComposite())
	{
		++layer;
	}
}

void mule::Csv::CsvOutHandler::OnRealmExit(Type *realm, const std::u16string &name)
{
	if (realm->IsComposite())
	{
		--layer;
	}
	if (layer <= wrapLayer)
	{
		if (status == CHS_READ_TRAILCELL)
		{
			status = CHS_READ;
			outstream->Write("\n");
		}
	}
}

void mule::Csv::CsvOutHandler::OnRealmEnter(Type *realm, int idx)
{
	if (realm->IsComposite())
	{
		++layer;
	}
}

void mule::Csv::CsvOutHandler::OnRealmExit(Type *realm, int idx)
{
	if (realm->IsComposite())
	{
		--layer;
	}
	if (layer <= wrapLayer)
	{
		if (status == CHS_READ_TRAILCELL)
		{
			status = CHS_READ;
			outstream->Write("\n");
		}
	}
}

void mule::Csv::CsvOutHandler::OnDataRead(const MultiValue &value)
{
	if (status == CHS_READ)
		status = CHS_READ_TRAILCELL;
	else if (status == CHS_READ_TRAILCELL)
	{
		outstream->Write(",");
	}
	if (value.IsType(MultiValue::MVT_STRING))
	{
		auto str = *value.value.stringValue;
		if (Configuration::GetInstance().IsExist(u"mule.handler.string-read-proc"))
		{
			auto name = Configuration::GetInstance().GetString(u"mule.handler.string-read-proc");
			str = *mule::Lua::LuaHost::GetInstance().Call(xybase::string::to_string(name), 1, &value).value.stringValue;
		}
		outstream->Write('"');
		outstream->Write(reinterpret_cast<const char*>(xybase::string::to_utf8(xybase::string::replace<char16_t>(str, u"\"", u"\"\"")).c_str()));
		outstream->Write('"');
	}
	else
		outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(value.Stringfy()).c_str()));
}
