#include "XmlNode.h"

using namespace mule::Xml;

const static XmlNode ERROR;

void mule::Xml::XmlNode::AddChild(XmlNode node)
{
	children.push_back(node);
}

std::list<XmlNode> mule::Xml::XmlNode::GetChildren() const
{
	return children;
}

void mule::Xml::XmlNode::SetText(std::string str)
{
	text = str;
}

std::string mule::Xml::XmlNode::GetText() const
{
	return text;
}

void mule::Xml::XmlNode::SetName(std::string name)
{
	this->name = name;
}

std::string mule::Xml::XmlNode::GetName() const
{
	return name;
}

void mule::Xml::XmlNode::AddAttribute(std::string name, std::string data)
{
	attributes[name] = data;
}

std::map<std::string, std::string> mule::Xml::XmlNode::GetAttributes() const
{
	return attributes;
}

bool mule::Xml::XmlNode::operator==(const XmlNode &rvalue) const
{
	return name == rvalue.name && attributes == rvalue.attributes;
}

XmlNode &mule::Xml::XmlNode::operator[](std::string name)
{
	for (auto &&child : children)
	{
		if (child.name == name)
		{
			return child;
		}
	}
}
