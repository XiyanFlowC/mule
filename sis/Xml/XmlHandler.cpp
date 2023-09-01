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
	entities["amp"] = "&";
	entities["qout"] = "\"";
	entities["lt"] = "<";
	entities["gt"] = ">";
	entities["apos"] = "'";
	status = XHS_IDLE;
}

void mule::Xml::XmlHandler::OnSheetReadStart()
{
	if (status != XHS_IDLE) throw xybase::InvalidOperationException(u"Cannot read whilst this handler is busy.", 3501);
	status = XHS_READ;
}

void mule::Xml::XmlHandler::OnSheetReadEnd()
{
	if (status != XHS_READ) throw xybase::InvalidOperationException(u"Cannot stop read whilst this handler is not read.", 3502);
	status = XHS_IDLE;
}

void mule::Xml::XmlHandler::OnSheetWriteStart()
{
	if (status != XHS_IDLE) throw xybase::InvalidOperationException(u"Cannot write whilst this handler is busy.", 3503);
	status = XHS_WRITE;
}

void mule::Xml::XmlHandler::OnSheetWriteEnd()
{
	if (status != XHS_WRITE) throw xybase::InvalidOperationException(u"Cannot stop write whilst this handler is not write.", 3504);
	status = XHS_IDLE;
}

void mule::Xml::XmlHandler::OnRealmEnter(Type *realm, const std::u16string& name)
{
	if (status == XHS_READ)
	{
		for (int i = 0; i < layer; ++i)
			for (int j = 0; j < ident; ++j)
			{
				stream->Write(type ? " " : "\t");
			}

		if (realm->IsComposite())
		{
			stream->Write("<");
			stream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(name).c_str()));
			stream->Write(">");

			stream->Write("\n");
			layer++;
		}
		else nodeName = name;
	}
	else if (status == XHS_WRITE)
	{
		xybase::StringBuilder<char8_t> sb;
		
		char ch = stream->ReadChar();
		while (ch != '<') ch = stream->ReadChar();

		ch = stream->ReadChar();
		while (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n' && ch != '>')
		{
			sb += ch;
			ch = stream->ReadChar();
		}
		
		std::u8string tag = sb.ToString();
		sb.Clear();

		if (xybase::string::to_utf16(tag) != name) throw xybase::RuntimeException(u"Format error, except " + name + u", but got " + xybase::string::to_utf16(name), 9002);

		ProcessAttributes(ch, sb);
	}
}

void mule::Xml::XmlHandler::ProcessAttributes(char &ch, xybase::StringBuilder<char8_t> &sb)
{
	while (ch != '>')
	{
		// 获取特性名
		ch = stream->ReadChar();
		while (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n') ch = stream->ReadChar();

		sb.Append(ch);
		while (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n' && ch != '=')
		{
			sb.Append(ch);
			ch = stream->ReadChar();
		}

		std::u16string attrName = xybase::string::to_utf16(sb.ToString());
		sb.Clear();

		if (ch != '=')
		{
			do
			{
				ch = stream->ReadChar();
			} while (ch != '=');
		}

		// 获取特性值
		while (ch != '\'' && ch != '\"') ch = stream->ReadChar();
		char endCh = ch;
		ch = stream->ReadChar();
		while (ch != endCh)
		{
			sb.Append(ch);
			ch = stream->ReadChar();
		}
		std::u16string attrValue = xybase::string::to_utf16(sb.ToString());
		sb.Clear();

		metadata[attrName] = mule::Data::Basic::MultiValue::Parse(attrValue);
		do
		{
			ch = stream->ReadChar();
		} while (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n');
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
					stream->Write(type ? " " : "\t");
				}
		}

		stream->Write("</");
		stream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(name).c_str()));
		stream->Write(">\n");
	}
	else
	{
		int c;
		do
		{
			c = stream->ReadChar();
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
				stream->Write(type ? " " : "\t");
			}

		if (realm->IsComposite())
		{
			stream->Write("<");
			stream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(realm->GetTypeName()).c_str()));
			stream->Write(">");

			stream->Write("\n");
			layer++;
		}
		else nodeName = realm->GetTypeName();
	}
	else if (status == XHS_WRITE)
	{
		xybase::StringBuilder<char8_t> sb;

		char ch = stream->ReadChar();
		while (ch != '<') ch = stream->ReadChar();

		ch = stream->ReadChar();
		while (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n' && ch != '>')
		{
			sb += ch;
			ch = stream->ReadChar();
		}

		std::u8string tag = sb.ToString();
		sb.Clear();

		if (xybase::string::to_utf16(tag) != realm->GetTypeName()) throw xybase::RuntimeException(u"Format error, except " + realm->GetTypeName() + u", but got " + xybase::string::to_utf16(realm->GetTypeName()), 9002);

		ProcessAttributes(ch, sb);
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
					stream->Write(type ? " " : "\t");
				}
		}

		stream->Write("</");
		stream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(realm->GetTypeName()).c_str()));
		stream->Write(">\n");
	}
	else
	{
		int c;
		do
		{
			c = stream->ReadChar();
		} while (c != '>');
	}
}

void mule::Xml::XmlHandler::OnDataRead(const MultiValue &value)
{
	stream->Write("<");
	stream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(nodeName).c_str()));
	for (auto &&datum : value.metadata)
	{
		stream->Write(" ");
		stream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(datum.first).c_str()));
		stream->Write("='");
		stream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(datum.second.Stringfy()).c_str()));
		stream->Write("'");
	}
	stream->Write(">");
	if (value.IsType(MultiValue::MVT_STRING))
	{
		/*auto in = value.Stringfy();
		auto loc = in.find(u"&");
		while (loc != std::string::npos)
		{
			in = in.replace(loc, 3, u"&amp;");
			loc = in.find(u"&", loc + 5);
		}
		loc = in.find(u"<");
		while (loc != std::string::npos)
		{
			in = in.replace(loc, 3, u"&lt;");
			loc = in.find(u"<", loc + 5);
		}
		stream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(in).c_str()));*/
		stream->Write("<![CDATA[");
		stream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(*value.value.stringValue).c_str()));
		stream->Write("]]>");
	}
	else
		stream->Write(reinterpret_cast<const char *>(xybase::string::to_utf8(value.Stringfy()).c_str()));
}

MultiValue mule::Xml::XmlHandler::OnDataWrite()
{
	xybase::StringBuilder<char8_t> sb;

	char c;

	c = stream->ReadChar();
	while (c != EOF) {

		if (c == '<')
		{
			c = stream->ReadChar();
			if (c == '!')
			{

			}
			else
			{
				sb.Append('<');
				sb.Append(c);
			}
		}

		if (c == '&')
		{
			std::string enty;
			while (c != ';')
			{
				enty += c;
				c = stream->ReadChar();
			}
			sb.Append((const char8_t *)entities[enty].c_str());
		}
		sb.Append(c);
		c = stream->ReadChar();
	}
	stream->ReadChar();

	std::u16string text = xybase::string::to_utf16(sb.ToString());
	text.erase(0, text.find_first_not_of(u" \t\n\r\f\v"));
	text.erase(text.find_last_not_of(u" \t\n\r\f\v") + 1);

	MultiValue ret = MultiValue::Parse(text);
	ret.metadata = metadata;
	metadata.clear();
	return ret;
}

void mule::Xml::XmlHandler::SetStream(xybase::Stream *stream)
{
	this->stream = dynamic_cast<xybase::TextStream *>(stream);
	if (this->stream == nullptr) throw xybase::InvalidParameterException(u"stream", u"Not a text stream for output.", 3505);
}
