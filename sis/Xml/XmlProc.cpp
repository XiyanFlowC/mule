#include "XmlProc.h"

using namespace mule::Data::Basic;

int mule::Xml::XmlGenerator::ident = 2;

mule::Xml::XmlParser::XmlParser()
{
    RegisterEntity("amp", "&");
    RegisterEntity("lt", "<");
    RegisterEntity("gt", ">");
    RegisterEntity("lf", "\n");
    RegisterEntity("cr", "\r");
    RegisterEntity("sp", " ");
}

mule::Data::Basic::MultiValue mule::Xml::XmlParser::Parse(const std::string &xml)
{
    error = "";
    index = 0;
    return std::get<1>(ParseNode(xml));
}

void mule::Xml::XmlParser::RegisterTagCallback(const std::string &tagName, std::function<std::string(mule::Data::Basic::MultiValue &)> callback)
{
    callbacks[tagName] = callback;
}

void mule::Xml::XmlParser::RegisterEntity(const std::string entityName, const std::string entitySeq)
{
    entities[entityName] = entitySeq;
}

std::tuple<std::string, mule::Data::Basic::MultiValue> mule::Xml::XmlParser::ParseNode(const std::string &xml)
{
    // ȥ���հ��ַ�
    index = xml.find_first_not_of(" \t\n\r", index);
    // �ڵ���ʼ
    if (xml[index] == '<')
    {
        MultiValue node;
        std::string name;

        if (xml[index + 1] == '/')
        {
            error += "Near " + xml.substr(index, 32) + ": Unexpected close tag.\n";
            index = xml.find(">", index) + 1;
            return {"$ERROR", MultiValue::MV_NULL};
        }

        // ������ǩ
        index = xml.find_first_not_of(" \t\n\r", index + 1);
        size_t endIndex = xml.find_first_of(" \t\n\r>/", index);
        name = xml.substr(index, endIndex - index);

        // ��������
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
            node.metadata[attrName] = attrValue;
            endIndex = xml.find_first_of(" \t\n\r>/", attrValueEnd + 1);
        }

        // ���ӽڵ�ڵ�
        if (xml[index] == '/')
        {
            if (xml[index + 1] == '>')
            {
                index += 2;
                return { name, node };
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
            if (xml[index] == '<')
            {
                // �رձ�ǩ
                if (xml[index + 1] == '/')
                {
                    index = xml.find_first_not_of(" \t\n\r", index + 2);
                    size_t endIndex = xml.find_first_of(" \t\n\r>", index);

                    // ����ȵĹرձ�ǩ������
                    if (name != xml.substr(index + 2, endIndex - index))
                    {
                        error += "Near " + xml.substr(index, 32) + ": unpairing with open tag " + name + ", ignoring...\n";
                        index = xml.find('>', index);
                        continue;
                    }
                    index = xml.find('>', index);
                    return { name, node };
                }
                // �ӽڵ㣬����
                else
                {
                    auto ret = ParseNode(xml);
                    (*node.value.mapValue)[std::get<0>(ret)] = std::get<1>(ret);
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
                        if (xml[index + 1] == '#')
                        {
                            // ����ֱ��ֵ
                            error += "Near " + xml.substr(index, 32) + ": unimplemented entity type, ignoring...\n";
                        }
                        // ����ת������
                        size_t escapeStart = index + 1;
                        index = xml.find(';', index);
                        sb += xml.substr(escapeStart, index - escapeStart);
                        index++;
                    }
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
                        // Ϊ������չ�ԣ��˴�������CDATA����Ŀ�
                        if (blockName == "CDATA")
                        {
                            // ����CDATA��
                            size_t cdataStart = blockNameEnd + 1;
                            index = xml.find("]]>", index);
                            sb += xml.substr(cdataStart, index - cdataStart + 3);
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
                    else if (xml[index] == '<')
                    {
                        // �ڵ�ر�
                        if (xml[index + 1] == '/')
                        {
                            index = xml.find_first_not_of(" \t\n\r", index + 2);
                            size_t endIndex = xml.find_first_of(" \t\n\r>", index);

                            // ����ȵĹرձ�ǩ������
                            if (name != xml.substr(index + 2, endIndex - index))
                            {
                                error += "Near " + xml.substr(index, 32) + ": unpairing with open tag " + name + ", ignoring...\n";
                                index = xml.find('>', index);
                                continue;
                            }
                            index = xml.find('>', index);
                            node.SetValue(sb.ToString());
                            return { name, node };
                        }
                        // Ƕ����ӽڵ�
                        else
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
                    }
                    else
                    {
                        sb += xml[index++];
                    }
                }
            }
        }
        return { name, node };
    }
    return { "$ERROR", MultiValue::MV_NULL };
}

mule::Xml::XmlGenerator::XmlGenerator()
{
    callback = [](std::string in) -> std::string {
        auto loc = in.find("]]>");
        while (loc != std::string::npos)
        {
            in = in.replace(loc, 3, "]]>]]><![CDATA[");
            loc = in.find("]]>", loc + 14);
        }
        return "<![CDATA[" + in + "]]>";
    };
}

std::string mule::Xml::XmlGenerator::ToXml(const mule::Data::Basic::MultiValue &value, std::string rootName)
{
    layer = 0;
    xybase::StringBuilder sb;
    sb += "<" + rootName + ">\n";
    ProcNode(value, sb);
    sb += "</" + rootName + ">\n";
    return sb.ToString();
}

void mule::Xml::XmlGenerator::ProcNode(const mule::Data::Basic::MultiValue &value, xybase::StringBuilder<char> &sb)
{
    if (value.GetType() == MultiValue::MVT_MAP)
    {
        for (auto &ele : *value.value.mapValue)
        {
            if (ele.first.GetType() == MultiValue::MVT_UINT)
            {
                for (int i = 1; ; ++i)
                {
                    auto itr = value.value.mapValue->find((unsigned long long)i);
                    if (itr == value.value.mapValue->end()) break;

                    if (ident)
                    {
                        ++layer;
                        for (int i = 0; i < layer * ident; ++i)
                        {
                            sb += ' ';
                        }
                    }
                    sb += "<i>";
                    if (ident) sb += '\n';
                    ProcNode(itr->second, sb);
                    if (ident)
                    {
                        for (int i = 0; i < layer * ident; ++i)
                        {
                            sb += ' ';
                        }
                        --layer;
                    }
                    sb += "</i>";
                    if (ident) sb += '\n';
                }
                break;
            }

            if (ident)
            {
                ++layer;
                for (int i = 0; i < layer * ident; ++i)
                {
                    sb += ' ';
                }
            }
            sb += '<';
            sb += ele.first.ToString();
            sb += ">";
            if (ident) sb += '\n';
            ProcNode(ele.second, sb);
            if (ident)
            {
                for (int i = 0; i < layer * ident; ++i)
                {
                    sb += ' ';
                }
                --layer;
            }
            sb += "</";
            sb += ele.first.ToString();
            sb += ">";
            if (ident) sb += '\n';
        }
    }
    else if (value.GetType() == MultiValue::MVT_STRING)
    {
        if (ident)
        {
            ++layer;
            for (int i = 0; i < layer * ident; ++i)
            {
                sb += ' ';
            }
            --layer;
        }
        sb += callback(*value.value.stringValue);
        if (ident) sb += '\n';
    }
    else
    {
        if (ident)
        {
            ++layer;
            for (int i = 0; i < layer * ident; ++i)
            {
                sb += ' ';
            }
            --layer;
        }
        sb += value.ToString();
        if (ident) sb += '\n';
    }
}
