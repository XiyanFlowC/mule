#pragma once

#ifndef XML_PROCESSOR_H__
#define XML_PROCESSOR_H__

#include <map>
#include <list>
#include <Data/Basic/MultiValue.h>
#include <functional>
#include <xystring.h>

namespace mule
{
    namespace Xml
    {
        template<typename XmlNodeT>
        class XmlGenerator
        {
        public:
            int indent = 0;

            XmlGenerator()
            {
            }

            std::string ToXml(const XmlNodeT &value)
            {
                layer = -1;
                xybase::StringBuilder sb;
                ProcNode(value, sb);
                return sb.ToString();
            }

            //std::function<std::string(std::string)> text_to_xml;
        private:
            int layer = 0;

            void ProcNode(const XmlNodeT &value, xybase::StringBuilder<char> &sb)
            {
                if (indent)
                {
                    ++layer;
                    for (int i = 0; i < layer * indent; ++i)
                    {
                        sb += ' ';
                    }
                }
                sb += '<';
                sb += xybase::string::to_utf8(value.GetName());

                std::map<std::u16string, std::u16string> attributes = value.GetAttributes();
                std::list<XmlNodeT> children = value.GetChildren();

                for (auto &&attr : attributes)
                {
                    sb += " " + xybase::string::to_utf8(attr.first) + "='" + xybase::string::to_utf8(attr.second) + "'";
                }

                if (children.size())
                {
                    sb += '>';
                    if (indent)
                        sb += '\n';

                    for (auto &ele : children)
                    {
                        ProcNode(ele, sb);
                    }
                    
                    if (indent)
                    {
                        for (int i = 0; i < layer * indent; ++i)
                        {
                            sb += ' ';
                        }
                        --layer;
                    }
                    sb += "</" + xybase::string::to_utf8(value.GetName()) + ">";
                    if (indent)
                        sb += '\n';
                }
                else
                {
                    std::string text = xybase::string::to_utf8(value.GetText());
                    if (text.size() == 0)
                    {
                        sb += "/>";
                    }
                    else
                    {
                        sb += ">";
                        if (indent)
                        {
                            sb += '\n';
                            ++layer;
                            for (int i = 0; i < layer * indent; ++i)
                            {
                                sb += ' ';
                            }
                        }
                        sb += text;
                        if (indent)
                        {
                            sb += '\n';
                            --layer;
                            for (int i = 0; i < layer * indent; ++i)
                            {
                                sb += ' ';
                            }
                        }
                        sb += "</" + xybase::string::to_utf8(value.GetName()) + ">";
                    }
                    if (indent)
                    {
                        sb += '\n';
                        --layer;
                    }
                }
            }
        };
    }
}

#endif // !XML_PROCESSOR_H__
