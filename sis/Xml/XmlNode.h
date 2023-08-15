#pragma once

#ifndef XML_NODE_H__
#define XML_NODE_H__

#include <string>
#include <list>
#include <map>

namespace mule
{
    namespace Xml
    {
        /**
         * @brief Xml节点
        */
        class XmlNode
        {
        public:
            bool IsTextNode() const;

            std::list<XmlNode> children;

            std::u16string text;

            std::u16string name;

            std::map<std::u16string, std::u16string> attributes;

            void AddChild(XmlNode node);

            std::list<XmlNode> GetChildren() const;

            void AddText(std::u16string str);

            std::u16string GetText() const;

            void SetName(std::u16string name);

            std::u16string GetName() const;

            void AddAttribute(std::u16string name, std::u16string data);

            std::map<std::u16string, std::u16string> GetAttributes() const;

            std::u16string GetAttribute(std::u16string name) const;

            const static XmlNode ERROR;

            bool operator== (const XmlNode &rvalue) const;

            XmlNode &operator[] (std::u16string name);
        };
    }
}

#endif // !XML_NODE_H__
