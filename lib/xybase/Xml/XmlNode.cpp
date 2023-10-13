#include "XmlNode.h"

using namespace xybase::xml;

const XmlNode XmlNode::ERROR = XmlNode();

bool xybase::xml::XmlNode::IsTextNode() const
{
	return text.size();
}

void xybase::xml::XmlNode::AddChild(XmlNode node)
{
	children.push_back(node);
}

std::list<XmlNode> xybase::xml::XmlNode::GetChildren() const
{
	return children;
}

void xybase::xml::XmlNode::AddText(std::u16string str)
{
	XmlNode tmp;
	tmp.text = str;
	AddChild(tmp);
}

std::u16string xybase::xml::XmlNode::GetText() const
{
	return text;
}

void xybase::xml::XmlNode::SetName(std::u16string name)
{
	this->name = name;
}

std::u16string xybase::xml::XmlNode::GetName() const
{
	return name;
}

void xybase::xml::XmlNode::AddAttribute(std::u16string name, std::u16string data)
{
	attributes[name] = data;
}

std::map<std::u16string, std::u16string> xybase::xml::XmlNode::GetAttributes() const
{
	return attributes;
}

std::u16string xybase::xml::XmlNode::GetAttribute(std::u16string name) const
{
	auto &&itr = attributes.find(name);
	if (itr == attributes.end()) return u"";
	else return itr->second;
}

bool xybase::xml::XmlNode::operator==(const XmlNode &rvalue) const
{
	return name == rvalue.name && attributes == rvalue.attributes;
}

XmlNode &xybase::xml::XmlNode::operator[](std::u16string name)
{
	for (auto &&child : children)
	{
		if (child.name == name)
		{
			return child;
		}
	}

	XmlNode tmp;
	tmp.name = name;
	children.push_back(tmp);
	auto &&itr = children.end();
	--itr;
	return *itr;
}
