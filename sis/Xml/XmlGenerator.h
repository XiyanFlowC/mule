#pragma once

#ifndef XML_PROCESSOR_H__
#define XML_PROCESSOR_H__

#include <map>
#include <list>
#include <Data/Basic/MultiValue.h>
#include <functional>

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

            //std::function<std::string(std::string)> callback;
        private:
            int layer;

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
                sb += value.GetName();

                std::map<std::string, std::string> attributes = value.GetAttributes();
                std::list<XmlNodeT> children = value.GetChildren();

                for (auto &&attr : attributes)
                {
                    sb += " " + attr.first + "='" + attr.second + "'";
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
                    sb += "</" + value.GetName() + ">";
                    if (indent)
                        sb += '\n';
                }
                else
                {
                    std::string text = value.GetText();
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
                        sb += "</" + value.GetName() + ">";
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
