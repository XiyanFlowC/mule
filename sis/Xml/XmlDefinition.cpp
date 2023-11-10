#include "XmlDefinition.h"

#include <Storage/ResourceManager.h>
#include <Data/TypeManager.h>
#include <Data/Structure.h>

void mule::Xml::XmlDefinition::Parse(std::u16string name)
{
	xybase::xml::XmlParser<xybase::xml::XmlNode, char8_t> parser;
	auto data = Storage::ResourceManager::GetInstance().LoadResource("definition/" + xybase::string::to_string(name) + ".xml");
	auto def = parser.Parse((const char8_t *)data.GetData());
	if (!parser.error.empty())
	{
		logger.Error(L"Failed to parse definition {}.", xybase::string::to_wstring(name));
		logger.Note(L"Error: {}", xybase::string::to_wstring(parser.error));
		return;
	}

	if (def.GetName() != u"def")
	{
		logger.Error(L"Format error, root element is not 'def'");
		logger.Note(L"Read {}.", xybase::string::to_wstring(def.GetName()));
		return;
	}
	auto version = def.GetAttribute(u"version");
	if (version == u"1")
	{
		ParseVersion1(def);
	}
	else
	{
		ParseVersion1(def);
	}
}

void mule::Xml::XmlDefinition::ParseVersion1(const xybase::xml::XmlNode &node)
{
	for (auto &&item : node.GetChildren())
	{
		if (item.GetName() == u"struct")
		{
			auto name = item.GetAttribute(u"name");
			mule::Data::Structure *type = new mule::Data::Structure(name);
			for (auto &&field : item.GetChildren())
			{
				if (field.GetName() != u"field")
				{
					logger.Error(L"Unexpected tag {}.", xybase::string::to_wstring(field.GetName()));
					continue;
				}

				auto metadata = field.GetChildren();
				std::map<std::u16string, std::u16string> metainfo;
				if (!metadata.empty())
				{
					for (auto &&metadatum : metadata)
					{
						if (metadatum.IsTextNode())
						{
							logger.Fatal(L"XML Definition Format Incorrect: No text allowed in tag field.");
							return;
						}

						metainfo[metadatum.GetName()] = metadatum.GetChildren().begin()->GetText();
					}
				}

				auto ret = mule::Data::TypeManager::GetInstance().GetOrCreateType(field.GetAttribute(u"type"), metainfo);
				if (ret == nullptr)
				{
					logger.Error(L"Invalid type: {} in definition of type {}", xybase::string::to_wstring(field.GetAttribute(u"type")), xybase::string::to_wstring(item.GetAttribute(u"name")));
					continue;
				}
				
				type->AppendField(field.GetAttribute(u"name"), ret);
			}
			Data::TypeManager::GetInstance().RegisterObject(type, item.GetAttribute(u"name"));
		}
		else if (item.GetName() == u"include")
		{
			Parse(item.GetAttribute(u"ref"));
		}
	}
}
