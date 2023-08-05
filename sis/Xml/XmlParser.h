#pragma once

#ifndef XML_PARSER_H__
#define XML_PARSER_H__

#include <string>
#include <map>
#include <functional>

#include <xybase/xyutils.h>

namespace mule
{
    namespace Xml
    {
        // �ػ�ǿ��ֹͣ
        template <typename Ch>
        std::basic_string<Ch> to_utf(long) {
            static_assert(sizeof(Ch) == 0, "Unsupported character type in to_utf");
        }

        // utf8 �ػ�
        template <>
        std::string to_utf<char>(long value) {
            return xybase::string::to_utf8(value);
        }

        // utf16 �ػ�
        template <>
        std::u16string to_utf<char16_t>(long value) {
            return xybase::string::to_utf16(value);
        }

        // utf32 �ػ�
        template <>
        std::u32string to_utf<char32_t>(long value) {
            return xybase::string::to_utf32(value);
        }

        /**
         * @brief ����Xml����
         * @tparam XmlNodeT Xml�ڵ�����
         * @tparam Ch Xml�ı����ַ�����
        */
        template <typename XmlNodeT, typename Ch = char>
        class XmlParser
        {
        public:
            XmlParser();

            XmlNodeT Parse(const std::basic_string<Ch> &xml);

            void RegisterTagCallback(const std::basic_string<Ch> &tagName, std::function<std::basic_string<Ch>(XmlNodeT &)> callback);

            void RegisterEntity(const std::basic_string<Ch> entityName, const std::basic_string<Ch> entitySeq);

            std::string error;

            bool mangleEmbeddedNodes;

        private:
            XmlNodeT ParseNode(const std::basic_string<Ch> &xml);

            size_t index;

            std::map<std::basic_string<Ch>, std::function<std::basic_string<Ch>(XmlNodeT &)>> callbacks;

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
            // XML 1.0 Ԥ����ʵ��
            RegisterEntity("amp", "&");
            RegisterEntity("lt", "<");
            RegisterEntity("gt", ">");
            RegisterEntity("apos", "'");
            RegisterEntity("quot", "\"");

            // ��չ��ͨ��ʵ��
            RegisterEntity("lf", "\n");
            RegisterEntity("cr", "\r");
            RegisterEntity("sp", " ");
        }

        template<typename XmlNodeT, typename Ch>
        inline XmlNodeT XmlParser<XmlNodeT, Ch>::Parse(const std::basic_string<Ch> &xml)
        {
            error = "";
            index = 0;
            return ParseNode(xml);
        }

        template<typename XmlNodeT, typename Ch>
        inline void XmlParser<XmlNodeT, Ch>::RegisterTagCallback(const std::basic_string<Ch> &tagName, std::function<std::basic_string<Ch>(XmlNodeT &)> callback)
        {
            callbacks[tagName] = callback;
        }

        template<typename XmlNodeT, typename Ch>
        XmlNodeT XmlParser<XmlNodeT, Ch>::ParseNode(const std::basic_string<Ch> &xml)
        {
            // ȥ���հ��ַ�
            index = xml.find_first_not_of(" \t\n\r", index);
            // �ڵ���ʼ
            if (xml[index] == '<')
            {
                XmlNodeT node{};

                if (xml[index + 1] == '/')
                {
                    error += "Near " + xml.substr(index, 32) + ": Unexpected close tag.\n";
                    index = xml.find(">", index) + 1;
                    return XmlNodeT::ERROR;
                }

                // ������ǩ
                index = xml.find_first_not_of(" \t\n\r", index + 1);
                size_t endIndex = xml.find_first_of(" \t\n\r>/", index);
                node.SetName(xml.substr(index, endIndex - index));

                // ��������
                while (true)
                {
                    index = xml.find_first_not_of(" \t\n\r", endIndex);
                    if (xml[index] == '>' || xml[index] == '/')
                    {
                        break;
                    }
                    size_t attrEndIndex = xml.find('=', index);
                    std::basic_string<Ch> attrName = xml.substr(index, attrEndIndex - index);
                    size_t attrValueStart = xml.find_first_of("\"'", attrEndIndex) + 1;
                    size_t attrValueEnd = xml.find(xml[attrValueStart - 1], attrValueStart);
                    std::basic_string<Ch> attrValue = xml.substr(attrValueStart, attrValueEnd - attrValueStart);
                    node.AddAttribute(attrName, attrValue);
                    endIndex = xml.find_first_of(" \t\n\r>/", attrValueEnd + 1);
                }

                // ��Ԫ�ر�ǩ
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

                // �����ӽڵ�
                while (index < xml.size())
                {
                    // ȥ���հ��ַ�
                    index = xml.find_first_not_of(" \t\n\r", index);
                    // �����ǩ�ڵ�
                    if (xml[index] == '<' && xml[index + 1] != '!')
                    {
                        // ����رձ�ǩ�������ڲ�Ԫ��ȫ���ݹ�������˴��������Ĺرձ�ǩһ���Ǹ�Ԫ�صĹرձ�ǩ
                        if (xml[index + 1] == '/')
                        {
                            index = xml.find_first_not_of(" \t\n\r", index + 2);
                            size_t endIndex = xml.find_first_of(" \t\n\r>", index);

                            // ����ȵĹرձ�ǩ������
                            if (node.GetName() != xml.substr(index, endIndex - index))
                            {
                                error += "Near " + xml.substr(index, 32) + ": unpairing with open tag " + node.GetName() + ", ignoring...\n";
                                index = xml.find('>', index);
                                continue;
                            }
                            index = xml.find('>', index) + 1;
                            return node;
                        }
                        // �ӽڵ㣬�ݹ����
                        else
                        {
                            node.AddChild(ParseNode(xml));
                        }
                    }
                    // �ı��ڵ�
                    else
                    {
                        xybase::StringBuilder sb;
                        while (true)
                        {
                            if (xml[index] == '&')
                            {
                                // ����ת������
                                size_t escapeStart = index + 1;
                                index = xml.find(';', index);
                                std::basic_string<Ch> seq = xml.substr(escapeStart, index - escapeStart);
                                if (seq[0] == '#')
                                {
                                    // ����ֱ��ֵ
                                    if (seq[1] == 'x')
                                    {
                                        sb += to_utf<Ch>(xybase::string::stoi(seq.substr(2), 16));
                                    }
                                    else
                                    {
                                        sb += to_utf<Ch>(xybase::string::stoi(seq.substr(1)));
                                    }
                                }
                                else sb += entities[seq];
                                index++;
                            }
                            // �հ��ַ�ȥβ
                            else if (xml[index] == ' ' || xml[index] == '\t' || xml[index] == '\r' || xml[index] == '\n')
                            {
                                size_t endOfBlank = xml.find_first_not_of(" \t\r\n", index);
                                // ȥβ����������
                                if (xml[endOfBlank] == '<' && xml[endOfBlank + 1] == '/')
                                {
                                    node.AddText(sb.ToString());
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
                                // �ڵ�ر�
                                if (xml[index + 1] == '/')
                                {
                                    node.AddText(sb.ToString());
                                    break;
                                }
                                // XML ����������
                                if (xml[index + 1] == '!')
                                {
                                    if (xml[index + 2] == '[')
                                    {
                                        size_t blockNameStart = index + 3;
                                        size_t blockNameEnd = xml.find("[", index + 3);

                                        auto blockName = xml.substr(blockNameStart, blockNameEnd - blockNameStart);
                                        // Ϊ������չ�ԣ��˴�������CDATA����Ŀ�
                                        if (blockName == "CDATA")
                                        {
                                            // ����CDATA��
                                            size_t cdataStart = blockNameEnd + 1;
                                            index = xml.find("]]>", index);
                                            sb += xml.substr(cdataStart, index - cdataStart);
                                            index += 3;
                                        }
                                        else
                                        {
                                            // ��Ч�飬���ӣ�������
                                            error += "Near " + xml.substr(index, 32) + ": unknown block type " + blockName + ", ignoring...\n";
                                            index = xml.find("]]>", index);
                                            index += 3;
                                        }
                                    }
                                }
                                // ��ǩ��
                                else
                                {
                                    if (mangleEmbeddedNodes)
                                    {
                                        // ͨ���ص���������
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
            // Ѱ��ͬ����
            index = xml.find("<", index);
            return XmlNodeT::ERROR;
        }
    }
}

#endif /* End of XML_PARSER_H__*/
