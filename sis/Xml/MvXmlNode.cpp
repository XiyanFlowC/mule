#include "MvXmlNode.h"

#include <xystring.h>

using namespace mule::Xml;
using namespace mule::Data::Basic;

const MvXmlNode MvXmlNode::ERROR;

std::function<std::u16string(std::u16string)> mule::Xml::MvXmlNode::text_to_xml = [](std::u16string in) -> std::u16string {
	auto loc = in.find(u"]]>");
	while (loc != std::string::npos)
	{
		in = in.replace(loc, 3, u"]]>]]><![CDATA[");
		loc = in.find(u"]]>", loc + 14);
	}
	return u"<![CDATA[" + in + u"]]>";
};

std::function<std::u16string(std::u16string)> mule::Xml::MvXmlNode::xml_to_text = [](std::u16string in) -> std::u16string {
	return in;
};

mule::Xml::MvXmlNode::MvXmlNode()
{
}

mule::Xml::MvXmlNode::MvXmlNode(std::u16string name, const mule::Data::Basic::MultiValue &val)
	: name(name), mv(val)
{
}

mule::Xml::MvXmlNode::MvXmlNode(const MvXmlNode &rvalue)
	: name(rvalue.name), mv(rvalue.mv)
{
}

mule::Xml::MvXmlNode::MvXmlNode(const MvXmlNode &&movee) noexcept
	: name(movee.name)
{
	mv = movee.mv;
}

void mule::Xml::MvXmlNode::AddChild(MvXmlNode node)
{
	mv.SetType(MultiValue::MVT_MAP);
	/*auto &&it = mv.metadata.find(u"_type");
	if (it != mv.metadata.end() && it->second == std::u16string(u"array"))
		(*mv.value.mapValue)[(uint64_t)counter++] = node.mv;
	else*/
		(*mv.value.mapValue)[node.GetName()] = node.mv;
}

std::list<MvXmlNode> mule::Xml::MvXmlNode::GetChildren() const
{
	std::list<MvXmlNode> ret;
	if (mv.GetType() != MultiValue::MVT_MAP) return ret;

	/*auto &&it = mv.metadata.find(u"_type");
	if (it != mv.metadata.end() && it->second == std::u16string(u"array"))
	{
		for (size_t i = 0; i < mv.value.mapValue->size(); ++i)
		{
			auto itr = mv.value.mapValue->find((uint64_t)i);
			if (itr == mv.value.mapValue->end()) break;

			ret.push_back(MvXmlNode(u"i", itr->second));
		}
	}
	else*/ for (auto &pair : *mv.value.mapValue)
	{
		ret.push_back(MvXmlNode(xybase::string::to_utf16(pair.first.ToString()), pair.second));
	}

	return ret;
}

void mule::Xml::MvXmlNode::AddText(std::u16string str)
{
	// 保全现有 metadata
	auto metadata = mv.metadata;
	mv = MultiValue::Parse(xybase::string::to_wstring(str));
	mv.metadata = metadata;
}

std::u16string mule::Xml::MvXmlNode::GetText() const
{
	if (mv.GetType() == mule::Data::Basic::MultiValue::MVT_STRING)
	{
		return text_to_xml(*mv.value.stringValue);
	}
	else
	{
		return xybase::string::to_utf16(mv.Stringfy());
	}
}

void mule::Xml::MvXmlNode::SetName(std::u16string name)
{
	this->name = name;
}

std::u16string mule::Xml::MvXmlNode::GetName() const
{
	return name;
}

void mule::Xml::MvXmlNode::AddAttribute(std::u16string name, std::u16string data)
{
	// if (name == u"_type" && data == u"array") counter = 0;
	mv.metadata[name] = MultiValue::Parse(xybase::string::to_wstring(data));
}

std::map<std::u16string, std::u16string> mule::Xml::MvXmlNode::GetAttributes() const
{
	std::map<std::u16string, std::u16string> ret;

	for (auto &&datum : mv.metadata)
	{
		ret[datum.first] = xybase::string::to_utf16(datum.second.Stringfy());
	}

	return ret;
}

bool mule::Xml::MvXmlNode::operator==(const MvXmlNode &rvalue) const
{
	return name == rvalue.name && mv == rvalue.mv;
}

MvXmlNode mule::Xml::MvXmlNode::operator[](std::u16string name)
{
	return MvXmlNode(name, (*mv.value.mapValue)[name]);
}
