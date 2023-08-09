#include "mule.h"

using namespace mule::Lua;
using namespace mule::Data;
using namespace mule::Data::Basic;

void InitialiseLuaEnvironment(xybase::Stream *);

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

	TypeManager::GetInstance().RegisterObjectCreator(new BasicFieldCreator());
	TypeManager::GetInstance().RegisterObjectCreator(new Referrence::ReferrenceCreator());
	TypeManager::GetInstance().RegisterObjectCreator(new Array::ArrayCreator());
	TypeManager::GetInstance().RegisterObjectCreator(new VarChar::VarCharCreator());

	mule::Xml::XmlParser<mule::Xml::XmlNode> xmlParser;
	std::string infoFile(ResourceManager::GetInstance().LoadData(Configuration::GetInstance().dataInfoFile).GetData());

	InitialiseLuaEnvironment(new mule::BinaryStream(Configuration::GetInstance().TargetFile.c_str()));

	// Execution
	try
	{
		if (argv[3] == std::string{"interact"})
		{
			puts("=====================================");
			//puts("MULE who f**ked ArTonelico");
			//puts("MULE User-configurable Large-file Editor");
			puts("MULE is a Utility for Limited Editing");
			puts("=====================================");
			puts("Interaction Mode [Lua based]");
			while (true)
			{
				printf("Lua> ");
				static char buffer[4096] = {'r', 'e', 't', 'u', 'r', 'n', ' '};
				scanf("%[^\n]", buffer + 7);
				getchar();
				if (strcmp(buffer, "quit") == 0) break;
				try
				{
					auto && ret = LuaHost::GetInstance().RunString(buffer);
					if (ret.GetType() == MultiValue::MVT_ARRAY)
					{
						for (int i = 0; i < ret.GetLength(); ++i)
						{
							printf(" %d=> %s\n", i, ret.value.arrayValue[i].ToString().c_str());
						}
					}
					else printf("  => %s\n", ret.ToString().c_str());
				}
				catch (LuaException &ex)
				{
					fprintf(stderr, "Lua Error: %s\n\n", ex.what());
				}
				catch (mule::Exception::Exception &ex)
				{
					fprintf(stderr, "Error when executing %s:\n%s\n\n", buffer, ex.what());
				}
				LuaHost::GetInstance().SetStackTop(0);
			}
			puts("Bye!");
		}
		else
		{
			LuaHost::GetInstance().RunScript((Configuration::GetInstance().ScriptsDir + argv[3] + ".lua").c_str());
		}
	}
	catch (mule::Lua::LuaException ex)
	{
		fputs("Error when execute lua script.\n", stderr);
		fputs(ex.what(), stderr);
	}

	return 0;
}
