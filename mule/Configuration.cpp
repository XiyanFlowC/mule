#include "Configuration.h"

#include <Data/Storage/DataManager.h>
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
	mule::Data::Storage::DataManager::GetInstance().Initialisation(xybase::string::to_utf16(DataDir));
	DefinitionsDir = path + "def/";
}
