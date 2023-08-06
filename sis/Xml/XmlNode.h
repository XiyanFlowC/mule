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
         * @brief Xml½Úµã
        */
        class XmlNode
        {
        public:
            bool IsTextNode() const;

            std::list<XmlNode> children;

            std::string text;

            std::string name;

            std::map<std::string, std::string> attributes;

            void AddChild(XmlNode node);

            std::list<XmlNode> GetChildren() const;

            void AddText(std::string str);

            std::string GetText() const;

            void SetName(std::string name);

            std::string GetName() const;

            void AddAttribute(std::string name, std::string data);

            std::map<std::string, std::string> GetAttributes() const;

            std::string GetAttribute(std::string name) const;

            const static XmlNode ERROR;

            bool operator== (const XmlNode &rvalue) const;

            XmlNode &operator[] (std::string name);
        };
    }
}

#endif // !XML_NODE_H__
