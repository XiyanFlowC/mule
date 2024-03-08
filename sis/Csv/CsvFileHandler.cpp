#include "CsvFileHandler.h"

#include <Configuration.h>
#include "../Lua/LuaHost.h"

using namespace mule::Data::Basic;

int mule::Csv::CsvFileHandler::ReadChar()
{
	char ch;
	int ret = u'�';
	int leng;
	switch (charMode)
	{
	case mule::Csv::CsvFileHandler::UCM_UTF8:
		ch = instream->ReadChar();
		if (ch & 0x80) {
			if ((ch & 0xE0) == 0xC0) {
				leng = 2;
				ret = ch & 0x1F;
			}
			else if ((ch & 0xF0) == 0xE0) {
				leng = 3;
				ret = ch & 0x0F;
			}
			else if ((ch & 0xF8) == 0xF0) {
				leng = 4;
				ret = ch & 0x07;
			}
			else {
				// Invalid UTF-8 encoding
				ret = u'�';
				break;
			}

			for (int j = 1; j < leng; ++j) {
				if (instream->IsEof())
				{
					ret = u'�';
					break;
				}
				ch = instream->ReadChar();
				if ((ch & 0xC0) == 0x80) {
					ret = (ret << 6) | (ch & 0x3F);
				}
				else {
					// Invalid UTF-8 encoding
					ret = u'�';
					break;
				}
			}
		}
		else ret = ch;
		break;
	case mule::Csv::CsvFileHandler::UCM_UTF16LE:
		ret = instream->ReadChar();
		ret |= instream->ReadChar() << 8;
		if (ret >= 0xD800 && ret <= 0xDBFF) {
			if (instream->IsEof())
			{
				ret = u'�';
				break;
			}

			// Surrogate pair for characters U+10000 to U+10FFFF
			char16_t leadSurrogate = ret;
			char16_t trailSurrogate = instream->ReadChar();
			trailSurrogate |= instream->ReadChar() << 8;

			// Singular lead surrogate
			if ((trailSurrogate & 0xFC00) != 0xDC00)
			{
				ret = u'�';
				instream->Seek(-2, xybase::Stream::SM_CURRENT);
				break;
			}

			ret = static_cast<char32_t>(0x10000 + ((leadSurrogate & 0x3FF) << 10) + (trailSurrogate & 0x3FF));
		}
		else {
			// Singular trail surrogate
			if ((ret & 0xFC00) == 0xDC00)
			{
				ret = u'�';
			}
		}

		// CRLF handle
		if (ret == '\r')
		{
			char16_t linefeedTest = instream->ReadChar();
			linefeedTest |= instream->ReadChar() << 8;
			if (linefeedTest == '\n')
				ret = '\r';
			else
				instream->Seek(-2, xybase::Stream::SM_CURRENT);
		}
		break;
	case mule::Csv::CsvFileHandler::UCM_UTF16BE:
		ret = instream->ReadChar() << 8;
		ret |= instream->ReadChar();
		if (ret >= 0xD800 && ret <= 0xDBFF) {
			if (instream->IsEof())
			{
				ret = u'�';
				break;
			}
			// Surrogate pair for characters U+10000 to U+10FFFF
			char16_t leadSurrogate = ret;
			char16_t trailSurrogate = instream->ReadChar() << 8;
			trailSurrogate |= instream->ReadChar();

			// Singular lead surrogate
			if ((trailSurrogate & 0xFC00) != 0xDC00)
			{
				ret = u'�';
				instream->Seek(-2, xybase::Stream::SM_CURRENT);
				break;
			}

			ret = static_cast<char32_t>(0x10000 + ((leadSurrogate & 0x3FF) << 10) + (trailSurrogate & 0x3FF));
		}
		else {
			// Singular trail surrogate
			if ((ret & 0xFC00) == 0xDC00)
			{
				ret = u'�';
			}
		}

		// CRLF handle
		if (ret == '\r')
		{
			char16_t linefeedTest = instream->ReadChar() << 8;
			linefeedTest |= instream->ReadChar();
			if (linefeedTest == '\n')
				ret = '\r';
			else
				instream->Seek(-2, xybase::Stream::SM_CURRENT);
		}
		break;
	case mule::Csv::CsvFileHandler::UCM_UTF32LE:
	case mule::Csv::CsvFileHandler::UCM_UTF32BE:
	default:
		break;
	}
	return ret;
}

std::u16string mule::Csv::CsvFileHandler::ReadCell()
{
	int ch = ReadChar();
	xybase::StringBuilder<char16_t> sb;
	if (ch == '"')
	{
		while (true)
		{
			ch = ReadChar();
			if (ch == '"')
			{
				ch = ReadChar();
				if (ch == '"')
				{
					sb += '"';
				}
				else
				{
					while (ch != ',' && ch != '\n')
					{
						ch = ReadChar();
					}
					flg_eol = ch == '\n';
					return sb.ToString();
				}
			}
			else
			{
				if (ch <= 0xFFFF) {
					sb += ch;
				}
				else if (ch <= 0x10FFFF) {
					ch -= 0x10000;
					char16_t leadSurrogate = static_cast<char16_t>((ch >> 10) + 0xD800);
					char16_t trailSurrogate = static_cast<char16_t>((ch & 0x3FF) + 0xDC00);
					sb.Append(leadSurrogate);
					sb.Append(trailSurrogate);
				}
			}
		}
	}
	else
	{
		while (ch != ',' && ch != '\n')
		{
			sb += ch;
			ch = ReadChar();
		}
		flg_eol = ch == '\n';
	}
	return sb.ToString();
}

void mule::Csv::CsvFileHandler::OnRealmEnter(Type *realm, const std::u16string &name)
{
	HandleRealmEnter(realm);
}

void mule::Csv::CsvFileHandler::OnRealmExit(Type *realm, const std::u16string &name)
{
	HandleReamExit(realm);
}

void mule::Csv::CsvFileHandler::HandleReamExit(mule::Data::Basic::Type *realm)
{
	if (realm->IsComposite())
	{
		--layer;
	}
	if (layer <= wrapLayer)
	{
		if (!flg_eol && (realm->IsComposite() ? wrapSuppression != 2 : wrapSuppression != 1))
		{
			logger.Debug(L"Weird file structure. Sync to End-Of-Line.");
			flg_eol = true;
			int ch = ReadChar();
			while (ch != '\n')
			{
				ch = ReadChar();
#ifndef NDEBUG
				if (ch != ',')
				{
					logger.Debug(L"File suspicious. Corrupted?");
				}
#endif // !NDEBUG
			}
		}
	}
}

void mule::Csv::CsvFileHandler::OnRealmEnter(Type *realm, int idx)
{
	HandleRealmEnter(realm);
}

void mule::Csv::CsvFileHandler::HandleRealmEnter(mule::Data::Basic::Type *realm)
{
	if (realm->IsComposite())
	{
		++layer;
		return;
	}
	auto str = ReadCell();
	if (str == u"null")
		readElement = MultiValue::MVT_NULL;
	else if (realm->GetDataType().starts_with(u"string/"))
	{
		if (realm->GetDataType() == u"string/text" && Configuration::GetInstance().IsExist(u"mule.handler.string-write-proc"))
		{
			auto name = Configuration::GetInstance().GetString(u"mule.handler.string-write-proc");
			MultiValue tmp{ str };
			str = *mule::Lua::LuaHost::GetInstance().Call(xybase::string::to_string(name), 1, &tmp).value.stringValue;
		}
		readElement = MultiValue{ str };
	}
	else if (realm->GetDataType().starts_with(u"real-number/"))
	{
		readElement = std::stod(xybase::string::to_wstring(str));
	}
	else readElement = MultiValue::Parse(str);
}

void mule::Csv::CsvFileHandler::OnRealmExit(Type *realm, int idx)
{
	HandleReamExit(realm);
}

void mule::Csv::CsvFileHandler::OnSheetWriteStart()
{
	char bom[4];
	for (int i = 0; i < 2; ++i)
	{
		bom[i] = instream->ReadChar();
	}
	// utf-16 be
	if (memcmp(bom, "\xFF\xFE", 2) == 0)
	{
		charMode = UCM_UTF16BE;
	}
	// utf-16 le
	else if (memcmp(bom, "\xFE\xFF", 2) == 0)
	{
		charMode = UCM_UTF16LE;
	}
	
	bom[2] = instream->ReadChar();
	// if not utf-8 with BOM
	if (memcmp(bom, "\xEF\xBB\xBF", 3))
	{
		// rewind, assume it is utf-8
		instream->Seek(0, xybase::Stream::SM_BEGIN);
	}

	// 初始化换行检查系统
	flg_eol = false;
	layer = 0;
	if (Configuration::GetInstance().IsExist(u"mule.handler.wrap-layer"))
		wrapLayer = (int)Configuration::GetInstance().GetSigned(u"mule.handler.wrap-layer");
	if (Configuration::GetInstance().IsExist(u"mule.handler.wrap-suppression"))
		wrapSuppression = (int)Configuration::GetInstance().GetSigned(u"mule.handler.wrap-suppression");
}

void mule::Csv::CsvFileHandler::OnSheetWriteEnd()
{
}

const mule::Data::Basic::MultiValue mule::Csv::CsvFileHandler::OnDataWrite()
{
	return readElement;
}
