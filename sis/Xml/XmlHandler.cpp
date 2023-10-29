#include "XmlHandler.h"

#include <xystring.h>
#include <Exception/IOException.h>
#include <Exception/InvalidOperationException.h>
#include <Exception/InvalidParameterException.h>

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

void mule::Xml::XmlHandler::OnRealmEnter(Type *realm, const std::u16string& name)
{
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
			outstream->Write(">");

			outstream->Write("\n");
			layer++;
		}
		else nodeName = name;
	}
	else if (status == XHS_WRITE)
	{
		xybase::StringBuilder<char8_t> sb;
		
		char ch = instream->ReadChar();
		while (ch != '<') ch = instream->ReadChar();

		// 获取标签名
		ch = instream->ReadChar();
		while (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n' && ch != '>')
		{
			sb += ch;
			ch = instream->ReadChar();
		}
		
		std::u8string tag = sb.ToString();
		sb += ch;

		if (xybase::string::to_utf16(tag) != name)
			throw xybase::RuntimeException(L"Format error, except " + xybase::string::to_wstring(name) + L", but got " + xybase::string::to_wstring(tag), 9002);

		// 组合类型内部仍为元素，找到结束点同步即可
		if (realm->IsComposite())
		{
			while (ch != '>') ch = instream->ReadChar();
			return;
		}

		// 非组合类型须解析内部元素
		ReadTagAndParse(tag, sb, realm->GetDataType() == u"string");
	}
}

void mule::Xml::XmlHandler::ReadTagAndParse(const std::u8string &tagName, xybase::StringBuilder<char8_t> &sb, bool isString)
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
		element = isString ? MultiValue{text} : MultiValue::Parse(text);
	for (auto &&attr : root.attributes)
	{
		element.metadata[attr.first] = MultiValue::Parse(attr.second);
	}
}

void mule::Xml::XmlHandler::OnRealmExit(Type *realm, const std::u16string& name)
{
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
			if (element.IsType(MultiValue::MVT_STRING))
			{
				outstream->Write("<![CDATA[");
				outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(*element.value.stringValue).c_str()));
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
			do
			{
				c = instream->ReadChar();
			} while (c != '>');
	}
}

void mule::Xml::XmlHandler::OnRealmEnter(Type *realm, int idx)
{
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
			outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(u"item").c_str()));
			outstream->Write(">");

			outstream->Write("\n");
			layer++;
		}
		else nodeName = u"item";
	}
	else if (status == XHS_WRITE)
	{
		xybase::StringBuilder<char8_t> sb;

		char ch = instream->ReadChar();
		while (ch != '<') ch = instream->ReadChar();

		ch = instream->ReadChar();
		while (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n' && ch != '>')
		{
			sb += ch;
			ch = instream->ReadChar();
		}

		std::u8string tag = sb.ToString();
		sb += ch;

		if (xybase::string::to_utf16(tag) != u"item")
			throw xybase::RuntimeException(L"Format error, except " L"item" L", but got " + xybase::string::to_wstring(tag), 9002);

		// 组合类型内部仍为元素，找到结束点同步即可
		if (realm->IsComposite())
		{
			while (ch != '>') ch = instream->ReadChar();
			return;
		}

		// 非组合类型须解析内部元素
		ReadTagAndParse(tag, sb, realm->GetDataType() == u"string");
	}
}

void mule::Xml::XmlHandler::OnRealmExit(Type *realm, int idx)
{
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
			if (element.IsType(MultiValue::MVT_STRING))
			{
				outstream->Write("<![CDATA[");
				outstream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(*element.value.stringValue).c_str()));
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
			do
			{
				c = instream->ReadChar();
			} while (c != '>');
	}
}

void mule::Xml::XmlHandler::OnDataRead(const MultiValue &value)
{
	element = value;
}

const MultiValue mule::Xml::XmlHandler::OnDataWrite()
{
	// 元素解析已经在上面完成，这里直接返回即可。
	return element;
}

void mule::Xml::XmlHandler::AppendMetadatum(std::u16string name, const mule::Data::Basic::MultiValue &mv)
{
	element.metadata[name] = mv;
}
