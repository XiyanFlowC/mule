#pragma once

#ifndef MV_XML_NODE_H__
#define MV_XML_NODE_H__

#include <map>
#include <list>
#include <functional>
#include <Data/Basic/MultiValue.h>

namespace mule
{
    namespace Xml
    {
        /**
         * @brief 提供MultiValue和Xml节点的对应关系
        */
        class MvXmlNode
        {
        public:
            mule::Data::Basic::MultiValue mv;

            std::string name;

            MvXmlNode();

            MvXmlNode(std::string name, const mule::Data::Basic::MultiValue &val);

            MvXmlNode(const MvXmlNode &rvalue);

            MvXmlNode(const MvXmlNode &&movee) noexcept;

            void AddChild(MvXmlNode node);

            std::list<MvXmlNode> GetChildren() const;

            void SetText(std::string str);

            std::string GetText() const;

            void SetName(std::string name);

            std::string GetName() const;

            void AddAttribute(std::string name, std::string data);

            std::map<std::string, std::string> GetAttributes() const;

            const static MvXmlNode ERROR;

            bool operator== (const MvXmlNode &rvalue) const;

            MvXmlNode operator[] (std::string name);

            static std::function<std::string(std::string)> callback;
        };
    }
}

#endif // !MV_XML_NODE_H__
