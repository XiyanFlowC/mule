#pragma once

#include <Xml/XmlParser.h>
#include <Stream.h>

#include "../mulert_api.h"
#include "../Logger.h"
#include "BinaryData.h"

namespace mule
{
	namespace Storage
	{
		class MULERT_API ResourceManager
		{
			xybase::xml::XmlParser<xybase::xml::XmlNode> xmlParser;

			mule::Logger logger = mule::Logger::GetLogger<ResourceManager>();

		public:

			static ResourceManager &GetInstance();

			mule::Storage::BinaryData LoadResource(std::string path);
		};
	}
}
