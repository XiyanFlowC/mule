#include "XmlDefinition.h"

#include <Storage/ResourceManager.h>
#include <Data/TypeManager.h>
#include <Data/Structure.h>
#include <Data/Basic/Enum.h>

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
	else if (version == u"1.1")
	{
		ParseVersion1(def, 1);
	}
	else if (version == u"")
	{
		ParseVersion1(def);
	}
	else
	{
		throw xybase::InvalidParameterException(L"name", L"Specified XML definition version is grater than support!", 52654);
	}
}

void mule::Xml::XmlDefinition::ParseVersion1(const xybase::xml::XmlNode &node, int subversion)
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
		else if (item.GetName() == u"enum")
		{
			if (subversion < 1)
			{
				logger.Warn(L"version smaller than 1.1, but contains enum. check if it is an error.");
			}

			auto name = item.GetAttribute(u"name");
			auto base = item.GetAttribute(u"base");
			auto e = base == u"" ?
				new mule::Data::Basic::Enum(name) :
				new mule::Data::Basic::Enum(name, base);

			mule::Data::Basic::MultiValue v = 0;
			for (auto &&i : item.GetChildren())
			{
				if (i.GetName() == u"constraint")
				{
					for (auto &&t : i.GetChildren())
					{
						using mule::Data::Basic::MultiValue;
						using mule::Data::Basic::BasicType;
						auto &&cons = t.GetText();
						auto condition = cons.substr(0, cons.find(':'));
						e->comparator = MultiValue::Parse(cons.substr(cons.find(':') + 1));
						if (condition == u"lt") e->constraintType = BasicType::BTCT_LT;
						if (condition == u"nlt") e->constraintType = BasicType::BTCT_NLT;
						if (condition == u"gt") e->constraintType = BasicType::BTCT_GT;
						if (condition == u"ngt") e->constraintType = BasicType::BTCT_NGT;
						if (condition == u"eq") e->constraintType = BasicType::BTCT_EQ;
						if (condition == u"neq") e->constraintType = BasicType::BTCT_NEQ;
					}
				}
				else if (i.GetName() != u"i")
				{
					logger.Error(L"Unexpected tag {}.", xybase::string::to_wstring(i.GetName()));
					continue;
				}

				if (i.GetChildren().size() != 1)
				{
					logger.Error(L"Too many element in enum definition.");
					continue;
				}
				for (auto &&t : i.GetChildren())
				{
					if (!t.IsTextNode())
					{
						logger.Error(L"Non-text node in <i> tag of <enum>.");
						continue;
					}

					if (i.GetAttribute(u"value") != u"")
					{
						v = mule::Data::Basic::MultiValue::Parse(i.GetAttribute(u"value"));
					}

					e->AddEnum(t.GetText(), v);
					logger.Debug(L"{} -> {}", xybase::string::to_wstring(t.GetText()), v.ToString());
					v = v + 1;
				}
			}

			Data::TypeManager::GetInstance().RegisterObject(e, name);
		}
		else if (item.GetName() == u"include")
		{
			Parse(item.GetAttribute(u"ref"));
		}
	}
}
