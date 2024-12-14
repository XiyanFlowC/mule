#include "XmlHandler.h"

#include <xystring.h>
#include <Configuration.h>
#include <Exception/IOException.h>
#include <Exception/InvalidOperationException.h>
#include <Exception/InvalidParameterException.h>
#include "../Lua/LuaHost.h"

using namespace mule::Data::Basic;

int mule::Xml::XmlHandler::ident = 1;
int mule::Xml::XmlHandler::type = 0;

mule::Xml::XmlHandler::XmlHandler()
{
	status = XHS_IDLE;
}

void mule::Xml::XmlHandler::OnSheetReadStart()
{
	if (status != XHS_IDLE) throw xybase::InvalidOperationException(L"Cannot read whilst this handler is busy.", 3501);
	status = XHS_READ;
}

void mule::Xml::XmlHandler::OnSheetReadEnd()
{
	if (status != XHS_READ) throw xybase::InvalidOperationException(L"Cannot stop read whilst this handler is not read.", 3502);
	status = XHS_IDLE;
}

void mule::Xml::XmlHandler::OnSheetWriteStart()
{
	if (status != XHS_IDLE) throw xybase::InvalidOperationException(L"Cannot write whilst this handler is busy.", 3503);
	status = XHS_WRITE;
}

void mule::Xml::XmlHandler::OnSheetWriteEnd()
{
	if (status != XHS_WRITE) throw xybase::InvalidOperationException(L"Cannot stop write whilst this handler is not write.", 3504);
	status = XHS_IDLE;
}

void mule::Xml::XmlHandler::OnRealmEnter(Type *realm, const std::u16string& rawName)
{
	std::u16string name = rawName;
	auto pos = name.find('/');
	while (pos != std::u16string::npos)
	{
		name.replace(pos, 1, u"-");
		pos = name.find('/');
	}
	if (status == XHS_READ_METADATA_WAITING)
	{
		status = XHS_READ;
		outstream->Write(">");

		outstream->Write("\n");
		layer++;
	}

	if (status == XHS_READ)
	{
		for (int i = 0; i < layer; ++i)
			for (int j = 0; j < ident; ++j)
			{
				outstream->Write(type ? " " : "\t");
			}

		if (realm->IsComposite())
		{
			outstream->Write("<");
			outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(name).c_str()));

			status = XHS_READ_METADATA_WAITING;
		}
		else nodeName = name;
	}
	else if (status == XHS_WRITE)
	{
		xybase::StringBuilder<char8_t> sb;
		
		char ch;
		bool againFlag;
		do
		{
			againFlag = false;
			ch = instream->ReadChar();
			while (ch != '<') ch = instream->ReadChar();

			// 获取标签名
			ch = instream->ReadChar();
			if (ch == '!' || ch == '?')
			{
				while (ch != '>') ch = instream->ReadChar();
				againFlag = true;
			}
		} while (againFlag);

		while (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n' && ch != '>')
		{
			sb += ch;
			ch = instream->ReadChar();
		}
		
		std::u8string tag = sb.ToString();
		sb += ch;

		if (xybase::string::to_utf16(tag) != name)
			throw xybase::RuntimeException(L"Format error, expect " + xybase::string::to_wstring(name) + L", but got " + xybase::string::to_wstring(tag), 9002);

		// 组合类型内部仍为元素，找到结束点同步即可
		if (realm->IsComposite())
		{
			element.SetType(mule::Data::Basic::MultiValue::MVT_NULL);
			element.metadata.clear();
			while (ch != '>')
			{
				if (ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t')
				{
					xybase::StringBuilder sb;
					while (ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t' && ch != '=')
					{
						sb.Append(ch);
						ch = instream->ReadChar();
					}
					std::string key = sb.ToString();
					while (ch != '=')
					{
						ch = instream->ReadChar();
					}
					while (ch != '\'' && ch != '"')
					{
						ch = instream->ReadChar();
					}
					char endCh = ch;
					sb.Clear();
					ch = instream->ReadChar();
					while (ch != endCh) {
						sb.Append(ch);
						ch = instream->ReadChar();
					}
					element.metadata[xybase::string::to_utf16(key)] = MultiValue::Parse(xybase::string::to_utf16(sb.ToString()));
				}
				ch = instream->ReadChar();
			}
			return;
		}

		// 非组合类型须解析内部元素
		ReadTagAndParse(tag, sb, realm->GetDataType().starts_with(u"string/"), realm->GetDataType() == u"string/text");
	}
}

void mule::Xml::XmlHandler::ReadTagAndParse(const std::u8string &tagName, xybase::StringBuilder<char8_t> &sb, bool isString, bool isText)
{
	std::u8string endTag = u8"</" + tagName + u8">";

	bool repeat = true;
	size_t cmp = 0;
	while (repeat)
	{
		char ch = instream->ReadChar();
		if (ch == endTag[cmp]) cmp++;
		else cmp = 0;
		if (cmp == endTag.size()) repeat = false;
		sb.Append(ch);
	}
	auto root = xmlParser.Parse(xybase::string::to_utf8("<") + sb.ToString());
	
	std::u16string text {};
	if (root.children.empty()) return;
	for (auto &&child : root.children)
	{
		if (child.IsTextNode()) text.append(child.GetText());
	}

	if (text == u"null")
		element = MultiValue::MV_NULL;
	else
		if (isString)
		{
			if (isText && Configuration::GetInstance().IsExist(u"mule.handler.string-write-proc"))
			{
				auto name = Configuration::GetInstance().GetString(u"mule.handler.string-write-proc");
				MultiValue tmp{ text };
				element = mule::Lua::LuaHost::GetInstance().Call(xybase::string::to_string(name), 1, &tmp);
			}
			else element = MultiValue{ text };
		}
		else
			element = MultiValue::Parse(text);
	for (auto &&attr : root.attributes)
	{
		element.metadata[attr.first] = MultiValue::Parse(attr.second);
	}
}

void mule::Xml::XmlHandler::OnRealmExit(Type *realm, const std::u16string& rawName)
{
	std::u16string name = rawName;
	auto pos = name.find('/');
	while (pos != std::u16string::npos)
	{
		name.replace(pos, 1, u"-");
		pos = name.find('/');
	}
	if (status == XHS_READ_METADATA_WAITING)
	{
		status = XHS_READ;
		outstream->Write(">");

		outstream->Write("\n");
		layer++;
	}

	if (status == XHS_READ)
	{
		if (realm->IsComposite())
		{
			layer--;
			for (int i = 0; i < layer; ++i)
				for (int j = 0; j < ident; ++j)
				{
					outstream->Write(type ? " " : "\t");
				}
		}
		else
		{
			outstream->Write("<");
			outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(nodeName).c_str()));
			for (auto &&datum : element.metadata)
			{
				outstream->Write(" ");
				outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(datum.first).c_str()));
				outstream->Write("='");
				outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(datum.second.Stringfy()).c_str()));
				outstream->Write("'");
			}
			outstream->Write(">");
			if (element.IsType(MultiValue::MVT_STRING) && realm->GetDataType().starts_with(u"string/"))
			{
				auto str = *element.value.stringValue;
				if (realm->GetDataType() == u"string/text" && Configuration::GetInstance().IsExist(u"mule.handler.string-read-proc"))
				{
					auto name = Configuration::GetInstance().GetString(u"mule.handler.string-read-proc");
					str = *mule::Lua::LuaHost::GetInstance().Call(xybase::string::to_string(name), 1, &element).value.stringValue;
				}

				outstream->Write("<![CDATA[");
				outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(str).c_str()));
				outstream->Write("]]>");
			}
			else
				outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(element.Stringfy()).c_str()));
		}

		outstream->Write("</");
		outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(name).c_str()));
		outstream->Write(">\n");
	}
	else
	{
		int c;
		// 非组合类型已经在Enter时完成读入和解析，这里只需要找到结束点同步即可
		if (realm->IsComposite())
		{
			do
			{
				c = instream->ReadChar();
			} while (c != '<');
			c = instream->ReadChar();
			xybase::StringBuilder<char8_t> sb;
			if (c != '/')
			{
				while (c != '>')
				{
					sb += c;
					c = instream->ReadChar();
				}
				throw xybase::RuntimeException(
					std::format(
						L"Unexpected open tag. expected </{}>, but got <{}>.",
						xybase::string::to_wstring(name),
						xybase::string::to_wstring(sb.ToString())),
					95071);
			}
			do
			{
				c = instream->ReadChar();
				sb += c;
			} while (c != '>');
			if (xybase::string::to_utf16(sb.ToString()) != name + u">")
			{
				throw xybase::RuntimeException(std::format(L"Unexpected close tag, expected </{}>, but got </{}.", xybase::string::to_wstring(name), xybase::string::to_wstring(sb.ToString())), 95073);
			}
		}
	}
}

void mule::Xml::XmlHandler::OnRealmEnter(Type *realm, int idx)
{
	if (status == XHS_READ_METADATA_WAITING)
	{
		status = XHS_READ;
		outstream->Write(">");

		outstream->Write("\n");
		layer++;
	}

	if (status == XHS_READ)
	{
		for (int i = 0; i < layer; ++i)
			for (int j = 0; j < ident; ++j)
			{
				outstream->Write(type ? " " : "\t");
			}

		if (realm->IsComposite())
		{
			outstream->Write("<");
			outstream->Write(reinterpret_cast<const char *>("item i='"));
			outstream->Write((int64_t)idx);
			outstream->Write("'");

			status = XHS_READ_METADATA_WAITING;
		}
		else nodeName = u"item";
	}
	else if (status == XHS_WRITE)
	{
		xybase::StringBuilder<char8_t> sb;

		char ch;
		bool againFlag;
		do
		{
			againFlag = false;
			ch = instream->ReadChar();
			while (ch != '<') ch = instream->ReadChar();

			// 检查是否是注释或者签名
			ch = instream->ReadChar();
			if (ch == '!' || ch == '?')
			{
				while (ch != '>') ch = instream->ReadChar();
				againFlag = true;
			}
		} while (againFlag);

		// 读取，构建标签名
		while (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n' && ch != '>')
		{
			sb += ch;
			ch = instream->ReadChar();
		}

		std::u8string tag = sb.ToString();
		sb += ch;

		if (xybase::string::to_utf16(tag) != u"item")
			throw xybase::RuntimeException(L"Format error, expect " L"item" L", but got " + xybase::string::to_wstring(tag), 9002);

		// 组合类型内部仍为元素，找到结束点同步即可
		if (realm->IsComposite())
		{
			element.SetType(mule::Data::Basic::MultiValue::MVT_NULL);
			element.metadata.clear();
			while (ch != '>')
			{
				if (ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t')
				{
					xybase::StringBuilder sb;
					while (ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t' && ch != '=')
					{
						sb.Append(ch);
						ch = instream->ReadChar();
					}
					std::string key = sb.ToString();
					while (ch != '=')
					{
						ch = instream->ReadChar();
					}
					while (ch != '\'' && ch != '"')
					{
						ch = instream->ReadChar();
					}
					char endCh = ch;
					sb.Clear();
					ch = instream->ReadChar();
					while (ch != endCh) {
						sb.Append(ch);
						ch = instream->ReadChar();
					}
					element.metadata[xybase::string::to_utf16(key)] = MultiValue::Parse(xybase::string::to_utf16(sb.ToString()));
				}
				ch = instream->ReadChar();
			}
			return;
		}

		// 非组合类型须解析内部元素
		ReadTagAndParse(tag, sb, realm->GetDataType().starts_with(u"string/"), realm->GetDataType() == u"string/text");
	}
}

void mule::Xml::XmlHandler::OnRealmExit(Type *realm, int idx)
{
	if (status == XHS_READ_METADATA_WAITING)
	{
		status = XHS_READ;
		outstream->Write(">");

		outstream->Write("\n");
		layer++;
	}

	if (status == XHS_READ)
	{
		if (realm->IsComposite())
		{
			layer--;
			for (int i = 0; i < layer; ++i)
				for (int j = 0; j < ident; ++j)
				{
					outstream->Write(type ? " " : "\t");
				}
		}
		else
		{
			outstream->Write("<");
			outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(nodeName).c_str()));
			element.metadata[u"i"] = idx;
			for (auto &&datum : element.metadata)
			{
				outstream->Write(" ");
				outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(datum.first).c_str()));
				outstream->Write("='");
				outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(datum.second.Stringfy()).c_str()));
				outstream->Write("'");
			}
			outstream->Write(">");
			if (element.IsType(MultiValue::MVT_STRING) && realm->GetDataType().starts_with(u"string/"))
			{
				auto str = *element.value.stringValue;
				if (realm->GetDataType() == u"string/text" && Configuration::GetInstance().IsExist(u"mule.handler.string-read-proc"))
				{
					auto name = Configuration::GetInstance().GetString(u"mule.handler.string-read-proc");
					str = *mule::Lua::LuaHost::GetInstance().Call(xybase::string::to_string(name), 1, &element).value.stringValue;
				}

				outstream->Write("<![CDATA[");
				outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(str).c_str()));
				outstream->Write("]]>");
			}
			else
				outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(element.Stringfy()).c_str()));
		}

		outstream->Write("</");
		outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(u"item").c_str()));
		outstream->Write(">\n");
	}
	else
	{
		int c;
		// 非组合类型已经在Enter时完成读入和解析，这里只需要找到结束点同步即可
		if (realm->IsComposite())
		{
			do
			{
				c = instream->ReadChar();
			} while (c != '<');
			c = instream->ReadChar();
			if (c != '/')
			{
				throw xybase::RuntimeException(L"Unexpected open tag. Try to find close tag for item.", 95072);
			}
			xybase::StringBuilder<char8_t> sb;
			do
			{
				c = instream->ReadChar();
				sb += c;
			} while (c != '>');
			if (xybase::string::to_utf16(sb.ToString()) != u"item>")
			{
				throw xybase::RuntimeException(std::format(L"Unexpected close tag, expect </item>, but got </{}.", xybase::string::to_wstring(sb.ToString())), 95073);
			}
		}
	}
}

void mule::Xml::XmlHandler::OnDataRead(const MultiValue &value)
{
	if (status == XHS_READ_METADATA_WAITING)
	{
		status = XHS_READ;
		outstream->Write(">");

		outstream->Write("\n");
		layer++;
	}

	element = value;
}

const MultiValue mule::Xml::XmlHandler::OnDataWrite()
{
	// 元素解析已经在上面完成，这里直接返回即可。
	return element;
}

void mule::Xml::XmlHandler::AppendMetadatum(std::u16string name, const mule::Data::Basic::MultiValue &mv)
{
	if (status == XHS_READ_METADATA_WAITING) {
		outstream->Write(' ');
		outstream->Write((char *)(xybase::string::to_utf8(name).c_str()));
		outstream->Write("='");
		outstream->Write((char *)(xybase::string::to_utf8(mv.ToString()).c_str()));
		outstream->Write('\'');
	}

	element.metadata[name] = mv;
}
