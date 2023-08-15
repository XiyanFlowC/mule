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
            int counter = 0;
        public:
            mule::Data::Basic::MultiValue mv;

            std::u16string name;

            MvXmlNode();

            MvXmlNode(std::u16string name, const mule::Data::Basic::MultiValue &val);

            MvXmlNode(const MvXmlNode &rvalue);

            MvXmlNode(const MvXmlNode &&movee) noexcept;

            void AddChild(MvXmlNode node);

            std::list<MvXmlNode> GetChildren() const;

            void AddText(std::u16string str);

            std::u16string GetText() const;

            void SetName(std::u16string name);

            std::u16string GetName() const;

            void AddAttribute(std::u16string name, std::u16string data);

            std::map<std::u16string, std::u16string> GetAttributes() const;

            const static MvXmlNode ERROR;

            bool operator== (const MvXmlNode &rvalue) const;

            MvXmlNode operator[] (std::u16string name);

            /**
             * @brief 从MultiValue值转换为Xml需要的Text
            */
            static std::function<std::u16string(std::u16string)> text_to_xml;
            /**
             * @brief 将Xml转换为需要的MultiValue
            */
            static std::function<std::u16string(std::u16string)> xml_to_text;
        };
    }
}

#endif // !MV_XML_NODE_H__
