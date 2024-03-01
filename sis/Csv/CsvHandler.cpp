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
		wrapLayer = (int)Configuration::GetInstance().GetSigned(u"mule.handler.wrap-layer");
	if (Configuration::GetInstance().IsExist(u"mule.handler.wrap-suppression"))
		wrapSuppression = (int)Configuration::GetInstance().GetSigned(u"mule.handler.wrap-suppression");
	outstream->Write("\xEF\xBB\xBF"); // 让Excel高兴
	disarmed = true;
	isString = false;
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
		layer++;
	else
		isString = realm->GetDataType().starts_with(u"string/"), isText = realm->GetDataType() == u"string/text";
	disarmed = realm->IsComposite();
}

void mule::Csv::CsvOutHandler::OnRealmExit(Type *realm, const std::u16string &name)
{
	if (realm->IsComposite())
	{
		--layer;
	}
	if (layer <= wrapLayer)
	{
		if (status == CHS_READ_TRAILCELL && (realm->IsComposite() ? wrapSuppression != 2 : wrapSuppression != 1))
		{
			status = CHS_READ;
			outstream->Write("\n");
		}
	}
}

void mule::Csv::CsvOutHandler::OnRealmEnter(Type *realm, int idx)
{
	if (realm->IsComposite())
		layer++;
	else
		isString = realm->GetDataType().starts_with(u"string"), isText = realm->GetDataType() == u"string/text";
	disarmed = realm->IsComposite();
}

void mule::Csv::CsvOutHandler::OnRealmExit(Type *realm, int idx)
{
	if (realm->IsComposite())
	{
		--layer;
	}
	if (layer <= wrapLayer)
	{
		if (status == CHS_READ_TRAILCELL && (realm->IsComposite() ? wrapSuppression != 2 : wrapSuppression != 1))
		{
			status = CHS_READ;
			outstream->Write("\n");
		}
	}
}

void mule::Csv::CsvOutHandler::OnDataRead(const MultiValue &value)
{
	if (disarmed) return;

	if (status == CHS_READ)
		status = CHS_READ_TRAILCELL;
	else if (status == CHS_READ_TRAILCELL)
	{
		outstream->Write(",");
	}
	if (isString && value.IsType(MultiValue::MVT_STRING))
	{
		auto str = *value.value.stringValue;
		if (Configuration::GetInstance().IsExist(u"mule.handler.string-read-proc"))
		{
			auto name = Configuration::GetInstance().GetString(u"mule.handler.string-read-proc");
			str = *mule::Lua::LuaHost::GetInstance().Call(xybase::string::to_string(name), 1, &value).value.stringValue;
		}
		/*if (str.find('\n') == std::u16string::npos && str.find('"') == std::u16string::npos)
			outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(str).c_str()));
		else
		{*/
			outstream->Write('"');
			outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(xybase::string::replace<char16_t>(str, u"\"", u"\"\"")).c_str()));
			outstream->Write('"');
		//}
	}
	else
		outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(value.Stringfy()).c_str()));
}
