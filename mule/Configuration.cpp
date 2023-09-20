#include "Configuration.h"

#include <Storage/DataManager.h>
#include <xystring.h>

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
	ScriptsDir = path + "scr/";
	SheetsDir = path + "sht/";
	DataDir = path + "dat/";
	mule::Storage::DataManager::GetInstance().Initialise(xybase::string::to_utf16(DataDir).c_str());
	DefinitionsDir = path + "def/";
}
