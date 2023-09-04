#pragma once

#ifndef CONFIGURATION_H__
#define CONFIGURATION_H__

#include <string>

class Configuration
{
	Configuration() {}

public:
	static Configuration& GetInstance();

	std::string ScriptsDir = "./scripts/";

	std::string DataDir = "./data/";

	std::string SheetsDir = "./sheet/";

	std::string ResourcesDir = "./res/";

	std::string DefinitionsDir = "./def/";

	std::string EnvironmentRootDir = "./";

	std::string TargetFile = "./foo.bin";

	void SetEnvironmentRootPath(std::string path);
};

#endif
