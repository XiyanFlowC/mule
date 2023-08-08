#include "mule.h"

using namespace mule::Lua;
using namespace mule::Data;
using namespace mule::Data::Basic;

extern luaL_Reg mulefuncs[];
extern void setRootStream(xybase::Stream *stream);

int main(int argc, char **argv)
{
	if (argc <= 3)
	{
		fprintf(stderr, "Usage: %s <path_to_execute_folder> <path_to_target_file(s)> <action> [options...]", argv[0]);
		exit(-1);
	}

	// Initialisation
	Configuration::GetInstance().SetEnvironmentRootPath(argv[1]);
	Configuration::GetInstance().TargetFile = argv[2];
	crc32_init();

	LuaHost::GetInstance().LoadLuaStandardLibs();
	LuaHost::GetInstance().SetGlobal("package.path", MultiValue(Configuration::GetInstance().ScriptsDir + "?.lua;" + Configuration::GetInstance().ScriptsDir + "?/init.lua"));

	LuaHost::GetInstance().RunScript((Configuration::GetInstance().ScriptsDir + "config.lua").c_str());

	LuaHost::GetInstance().RegisterLibrary(mulefuncs);

	TypeManager::GetInstance().RegisterObjectCreator(new BasicFieldCreator());
	TypeManager::GetInstance().RegisterObjectCreator(new Referrence::ReferrenceCreator());
	TypeManager::GetInstance().RegisterObjectCreator(new Array::ArrayCreator());
	TypeManager::GetInstance().RegisterObjectCreator(new VarChar::VarCharCreator());

	mule::Xml::XmlParser<mule::Xml::XmlNode> xmlParser;
	std::string infoFile(ResourceManager::GetInstance().LoadData(Configuration::GetInstance().dataInfoFile).GetData());

	setRootStream(new mule::BinaryStream(Configuration::GetInstance().TargetFile.c_str()));

	// Execution
	try
	{
		LuaHost::GetInstance().RunScript((Configuration::GetInstance().ScriptsDir + argv[3] + ".lua").c_str());
	}
	catch (mule::Lua::LuaException ex)
	{
		fputs("Error when execute lua script.\n", stderr);
		fputs(ex.what(), stderr);
	}

	return 0;
}
