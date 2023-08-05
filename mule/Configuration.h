#pragma once

#ifndef CONFIGURATION_H__
#define CONFIGURATION_H__

#include <string>

class Configuration
{
	Configuration() {}

public:
	static Configuration& GetInstance();

	std::string ScriptsDir = "./res/scripts/";

	std::string DataDir = "./data/";

	std::string SheetsDir = "./res/sheet/";

	std::string ResourceDir = "./res/";

	std::string EnvironmentRootDir = "./";

	void SetEnvironmentRootPath(std::string path);
};

#endif
