#pragma once

#ifndef XML_PARSER_H__
#define XML_PARSER_H__

#include <string>
#include <map>
#include <functional>

#include <StringBuilder.h>
#include <xyutils.h>

namespace mule
{
    namespace Xml
    {

        // 特化强行停止
        template <typename Ch>
        std::basic_string<Ch> to_utf(long v) {
            return std::basic_string<Ch>{static_cast<Ch>(v)};
        }

        // utf8 特化
        template <>
        std::u8string to_utf<char8_t>(long value);

        // utf16 特化
        template <>
        std::u16string to_utf<char16_t>(long value);

        // utf32 特化
        template <>
        std::u32string to_utf<char32_t>(long value);

        template <typename Ch>
        std::basic_string<Ch> str_encflip(const std::string &str)
        {
            return str;
        }

        template <>
        std::basic_string<char8_t> str_encflip(const std::string &str);

        template <>
        std::basic_string<char16_t> str_encflip(const std::string &str);

        template <>
        std::basic_string<char32_t> str_encflip(const std::string &str);

        /**
         * @brief 处理Xml的类
         * @tparam XmlNodeT Xml节点类型
         * @tparam Ch Xml文本流字符类型
        */
        template <typename XmlNodeT, typename Ch = char>
        class XmlParser
        {
        public:
            XmlParser();

            XmlNodeT Parse(const std::basic_string<Ch> &xml);

            void RegisterTagCallback(const std::u16string &tagName, std::function<std::basic_string<Ch>(XmlNodeT &)> callback);

            void RegisterEntity(const std::basic_string<Ch> entityName, const std::basic_string<Ch> entitySeq);

            std::string error;

            bool mangleEmbeddedNodes;

        private:
            XmlNodeT ParseNode(const std::basic_string<Ch> &xml);

            size_t index;

            std::map<std::u16string, std::function<std::basic_string<Ch>(XmlNodeT &)>> callbacks;

            std::map<std::basic_string<Ch>, std::basic_string<Ch>> entities;
        };


        template<typename XmlNodeT, typename Ch>
        inline void XmlParser<XmlNodeT, Ch>::RegisterEntity(const std::basic_string<Ch> entityName, const std::basic_string<Ch> entitySeq)
        {
            entities[entityName] = entitySeq;
        }

        template<typename XmlNodeT, typename Ch>
        inline XmlParser<XmlNodeT, Ch>::XmlParser()
            : mangleEmbeddedNodes(false)
        {
            // XML 1.0 预定义实体
            RegisterEntity(str_encflip<Ch>("amp"), str_encflip<Ch>("&"));
            RegisterEntity(str_encflip<Ch>("lt"), str_encflip<Ch>("<"));
            RegisterEntity(str_encflip<Ch>("gt"), str_encflip<Ch>(">"));
            RegisterEntity(str_encflip<Ch>("apos"), str_encflip<Ch>("'"));
            RegisterEntity(str_encflip<Ch>("quot"), str_encflip<Ch>("\""));

            //// 拓展的通用实体
            //RegisterEntity("lf", "\n");
            //RegisterEntity("cr", "\r");
            //RegisterEntity("sp", " ");
        }

        template<typename XmlNodeT, typename Ch>
        inline XmlNodeT XmlParser<XmlNodeT, Ch>::Parse(const std::basic_string<Ch> &xml)
        {
            error = "";
            index = 0;
            return ParseNode(xml);
        }

        template<typename XmlNodeT, typename Ch>
        inline void XmlParser<XmlNodeT, Ch>::RegisterTagCallback(const std::u16string &tagName, std::function<std::basic_string<Ch>(XmlNodeT &)> callback)
        {
            callbacks[tagName] = callback;
        }

        template<typename XmlNodeT, typename Ch>
        XmlNodeT XmlParser<XmlNodeT, Ch>::ParseNode(const std::basic_string<Ch> &xml)
        {
            // 去除空白字符
            index = xml.find_first_not_of(str_encflip<Ch>(" \t\n\r"), index);

            // 去除顶层注释（喵的越写越乱了，找时间换成开源Xml解析库得了
            while (xml[index] == '<' && xml[index + 1] == '!' && xml[index + 2] == '-' && xml[index + 3] == '-')
            {
                index = xml.find(str_encflip<Ch>("-->"), index + 4) + 3;
                index = xml.find_first_not_of(str_encflip<Ch>(" \t\n\r"), index);
            }

            // 节点起始
            if (xml[index] == '<')
            {
                XmlNodeT node{};

                if (xml[index + 1] == '/')
                {
                    error += "Near " + xybase::string::to_string(xml.substr(index, 32)) + ": Unexpected close tag.\n";
                    index = xml.find(str_encflip<Ch>(">"), index) + 1;
                    return XmlNodeT::ERROR;
                }

                // 开启标签
                index = xml.find_first_not_of(str_encflip<Ch>(" \t\n\r"), index + 1);
                size_t endIndex = xml.find_first_of(str_encflip<Ch>(" \t\n\r>/"), index);
                node.SetName(xybase::string::to_utf16(xml.substr(index, endIndex - index)));

                // 处理特性
                while (true)
                {
                    index = xml.find_first_not_of(str_encflip<Ch>(" \t\n\r"), endIndex);
                    if (xml[index] == '>' || xml[index] == '/')
                    {
                        break;
                    }
                    size_t attrEndIndex = xml.find('=', index);
                    std::basic_string<Ch> attrName = xml.substr(index, attrEndIndex - index);
                    size_t attrValueStart = xml.find_first_of(str_encflip<Ch>("\"'"), attrEndIndex) + 1;
                    size_t attrValueEnd = xml.find(xml[attrValueStart - 1], attrValueStart);
                    std::basic_string<Ch> attrValue = xml.substr(attrValueStart, attrValueEnd - attrValueStart);
                    node.AddAttribute(xybase::string::to_utf16(attrName), xybase::string::to_utf16(attrValue));
                    endIndex = xml.find_first_of(str_encflip<Ch>(" \t\n\r>/"), attrValueEnd + 1);
                }

                // 空元素标签
                if (xml[index] == '/')
                {
                    if (xml[index + 1] == '>')
                    {
                        index += 2;
                        return node;
                    }

                    error += "Near " + xybase::string::to_string(xml.substr(index, 32)) + ": unexpected character '/', ignoring...\n";
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
                    index = xml.find_first_not_of(str_encflip<Ch>(" \t\n\r"), index);

                    // 处理标签节点
                    if (xml[index] == '<' && xml[index + 1] != '!')
                    {
                        // 处理关闭标签：由于内部元素全部递归解析，此处能遇到的关闭标签一定是该元素的关闭标签
                        if (xml[index + 1] == '/')
                        {
                            index = xml.find_first_not_of(str_encflip<Ch>(" \t\n\r"), index + 2);
                            size_t endIndex = xml.find_first_of(str_encflip<Ch>(" \t\n\r>"), index);

                            // 非相等的关闭标签，忽略
                            if (xybase::string::to_utf16(node.GetName()) != xybase::string::to_utf16(xml.substr(index, endIndex - index)))
                            {
                                error += "Near " + xybase::string::to_string(xml.substr(index, 32)) + ": unpairing with open tag " + xybase::string::to_string(node.GetName()) + ", ignoring...\n";
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
                    // 处理注释
                    else if (xml[index] == '<' && xml[index + 1] == '!' && xml[index + 2] == '-' && xml[index + 3] == '-')
                    {
                        index = xml.find(str_encflip<Ch>("-->"), index + 4);
                        index = xml.find_first_not_of(str_encflip<Ch>(" \t\n\r"), index + 3);
                    }
                    // 文本节点
                    else
                    {
                        xybase::StringBuilder<Ch> sb;
                        while (true)
                        {
                            if (xml[index] == '&')
                            {
                                // 处理转义序列
                                size_t escapeStart = index + 1;
                                index = xml.find(';', index);
                                std::basic_string<Ch> seq = xml.substr(escapeStart, index - escapeStart);
                                if (seq[0] == '#')
                                {
                                    // 处理直接值
                                    if (seq[1] == 'x')
                                    {
                                        sb += to_utf<Ch>(static_cast<Ch>(xybase::string::stoi(seq.substr(2), 16)));
                                    }
                                    else
                                    {
                                        sb += to_utf<Ch>(static_cast<Ch>(xybase::string::stoi(seq.substr(1))));
                                    }
                                }
                                else sb += entities[seq];
                                index++;
                            }
                            // 空白字符去尾
                            else if (xml[index] == ' ' || xml[index] == '\t' || xml[index] == '\r' || xml[index] == '\n')
                            {
                                size_t endOfBlank = xml.find_first_not_of(str_encflip<Ch>(" \t\r\n"), index);
                                // 去尾，立即返回
                                if (xml[endOfBlank] == '<' && xml[endOfBlank + 1] == '/')
                                {
                                    node.AddText(xybase::string::to_utf16(sb.ToString()));
                                    index = endOfBlank;
                                    break;
                                }
                                else
                                {
                                    sb += xml.substr(index, endOfBlank - index);
                                    index = endOfBlank;
                                }
                            }
                            else if (xml[index] == '<')
                            {
                                // 节点关闭
                                if (xml[index + 1] == '/')
                                {
                                    node.AddText(xybase::string::to_utf16(sb.ToString()));
                                    break;
                                }
                                // XML 解析器命令
                                if (xml[index + 1] == '!')
                                {
                                    if (xml[index + 2] == '[')
                                    {
                                        size_t blockNameStart = index + 3;
                                        size_t blockNameEnd = xml.find('[', index + 3);

                                        auto blockName = xml.substr(blockNameStart, blockNameEnd - blockNameStart);
                                        // 为保持拓展性，此处允许处理CDATA以外的块
                                        if (blockName == str_encflip<Ch>("CDATA"))
                                        {
                                            // 处理CDATA块
                                            size_t cdataStart = blockNameEnd + 1;
                                            index = xml.find(str_encflip<Ch>("]]>"), index);
                                            sb += xml.substr(cdataStart, index - cdataStart);
                                            index += 3;
                                        }
                                        else
                                        {
                                            // 无效块，忽视，不处理
                                            error += "Near " + xybase::string::to_string(xml.substr(index, 32)) + ": unknown block type " + xybase::string::to_string(blockName) + ", ignoring...\n";
                                            index = xml.find(str_encflip<Ch>("]]>"), index);
                                            index += 3;
                                        }
                                    }
                                    else if (xml[index + 2] == '-' && xml[index + 3] == '-')
                                    {
                                        index = xml.find(str_encflip<Ch>("-->"), index + 4);
                                        index = xml.find_first_not_of(str_encflip<Ch>(" \t\n\r"), index + 3);
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
                                            error += "Near " + xybase::string::to_string(xml.substr(index, 32)) + ": unknown callback for embedded element " + xybase::string::to_string(res.GetName()) + ", ignoring...\n";
                                        }
                                    }
                                    else
                                    {
                                        if (sb.Length())
                                            node.AddText(xybase::string::to_utf16(sb.ToString()));
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
            error += "Near " + xybase::string::to_string(xml.substr(index, 32)) + ": expected '<', but got '" + static_cast<char>(xml[index]) + "'.\n";
            // 寻找同步点
            index = xml.find('<', index);
            return XmlNodeT::ERROR;
        }
    }
}

#endif /* End of XML_PARSER_H__*/
