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
		class ResourceManager
		{
			xybase::xml::XmlParser<xybase::xml::XmlNode> xmlParser;

			mule::Logger logger = mule::Logger::GetLogger<ResourceManager>();

		public:

			MULERT_API static ResourceManager &GetInstance();

			MULERT_API mule::Storage::BinaryData LoadResource(std::string path);

			MULERT_API void SaveResource(std::u16string path, const mule::Storage::BinaryData &data);
		};
	}
}
