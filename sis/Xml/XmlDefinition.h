#pragma once

#include <Xml/XmlParser.h>
#include <Logger.h>

namespace mule
{
	namespace Xml
	{
		class XmlDefinition
		{
			mule::Logger logger = mule::Logger{ "<xmldef>", -1 };
		public:
			void Parse(std::u16string name);

			void ParseVersion1(const xybase::xml::XmlNode &node);
		};
	}
}
