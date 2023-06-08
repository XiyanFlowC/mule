#pragma once

#ifndef CONFIGURATION_H__
#define CONFIGURATION_H__

#include <string>

class Configuration
{
	Configuration() {}

public:
	static Configuration& GetInstance();

	std::string ScriptDir = "./scripts/";
	std::string CellSeperator = ",";
	std::string RecordSeperator = "\n";

	bool UseLuaDataHandler = false;
};

#endif
