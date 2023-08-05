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
	ResourceDir = path + "res/";
	ScriptsDir = ResourceDir + "scripts/";
	SheetsDir = ResourceDir + "sheets/";
	DataDir = path + "data/";
}
