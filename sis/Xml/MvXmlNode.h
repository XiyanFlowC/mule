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
         * @brief �ṩMultiValue��Xml�ڵ�Ķ�Ӧ��ϵ
        */
        class MvXmlNode
        {
            int counter = 0;
        public:
            mule::Data::Basic::MultiValue mv;

            std::string name;

            MvXmlNode();

            MvXmlNode(std::string name, const mule::Data::Basic::MultiValue &val);

            MvXmlNode(const MvXmlNode &rvalue);

            MvXmlNode(const MvXmlNode &&movee) noexcept;

            void AddChild(MvXmlNode node);

            std::list<MvXmlNode> GetChildren() const;

            void AddText(std::string str);

            std::string GetText() const;

            void SetName(std::string name);

            std::string GetName() const;

            void AddAttribute(std::string name, std::string data);

            std::map<std::string, std::string> GetAttributes() const;

            const static MvXmlNode ERROR;

            bool operator== (const MvXmlNode &rvalue) const;

            MvXmlNode operator[] (std::string name);

            /**
             * @brief ��MultiValueֵת��ΪXml��Ҫ��Text
            */
            static std::function<std::string(std::string)> text_to_xml;
            /**
             * @brief ��Xmlת��Ϊ��Ҫ��MultiValue
            */
            static std::function<std::string(std::string)> xml_to_text;
        };
    }
}

#endif // !MV_XML_NODE_H__