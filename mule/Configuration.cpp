#include "Configuration.h"

Configuration& Configuration::GetInstance()
{
	static Configuration inst;
	return inst;
}

void Configuration::SetEnvironmentRootPath(std::string path)
{
	if (!path.ends_with('/') && !path.ends_with('\\'))
	{
		path += "/";
	}

	EnvironmentRootDir = path;
	ScriptsDir = path + "scripts/";
	DataDir = path + "data/";
	SheetsDir = path + "sheets/";
}
