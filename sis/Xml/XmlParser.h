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

            bool mangleEmbeddedNodes;

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
            : mangleEmbeddedNodes(false)
        {
            // XML 1.0 预定义实体
            RegisterEntity("amp", "&");
            RegisterEntity("lt", "<");
            RegisterEntity("gt", ">");
            RegisterEntity("apos", "'");
            RegisterEntity("quot", "\"");

            // 拓展的通用实体
            RegisterEntity("lf", "\n");
            RegisterEntity("cr", "\r");
            RegisterEntity("sp", " ");
        }

        template<typename XmlNodeT>
        inline XmlNodeT XmlParser<XmlNodeT>::Parse(const std::string &xml)
        {
            error = "";
            index = 0;
            return ParseNode(xml);
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
                    return XmlNodeT::ERROR;
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

                // 空元素标签
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
                    // 处理标签节点
                    if (xml[index] == '<' && xml[index + 1] != '!')
                    {
                        // 处理关闭标签：由于内部元素全部递归解析，此处能遇到的关闭标签一定是该元素的关闭标签
                        if (xml[index + 1] == '/')
                        {
                            index = xml.find_first_not_of(" \t\n\r", index + 2);
                            size_t endIndex = xml.find_first_of(" \t\n\r>", index);

                            // 非相等的关闭标签，忽略
                            if (node.GetName() != xml.substr(index, endIndex - index))
                            {
                                error += "Near " + xml.substr(index, 32) + ": unpairing with open tag " + node.GetName() + ", ignoring...\n";
                                index = xml.find('>', index);
                                continue;
                            }
                            index = xml.find('>', index) + 1;
                            return node;
                        }
                        // 子节点，递归解析
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
                                // 去尾，立即返回
                                if (xml[index] == '<' && xml[index + 1] == '/')
                                {
                                    node.AddText(sb.ToString());
                                    break;
                                }
                                else
                                {
                                    sb += ' ';
                                }
                            }
                            else if (xml[index] == '<')
                            {
                                // 节点关闭
                                if (xml[index + 1] == '/')
                                {
                                    node.AddText(sb.ToString());
                                    break;
                                }
                                // XML 解析器命令
                                if (xml[index + 1] == '!')
                                {
                                    if (xml[index + 2] == '[')
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
                                            sb += xml.substr(cdataStart, index - cdataStart);
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
                                }
                                // 标签打开
                                else
                                {
                                    if (mangleEmbeddedNodes)
                                    {
                                        // 通过回调函数碾碎
                                        auto res = ParseNode(xml);
                                        auto itr = callbacks.find(res.GetName());
                                        if (itr != callbacks.end())
                                        {
                                            sb += itr->second(res);
                                        }
                                        else
                                        {
                                            error += "Near " + xml.substr(index, 32) + ": unknown callback for embedded element " + res.GetName() + ", ignoring...\n";
                                        }
                                    }
                                    else
                                    {
                                        node.AddText(sb.ToString());
                                        break;
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
