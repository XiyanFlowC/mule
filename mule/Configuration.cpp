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
	ResourcesDir = path + "res/";
	ScriptsDir = path + "scripts/";
	SheetsDir = path + "sheets/";
	DataDir = path + "data/";
	DefinitionsDir = path + "def/";
}
