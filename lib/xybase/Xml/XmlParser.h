#pragma once

#ifndef XY_XML_PARSER_H__
#define XY_XML_PARSER_H__

#include <string>
#include <map>
#include <functional>

#include "../StringBuilder.h"
#include "../xystring.h"
#include "../xyutils.h"
#include "XmlNode.h"

namespace xybase
{
    namespace xml
    {

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

            void RegisterEntity(const std::basic_string<Ch> entityName, const std::basic_string<Ch> entitySeq);

            std::string error;

        private:
            XmlNodeT ParseNode(const std::basic_string<Ch> &xml);

            size_t index;

            std::map<std::basic_string<Ch>, std::basic_string<Ch>> entities;

            // buffered strings for comparing
            std::basic_string<Ch> EMPTY_CHARS;
            std::basic_string<Ch> COMMENT_END;
            std::basic_string<Ch> TAGSTR_END;
            std::basic_string<Ch> CLOSE_TAGSTR_END;
            std::basic_string<Ch> QOUTES;
            std::basic_string<Ch> END_OF_QOUTES;
            std::basic_string<Ch> CDATA_STR;
        };

        template<typename XmlNodeT, typename Ch>
        inline void XmlParser<XmlNodeT, Ch>::RegisterEntity(const std::basic_string<Ch> entityName, const std::basic_string<Ch> entitySeq)
        {
            entities[entityName] = entitySeq;
        }

        template<typename XmlNodeT, typename Ch>
        inline XmlParser<XmlNodeT, Ch>::XmlParser()
        {
            // XML 1.0 预定义实体
            RegisterEntity(xybase::string::to_enc<Ch, char>("amp"), xybase::string::to_enc<Ch, char>("&"));
            RegisterEntity(xybase::string::to_enc<Ch, char>("lt"), xybase::string::to_enc<Ch, char>("<"));
            RegisterEntity(xybase::string::to_enc<Ch, char>("gt"), xybase::string::to_enc<Ch, char>(">"));
            RegisterEntity(xybase::string::to_enc<Ch, char>("apos"), xybase::string::to_enc<Ch, char>("'"));
            RegisterEntity(xybase::string::to_enc<Ch, char>("quot"), xybase::string::to_enc<Ch, char>("\""));

            EMPTY_CHARS = xybase::string::to_enc<Ch, char>(" \t\n\r");
            COMMENT_END = xybase::string::to_enc<Ch, char>("-->");
            TAGSTR_END = xybase::string::to_enc<Ch, char>(" \t\n\r>/");
            CLOSE_TAGSTR_END = xybase::string::to_enc<Ch, char>(" \t\n\r>");
            QOUTES = xybase::string::to_enc<Ch, char>("'\"");
            END_OF_QOUTES = xybase::string::to_enc<Ch, char>("]]>");
            CDATA_STR = xybase::string::to_enc<Ch, char>("CDATA");

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
        XmlNodeT XmlParser<XmlNodeT, Ch>::ParseNode(const std::basic_string<Ch> &xml)
        {
            // 去除空白字符
            index = xml.find_first_not_of(EMPTY_CHARS, index);
            
            // 检查是否已经到达字符串末尾
            if (index == std::basic_string<Ch>::npos || index >= xml.size()) {
                error += "Unexpected end of input while parsing.\n";
                return XmlNodeT::ERROR;
            }

            // 去除顶层注释（喵的越写越乱了，找时间换成开源Xml解析库得了
            while (index + 3 < xml.size() && xml[index] == '<' && xml[index + 1] == '!' && xml[index + 2] == '-' && xml[index + 3] == '-')
            {
                size_t commentEnd = xml.find(COMMENT_END, index + 4);
                if (commentEnd == std::basic_string<Ch>::npos) {
                    error += "Unclosed comment found.\n";
                    return XmlNodeT::ERROR;
                }
                index = commentEnd + 3;
                index = xml.find_first_not_of(EMPTY_CHARS, index);
                if (index == std::basic_string<Ch>::npos || index >= xml.size()) {
                    error += "Unexpected end of input after comment.\n";
                    return XmlNodeT::ERROR;
                }
            }

            // 检查是否还有足够的字符进行节点解析
            if (index >= xml.size()) {
                error += "Unexpected end of input while parsing node.\n";
                return XmlNodeT::ERROR;
            }

            // 节点起始
            if (xml[index] == '<')
            {
                XmlNodeT node{};

                // 检查是否有足够的字符检查闭合标签
                if (index + 1 >= xml.size()) {
                    error += "Unexpected end of input after '<'.\n";
                    return XmlNodeT::ERROR;
                }

                if (xml[index + 1] == '/')
                {
                    error += "Near " + xybase::string::to_string(xml.substr(index, std::min(static_cast<size_t>(32), xml.size() - index))) + ": Unexpected close tag.\n";
                    size_t closePos = xml.find('>', index);
                    if (closePos == std::basic_string<Ch>::npos) {
                        index = xml.size();
                    } else {
                        index = closePos + 1;
                    }
                    return XmlNodeT::ERROR;
                }

                // 开启标签
                index = xml.find_first_not_of(EMPTY_CHARS, index + 1);
                if (index == std::basic_string<Ch>::npos || index >= xml.size()) {
                    error += "Unexpected end of input while parsing tag name.\n";
                    return XmlNodeT::ERROR;
                }
                
                size_t endIndex = xml.find_first_of(TAGSTR_END, index);
                if (endIndex == std::basic_string<Ch>::npos) {
                    error += "Malformed tag, no closing found.\n";
                    return XmlNodeT::ERROR;
                }
                node.SetName(xybase::string::to_utf16(xml.substr(index, endIndex - index)));

                // 处理特性
                while (index < xml.length())
                {
                    index = xml.find_first_not_of(EMPTY_CHARS, endIndex);
                    if (index == std::basic_string<Ch>::npos || index >= xml.size()) {
                        error += "Unexpected end of input while parsing attributes.\n";
                        return XmlNodeT::ERROR;
                    }
                    
                    if (xml[index] == '>' || xml[index] == '/')
                    {
                        break;
                    }
                    
                    size_t attrEndIndex = xml.find('=', index);
                    if (attrEndIndex == std::basic_string<Ch>::npos) {
                        error += "Malformed attribute, no '=' found.\n";
                        return XmlNodeT::ERROR;
                    }
                    
                    std::basic_string<Ch> attrName = xml.substr(index, attrEndIndex - index);
                    size_t attrValueStart = xml.find_first_of(QOUTES, attrEndIndex);
                    if (attrValueStart == std::basic_string<Ch>::npos || attrValueStart + 1 >= xml.size()) {
                        error += "Malformed attribute value, no quote found.\n";
                        return XmlNodeT::ERROR;
                    }
                    attrValueStart += 1;
                    
                    size_t attrValueEnd = xml.find(xml[attrValueStart - 1], attrValueStart);
                    if (attrValueEnd == std::basic_string<Ch>::npos) {
                        error += "Unclosed attribute value.\n";
                        return XmlNodeT::ERROR;
                    }
                    
                    std::basic_string<Ch> attrRawValue = xml.substr(attrValueStart, attrValueEnd - attrValueStart);
                    StringBuilder<Ch> attrSb;
                    for (size_t ai = 0; ai < attrRawValue.size(); ++ai)
                    {
                        if (attrRawValue[ai] == '&')
                        {
                            // 处理转义序列
                            size_t escapeStart = ai + 1;
                            size_t semicolonPos = attrRawValue.find(';', ai);
                            if (semicolonPos == std::basic_string<Ch>::npos) {
                                error += "Malformed entity reference, no ';' found.\n";
                                attrSb += attrRawValue[ai];
                                continue;
                            }
                            ai = semicolonPos;
                            std::basic_string<Ch> seq = attrRawValue.substr(escapeStart, ai - escapeStart);
                            if (seq.empty()) {
                                attrSb += '&';
                                continue;
                            }
                            if (seq[0] == '#')
                            {
                                if (seq.size() < 2) {
                                    attrSb += '&';
                                    attrSb += seq;
                                    attrSb += ';';
                                    continue;
                                }
                                long codepoint;
                                // 处理直接值
                                if (seq[1] == 'x')
                                {
                                    if (seq.size() < 3) {
                                        attrSb += '&';
                                        attrSb += seq;
                                        attrSb += ';';
                                        continue;
                                    }
                                    codepoint = static_cast<long>(xybase::string::stoi(seq.substr(2), 16));
                                }
                                else
                                {
                                    codepoint = static_cast<long>(xybase::string::stoi(seq.substr(1)));
                                }
                                attrSb += xybase::string::to_enc<Ch, char32_t>(xybase::string::to_utf32(codepoint));
                            }
                            else {
                                auto entityIt = entities.find(seq);
                                if (entityIt != entities.end()) {
                                    attrSb += entityIt->second;
                                } else {
                                    // 未知实体，保持原样
                                    attrSb += '&';
                                    attrSb += seq;
                                    attrSb += ';';
                                }
                            }
                        }
                        else
                            attrSb += attrRawValue[ai];
                    }
                    node.AddAttribute(xybase::string::to_utf16(attrName), xybase::string::to_utf16(attrSb.ToString()));
                    endIndex = xml.find_first_of(TAGSTR_END, attrValueEnd + 1);
                    if (endIndex == std::basic_string<Ch>::npos) {
                        endIndex = xml.size();
                    }
                }

                // 空元素标签
                if (index >= xml.size()) {
                    error += "Unexpected end of input while parsing tag end.\n";
                    return XmlNodeT::ERROR;
                }
                
                if (xml[index] == '/')
                {
                    if (index + 1 >= xml.size()) {
                        error += "Unexpected end of input after '/'.\n";
                        return XmlNodeT::ERROR;
                    }
                    
                    if (xml[index + 1] == '>')
                    {
                        index += 2;
                        return node;
                    }

                    error += "Near " + xybase::string::to_string(xml.substr(index, std::min(static_cast<size_t>(32), xml.size() - index))) + ": unexpected character '/', ignoring...\n";
                    size_t closePos = xml.find('>', index);
                    if (closePos == std::basic_string<Ch>::npos) {
                        index = xml.size();
                    } else {
                        index = closePos;
                    }
                }
                else // xml[index] == '>'
                {
                    index++;
                }

                // 处理子节点
                while (index < xml.size())
                {
                    // 去除空白字符
                    index = xml.find_first_not_of(EMPTY_CHARS, index);
                    if (index == std::basic_string<Ch>::npos || index >= xml.size()) {
                        break;
                    }

                    // 处理标签节点
                    if (xml[index] == '<' && index + 1 < xml.size() && xml[index + 1] != '!')
                    {
                        // 处理关闭标签：由于内部元素全部递归解析，此处能遇到的关闭标签一定是该元素的关闭标签
                        if (xml[index + 1] == '/')
                        {
                            index = xml.find_first_not_of(EMPTY_CHARS, index + 2);
                            if (index == std::basic_string<Ch>::npos || index >= xml.size()) {
                                error += "Unexpected end of input in close tag.\n";
                                return XmlNodeT::ERROR;
                            }
                            
                            size_t endIndex = xml.find_first_of(CLOSE_TAGSTR_END, index);
                            if (endIndex == std::basic_string<Ch>::npos) {
                                error += "Malformed close tag.\n";
                                return XmlNodeT::ERROR;
                            }

                            // 非相等的关闭标签，忽略
                            if (xybase::string::to_utf16(node.GetName()) != xybase::string::to_utf16(xml.substr(index, endIndex - index)))
                            {
                                error += "Near " + xybase::string::to_string(xml.substr(index, std::min(static_cast<size_t>(32), xml.size() - index))) + ": unpairing with open tag " + xybase::string::to_string(node.GetName()) + ", ignoring...\n";
                                size_t closePos = xml.find('>', index);
                                if (closePos == std::basic_string<Ch>::npos) {
                                    index = xml.size();
                                } else {
                                    index = closePos;
                                }
                                continue;
                            }
                            size_t closePos = xml.find('>', index);
                            if (closePos == std::basic_string<Ch>::npos) {
                                error += "Unclosed close tag.\n";
                                return XmlNodeT::ERROR;
                            }
                            index = closePos + 1;
                            return node;
                        }
                        // 子节点，递归解析
                        else
                        {
                            node.AddChild(ParseNode(xml));
                        }
                    }
                    // 处理注释
                    else if (index + 3 < xml.size() && xml[index] == '<' && xml[index + 1] == '!' && xml[index + 2] == '-' && xml[index + 3] == '-')
                    {
                        size_t commentEnd = xml.find(COMMENT_END, index + 4);
                        if (commentEnd == std::basic_string<Ch>::npos) {
                            error += "Unclosed comment.\n";
                            return XmlNodeT::ERROR;
                        }
                        index = commentEnd;
                        index = xml.find_first_not_of(EMPTY_CHARS, index + 3);
                        if (index == std::basic_string<Ch>::npos) {
                            break;
                        }
                    }
                    // 文本节点
                    else
                    {
                        xybase::StringBuilder<Ch> sb;
                        while (index < xml.size())
                        {
                            if (xml[index] == '&')
                            {
                                // 处理转义序列
                                size_t escapeStart = index + 1;
                                size_t semicolonPos = xml.find(';', index);
                                if (semicolonPos == std::basic_string<Ch>::npos) {
                                    error += "Malformed entity reference, no ';' found.\n";
                                    sb += xml[index];
                                    index++;
                                    continue;
                                }
                                index = semicolonPos;
                                std::basic_string<Ch> seq = xml.substr(escapeStart, index - escapeStart);
                                if (seq.empty()) {
                                    sb += '&';
                                    index++;
                                    continue;
                                }
                                if (seq[0] == '#')
                                {
                                    if (seq.size() < 2) {
                                        sb += '&';
                                        sb += seq;
                                        sb += ';';
                                        index++;
                                        continue;
                                    }
                                    long codepoint;
                                    // 处理直接值
                                    if (seq[1] == 'x')
                                    {
                                        if (seq.size() < 3) {
                                            sb += '&';
                                            sb += seq;
                                            sb += ';';
                                            index++;
                                            continue;
                                        }
                                        codepoint = static_cast<long>(xybase::string::stoi(seq.substr(2), 16));
                                    }
                                    else
                                    {
                                        codepoint = static_cast<long>(xybase::string::stoi(seq.substr(1)));
                                    }
                                    sb += xybase::string::to_enc<Ch, char32_t>(xybase::string::to_utf32(codepoint));
                                }
                                else {
                                    auto entityIt = entities.find(seq);
                                    if (entityIt != entities.end()) {
                                        sb += entityIt->second;
                                    } else {
                                        // 未知实体，保持原样
                                        sb += '&';
                                        sb += seq;
                                        sb += ';';
                                    }
                                }
                                index++;
                            }
                            // 空白字符去尾
                            else if (xml[index] == ' ' || xml[index] == '\t' || xml[index] == '\r' || xml[index] == '\n')
                            {
                                size_t endOfBlank = xml.find_first_not_of(EMPTY_CHARS, index);
                                // 去尾，立即返回
                                if (endOfBlank != std::basic_string<Ch>::npos && 
                                    endOfBlank + 1 < xml.size() && 
                                    xml[endOfBlank] == '<' && xml[endOfBlank + 1] == '/')
                                {
                                    node.AddText(xybase::string::to_utf16(sb.ToString()));
                                    index = endOfBlank;
                                    break;
                                }
                                else
                                {
                                    if (endOfBlank == std::basic_string<Ch>::npos) {
                                        sb += xml.substr(index);
                                        index = xml.size();
                                    } else {
                                        sb += xml.substr(index, endOfBlank - index);
                                        index = endOfBlank;
                                    }
                                }
                            }
                            else if (xml[index] == '<')
                            {
                                // 节点关闭
                                if (index + 1 < xml.size() && xml[index + 1] == '/')
                                {
                                    node.AddText(xybase::string::to_utf16(sb.ToString()));
                                    break;
                                }
                                // XML 解析器命令
                                if (index + 1 < xml.size() && xml[index + 1] == '!')
                                {
                                    if (index + 2 < xml.size() && xml[index + 2] == '[')
                                    {
                                        size_t blockNameStart = index + 3;
                                        size_t blockNameEnd = xml.find('[', index + 3);
                                        if (blockNameEnd == std::basic_string<Ch>::npos) {
                                            error += "Malformed CDATA section.\n";
                                            sb += xml[index];
                                            index++;
                                            continue;
                                        }

                                        auto blockName = xml.substr(blockNameStart, blockNameEnd - blockNameStart);
                                        // 为保持拓展性，此处允许处理CDATA以外的块
                                        if (blockName == CDATA_STR)
                                        {
                                            // 处理CDATA块
                                            size_t cdataStart = blockNameEnd + 1;
                                            size_t cdataEnd = xml.find(END_OF_QOUTES, index);
                                            if (cdataEnd == std::basic_string<Ch>::npos) {
                                                error += "Unclosed CDATA section.\n";
                                                sb += xml[index];
                                                index++;
                                                continue;
                                            }
                                            sb += xml.substr(cdataStart, cdataEnd - cdataStart);
                                            index = cdataEnd + 3;
                                        }
                                        else
                                        {
                                            // 无效块，忽视，不处理
                                            error += "Near " + xybase::string::to_string(xml.substr(index, std::min(static_cast<size_t>(32), xml.size() - index))) + ": unknown block type " + xybase::string::to_string(blockName) + ", ignoring...\n";
                                            size_t blockEnd = xml.find(END_OF_QOUTES, index);
                                            if (blockEnd == std::basic_string<Ch>::npos) {
                                                index = xml.size();
                                            } else {
                                                index = blockEnd + 3;
                                            }
                                        }
                                    }
                                    else if (index + 3 < xml.size() && xml[index + 2] == '-' && xml[index + 3] == '-')
                                    {
                                        size_t commentEnd = xml.find(COMMENT_END, index + 4);
                                        if (commentEnd == std::basic_string<Ch>::npos) {
                                            error += "Unclosed comment in text.\n";
                                            sb += xml[index];
                                            index++;
                                            continue;
                                        }
                                        index = commentEnd;
                                        index = xml.find_first_not_of(EMPTY_CHARS, index + 3);
                                        if (index == std::basic_string<Ch>::npos) {
                                            index = xml.size();
                                        }
                                    }
                                    else
                                    {
                                        sb += xml[index];
                                        index++;
                                    }
                                }
                                // 标签打开
                                else
                                {
                                    if (sb.Length())
                                        node.AddText(xybase::string::to_utf16(sb.ToString()));
                                    break;
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
            error += "Near " + xybase::string::to_string(xml.substr(index, std::min(static_cast<size_t>(32), xml.size() - index))) + ": expected '<', but got '" + static_cast<char>(xml[index >= xml.size() ? xml.size() - 1 : index]) + "'.\n";
            // 寻找同步点
            size_t syncPos = xml.find('<', index);
            if (syncPos == std::basic_string<Ch>::npos) {
                index = xml.size();
            } else {
                index = syncPos;
            }
            return XmlNodeT::ERROR;
        }
    }
}

#endif /* End of XY_XML_PARSER_H__*/
