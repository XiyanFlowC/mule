#pragma once

#ifndef RESOURCE_MANAGER_H__
#define RESOURCE_MANAGER_H__

#include <cstdio>

#include <map>
#include <string>
#include <memory>
#include <functional>

#include <Stream.h>
#include <xyutils.h>
#include <Exception/IOException.h>
#include <Exception/RuntimeException.h>
#include <Storage/BinaryData.h>
#include <Data/TypeManager.h>
#include <Data/Structure.h>
#include <Xml/XmlParser.h>
#include <Xml/XmlNode.h>

#include "Configuration.h"
#include "crc32.h"

#include <Logger.h>

class ResourceManager
{
	mule::Xml::XmlParser<mule::Xml::XmlNode> xmlParser;

	mule::Logger logger = mule::Logger::GetLogger<ResourceManager>();
	
public:

	static ResourceManager &GetInstance();

	mule::Storage::BinaryData LoadResource(std::string path);

	xybase::Stream *OpenResource(std::string path, std::function<xybase::Stream *(std::string path)> creator);

	void LoadDefinition(std::string def);

	std::string LoadSheet(std::string sheetName);

	void SaveSheet(std::string sheetName, std::string sheet);
};

#endif
