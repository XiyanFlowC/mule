#pragma once

#ifndef XML_PARSER_H__
#define XML_PARSER_H__

#include <string>
#include <map>
#include <functional>

namespace mule
{
    namespace Xml
    {
        /**
         * @brief 处理Xml的类。
        */
        template <typename XmlNodeT>
        class XmlParser
        {
        public:
            XmlParser();

            XmlNodeT Parse(const std::string &xml);

            void RegisterTagCallback(const std::string &tagName, std::function<std::string(XmlNodeT &)> callback);

            void RegisterEntity(const std::string entityName, const std::string entitySeq);

            std::string error;

            bool textOnlyLeaves;

        private:
            XmlNodeT ParseNode(const std::string &xml);

            size_t index;

            std::map<std::string, std::function<std::string(XmlNodeT &)>> callbacks;

            std::map<std::string, std::string> entities;
        };


        template<typename XmlNodeT>
        inline void XmlParser<XmlNodeT>::RegisterEntity(const std::string entityName, const std::string entitySeq)
        {
            entities[entityName] = entitySeq;
        }

        template<typename XmlNodeT>
        inline XmlParser<XmlNodeT>::XmlParser()
            : textOnlyLeaves(false)
        {
            RegisterEntity("amp", "&");
            RegisterEntity("lt", "<");
            RegisterEntity("gt", ">");
            RegisterEntity("lf", "\n");
            RegisterEntity("cr", "\r");
            RegisterEntity("sp", " ");
        }

        template<typename XmlNodeT>
        inline XmlNodeT XmlParser<XmlNodeT>::Parse(const std::string &xml)
        {
            error = "";
            index = 0;
            return std::get<1>(ParseNode(xml));
        }

        template<typename XmlNodeT>
        inline void XmlParser<XmlNodeT>::RegisterTagCallback(const std::string &tagName, std::function<std::string(XmlNodeT &)> callback)
        {
            callbacks[tagName] = callback;
        }

        template<typename XmlNodeT>
        XmlNodeT XmlParser<XmlNodeT>::ParseNode(const std::string &xml)
        {
            // 去除空白字符
            index = xml.find_first_not_of(" \t\n\r", index);
            // 节点起始
            if (xml[index] == '<')
            {
                XmlNodeT node;

                if (xml[index + 1] == '/')
                {
                    error += "Near " + xml.substr(index, 32) + ": Unexpected close tag.\n";
                    index = xml.find(">", index) + 1;
                    return { "$ERROR", mule::Data::Basic::MultiValue::MV_NULL };
                }

                // 开启标签
                index = xml.find_first_not_of(" \t\n\r", index + 1);
                size_t endIndex = xml.find_first_of(" \t\n\r>/", index);
                node.SetName(xml.substr(index, endIndex - index));

                // 处理特性
                while (true)
                {
                    index = xml.find_first_not_of(" \t\n\r", endIndex);
                    if (xml[index] == '>' || xml[index] == '/')
                    {
                        break;
                    }
                    size_t attrEndIndex = xml.find('=', index);
                    std::string attrName = xml.substr(index, attrEndIndex - index);
                    size_t attrValueStart = xml.find_first_of("\"'", attrEndIndex) + 1;
                    size_t attrValueEnd = xml.find(xml[attrValueStart - 1], attrValueStart);
                    std::string attrValue = xml.substr(attrValueStart, attrValueEnd - attrValueStart);
                    node.AddAttribute(attrName, attrValue);
                    endIndex = xml.find_first_of(" \t\n\r>/", attrValueEnd + 1);
                }

                // 无子节点节点
                if (xml[index] == '/')
                {
                    if (xml[index + 1] == '>')
                    {
                        index += 2;
                        return node;
                    }

                    error += "Near " + xml.substr(index, 32) + ": unexpected character '/', ignoring...\n";
                    index = xml.find('>', index);
                }
                else // xml[index] == '>'
                {
                    index++;
                }

                // 处理子节点
                while (index < xml.size())
                {
                    // 去除空白字符
                    index = xml.find_first_not_of(" \t\n\r", index);
                    if (xml[index] == '<')
                    {
                        // 关闭标签
                        if (xml[index + 1] == '/')
                        {
                            index = xml.find_first_not_of(" \t\n\r", index + 2);
                            size_t endIndex = xml.find_first_of(" \t\n\r>", index);

                            // 非相等的关闭标签，忽略
                            if (node.GetName() != xml.substr(index + 2, endIndex - index))
                            {
                                error += "Near " + xml.substr(index, 32) + ": unpairing with open tag " + node.GetName() + ", ignoring...\n";
                                index = xml.find('>', index);
                                continue;
                            }
                            index = xml.find('>', index);
                            return node;
                        }
                        // 子节点，解析
                        else
                        {
                            node.AddChild(ParseNode(xml));
                        }
                    }
                    // 文本节点
                    else
                    {
                        xybase::StringBuilder sb;
                        while (true)
                        {
                            if (xml[index] == '&')
                            {
                                if (xml[index + 1] == '#')
                                {
                                    // 处理直接值
                                    error += "Near " + xml.substr(index, 32) + ": unimplemented entity type, ignoring...\n";
                                }
                                // 处理转义序列
                                size_t escapeStart = index + 1;
                                index = xml.find(';', index);
                                sb += entities[xml.substr(escapeStart, index - escapeStart)];
                                index++;
                            }
                            // 空白字符合并
                            else if (xml[index] == ' ' || xml[index] == '\t' || xml[index] == '\r' || xml[index] == '\n')
                            {
                                index = xml.find_first_not_of(" \t\r\n", index);
                                if (xml[index] == '<' && xml[index + 1] == '/')
                                {
                                    continue;
                                }
                                else
                                {
                                    sb += ' ';
                                }
                            }
                            else if (xml[index] == '<' && xml[index + 1] == '!' && xml[index + 2] == '[')
                            {
                                size_t blockNameStart = index + 3;
                                size_t blockNameEnd = xml.find("[", index + 3);

                                auto blockName = xml.substr(blockNameStart, blockNameEnd - blockNameStart);
                                // 为保持拓展性，此处允许处理CDATA以外的块
                                if (blockName == "CDATA")
                                {
                                    // 处理CDATA块
                                    size_t cdataStart = blockNameEnd + 1;
                                    index = xml.find("]]>", index);
                                    sb += xml.substr(cdataStart, index - cdataStart + 3);
                                    index += 3;
                                }
                                else
                                {
                                    // 无效块，忽视，不处理
                                    error += "Near " + xml.substr(index, 32) + ": unknown block type " + blockName + ", ignoring...\n";
                                    index = xml.find("]]>", index);
                                    index += 3;
                                }
                            }
                            else if (xml[index] == '<')
                            {
                                // 节点关闭
                                if (xml[index + 1] == '/')
                                {
                                    index = xml.find_first_not_of(" \t\n\r", index + 2);
                                    size_t endIndex = xml.find_first_of(" \t\n\r>", index);

                                    // 非相等的关闭标签，忽略
                                    if (node.GetName() != xml.substr(index + 2, endIndex - index))
                                    {
                                        error += "Near " + xml.substr(index, 32) + ": unpairing with open tag " + node.GetName() + ", ignoring...\n";
                                        index = xml.find('>', index);
                                        continue;
                                    }
                                    index = xml.find('>', index);
                                    node.SetText(sb.ToString());
                                    return node;
                                }
                                // 节点/节点打开
                                else
                                {
                                    if (textOnlyLeaves)
                                    {
                                        auto res = ParseNode(xml);
                                        auto itr = callbacks.find(std::get<0>(res));
                                        if (itr != callbacks.end())
                                        {
                                            sb += itr->second(std::get<1>(res));
                                        }
                                        else
                                        {
                                            error += "Near " + xml.substr(index, 32) + ": unknown callback for embedded element " + std::get<0>(res) + ", ignoring...\n";
                                        }
                                    }
                                    else
                                    {
                                        node.SetText(sb.ToString());
                                        return node;
                                    }
                                }
                            }
                            else
                            {
                                sb += xml[index++];
                            }
                        }
                    }
                }
                return node;
            }
            error += "Near " + xml.substr(index, 32) + ": expected '<', but got '" + xml[index] + "'.\n";
            // 寻找同步点
            index = xml.find("<", index);
            return XmlNodeT::ERROR;
        }
    }
}

#endif /* End of XML_PARSER_H__*/
