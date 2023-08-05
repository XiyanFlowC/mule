#include "MvXmlNode.h"

using namespace mule::Xml;
using namespace mule::Data::Basic;

const MvXmlNode MvXmlNode::ERROR;

std::function<std::string(std::string)> mule::Xml::MvXmlNode::callback = [](std::string in) -> std::string {
	auto loc = in.find("]]>");
	while (loc != std::string::npos)
	{
		in = in.replace(loc, 3, "]]>]]><![CDATA[");
		loc = in.find("]]>", loc + 14);
	}
	return "<![CDATA[" + in + "]]>";
};

mule::Xml::MvXmlNode::MvXmlNode()
{
}

mule::Xml::MvXmlNode::MvXmlNode(std::string name, const mule::Data::Basic::MultiValue &val)
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
	auto &&it = mv.metadata.find("_type");
	if (it != mv.metadata.end() && it->second == std::string("array"))
		(*mv.value.mapValue)[(unsigned long long)counter++] = node.mv;
	else
		(*mv.value.mapValue)[node.GetName()] = node.mv;
}

std::list<MvXmlNode> mule::Xml::MvXmlNode::GetChildren() const
{
	std::list<MvXmlNode> ret;
	if (mv.GetType() != MultiValue::MVT_MAP) return ret;

	auto &&it = mv.metadata.find("_type");
	if (it != mv.metadata.end() && it->second == std::string("array"))
	{
		for (int i = 0; i < mv.value.mapValue->size(); ++i)
		{
			auto itr = mv.value.mapValue->find((unsigned long long)i);
			if (itr == mv.value.mapValue->end()) break;

			ret.push_back(MvXmlNode("i", itr->second));
		}
	}
	else for (auto &pair : *mv.value.mapValue)
	{
		ret.push_back(MvXmlNode(pair.first.ToString(), pair.second));
	}

	return ret;
}

void mule::Xml::MvXmlNode::AddText(std::string str)
{
	// 保全现有 metadata
	auto metadata = mv.metadata;
	mv = MultiValue::Parse(str);
	mv.metadata = metadata;
}

std::string mule::Xml::MvXmlNode::GetText() const
{
	if (mv.GetType() == mule::Data::Basic::MultiValue::MVT_STRING)
	{
		return callback(*mv.value.stringValue);
	}
	else
	{
		return mv.Stringfy();
	}
}

void mule::Xml::MvXmlNode::SetName(std::string name)
{
	this->name = name;
}

std::string mule::Xml::MvXmlNode::GetName() const
{
	return name;
}

void mule::Xml::MvXmlNode::AddAttribute(std::string name, std::string data)
{
	if (name == "_type" && data == "array") counter = 0;
	mv.metadata[name] = MultiValue::Parse(data);
}

std::map<std::string, std::string> mule::Xml::MvXmlNode::GetAttributes() const
{
	std::map<std::string, std::string> ret;

	for (auto &&datum : mv.metadata)
	{
		ret[datum.first] = datum.second.Stringfy();
	}

	return ret;
}

bool mule::Xml::MvXmlNode::operator==(const MvXmlNode &rvalue) const
{
	return name == rvalue.name && mv == rvalue.mv;
}

MvXmlNode mule::Xml::MvXmlNode::operator[](std::string name)
{
	return MvXmlNode(name, (*mv.value.mapValue)[name]);
}
